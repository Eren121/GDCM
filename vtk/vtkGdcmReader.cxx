// vtkGdcmReader.cxx
//-----------------------------------------------------------------------------
// //////////////////////////////////////////////////////////////
// WARNING TODO CLEANME 
// Actual limitations of this code:
//
// /////// Redundant and unnecessary header parsing
// In it's current state this code actually parses three times the Dicom
// header of a file before the corresponding image gets loaded in the
// ad-hoc vtkData !
// Here is the process:
//  1/ First loading happens in ExecuteInformation which in order to
//     positionate the vtk extents calls CheckFileCoherence. The purpose
//     of CheckFileCoherence is to make sure all the images in the future
//     stack are "homogenous" (same size, same representation...). This
//     can only be achieved by parsing all the Dicom headers...
//  2/ ExecuteData is then responsible for the next two loadings:
//  2a/ ExecuteData calls AllocateOutputData that in turn seems to 
//      (indirectely call) ExecuteInformation which ends up in a second
//      header parsing
//      This is fixed by adding a test at the beginning of ExecuteInformation
//      on the modification of the object instance. If a modification have been
//      made (method Modified() ), the MTime value is increased. The fileTime
//      is compared to this new value to find a modification in the class
//      parameters
//  2b/ the core of ExecuteData then needs gdcmFile (which in turns
//      initialises gdcmHeader in the constructor) in order to access
//      the data-image.
//
// Possible solution:
// maintain a list of gdcmFiles (created by say ExecuteInformation) created
// once and for all accross the life of vtkGdcmHeader (it would only load
// new gdcmFile if the user changes the list). ExecuteData would then use 
// those gdcmFile and hence avoid calling the construtor:
//  - advantage: the header of the files would only be parser once.
//  - drawback: once execute information is called (i.e. on creation of
//              a vtkGdcmHeader) the gdcmFile structure is loaded in memory.
//              The average size of a gdcmHeader being of 100Ko, is one
//              loads 10 stacks of images with say 200 images each, you
//              end-up with a loss of 200Mo...
//
// /////// Never unallocated memory:
// ExecuteData allocates space for the pixel data [which will get pointed
// by the vtkPointData() through the call
// data->GetPointData()->GetScalars()->SetVoidArray(mem, StackNumPixels, 0);]
// This data is never "freed" neither in the destructor nor when the
// filename list is extended, ExecuteData is called a second (or third)
// time...
// //////////////////////////////////////////////////////////////

#include "gdcmFile.h"
#include "gdcmHeader.h"
#include "vtkGdcmReader.h"

//#include <stdio.h>
#include <vtkObjectFactory.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>

vtkCxxRevisionMacro(vtkGdcmReader, "$Revision: 1.61 $");
vtkStandardNewMacro(vtkGdcmReader);

//-----------------------------------------------------------------------------
// Constructor / Destructor
vtkGdcmReader::vtkGdcmReader()
{
   this->LookupTable = NULL;
   this->AllowLookupTable = 0;
}

vtkGdcmReader::~vtkGdcmReader()
{
   this->RemoveAllFileName();
   this->InternalFileNameList.clear();
   if(this->LookupTable) 
      this->LookupTable->Delete();
}

//-----------------------------------------------------------------------------
// Print
void vtkGdcmReader::PrintSelf(ostream& os, vtkIndent indent)
{
   this->Superclass::PrintSelf(os,indent);
   os << indent << "Filenames  : " << endl;
   vtkIndent nextIndent = indent.GetNextIndent();
   for (std::list<std::string>::iterator it = FileNameList.begin();
        it != FileNameList.end();
        ++it)
   {
      os << nextIndent << it->c_str() << endl ;
   }
}

//-----------------------------------------------------------------------------
// Public
/*
 * Remove all files from the list of images to read.
 */
void vtkGdcmReader::RemoveAllFileName(void)
{
   this->FileNameList.clear();
   this->Modified();
}

/*
 * Adds a file name to the list of images to read.
 */
void vtkGdcmReader::AddFileName(const char* name)
{
   // We need to bypass the const pointer [since list<>.push_bash() only
   // takes a char* (but not a const char*)] by making a local copy:
   char * LocalName = new char[strlen(name) + 1];
   strcpy(LocalName, name);
   this->FileNameList.push_back(LocalName);
   delete[] LocalName;
   this->Modified();
}

/*
 * Sets up a filename to be read.
 */
void vtkGdcmReader::SetFileName(const char *name) 
{
   vtkImageReader2::SetFileName(name);
   // Since we maintain a list of filenames, when building a volume,
   // (see vtkGdcmReader::AddFileName), we additionaly need to purge
   // this list when we manually positionate the filename.
   vtkDebugMacro(<< "Clearing all files given with AddFileName");
   this->FileNameList.clear();
   this->Modified();
}

//-----------------------------------------------------------------------------
// Protected
/*
 * Configure the output e.g. WholeExtent, spacing, origin, scalar type...
 */
void vtkGdcmReader::ExecuteInformation()
{
   if(this->MTime>this->fileTime)
   {
      this->TotalNumberOfPlanes = this->CheckFileCoherence();
      if ( this->TotalNumberOfPlanes == 0)
      {
         vtkErrorMacro(<< "File set is not coherent. Exiting...");
         return;
      }
      
      // if the user has not set the extent, but has set the VOI
      // set the z axis extent to the VOI z axis
      if (this->DataExtent[4]==0 && this->DataExtent[5] == 0 &&
      (this->DataVOI[4] || this->DataVOI[5]))
      {
         this->DataExtent[4] = this->DataVOI[4];
         this->DataExtent[5] = this->DataVOI[5];
      }

      // When the user has set the VOI, check it's coherence with the file content.
      if (this->DataVOI[0] || this->DataVOI[1] || 
      this->DataVOI[2] || this->DataVOI[3] ||
      this->DataVOI[4] || this->DataVOI[5])
      { 
         if ((this->DataVOI[0] < 0) ||
             (this->DataVOI[1] >= this->NumColumns) ||
             (this->DataVOI[2] < 0) ||
             (this->DataVOI[3] >= this->NumLines) ||
             (this->DataVOI[4] < 0) ||
             (this->DataVOI[5] >= this->TotalNumberOfPlanes ))
         {
            vtkWarningMacro(<< "The requested VOI is larger than expected extent.");
            this->DataVOI[0] = 0;
            this->DataVOI[1] = this->NumColumns - 1;
            this->DataVOI[2] = 0;
            this->DataVOI[3] = this->NumLines - 1;
            this->DataVOI[4] = 0;
            this->DataVOI[5] = this->TotalNumberOfPlanes - 1;
         }
      }

      // Positionate the Extent.
      this->DataExtent[0] = 0;
      this->DataExtent[1] = this->NumColumns - 1;
      this->DataExtent[2] = 0;
      this->DataExtent[3] = this->NumLines - 1;
      this->DataExtent[4] = 0;
      this->DataExtent[5] = this->TotalNumberOfPlanes - 1;
  
      // We don't need to positionate the Endian related stuff (by using
      // this->SetDataByteOrderToBigEndian() or SetDataByteOrderToLittleEndian()
      // since the reading of the file is done by gdcm.
      // But we do need to set up the data type for downstream filters:
      if      ( ImageType == "8U" )
      {
         vtkDebugMacro(<< "8 bits unsigned image");
         this->SetDataScalarTypeToUnsignedChar(); 
      }
      else if ( ImageType == "8S" )
      {
         vtkErrorMacro(<< "Cannot handle 8 bit signed files");
         return;
      }
      else if ( ImageType == "16U" )
      {
         vtkDebugMacro(<< "16 bits unsigned image");
         this->SetDataScalarTypeToUnsignedShort();
      }
      else if ( ImageType == "16S" )
      {
         vtkDebugMacro(<< "16 bits signed image");
         this->SetDataScalarTypeToShort();
         //vtkErrorMacro(<< "Cannot handle 16 bit signed files");
      }
      else if ( ImageType == "32U" )
      {
         vtkDebugMacro(<< "32 bits unsigned image");
         vtkDebugMacro(<< "WARNING: forced to signed int !");
         this->SetDataScalarTypeToInt();
      }
      else if ( ImageType == "32S" )
      {
         vtkDebugMacro(<< "32 bits signed image");
         this->SetDataScalarTypeToInt();
      }
      else if ( ImageType == "FD" )
      {
         vtkDebugMacro(<< "64 bits Double image");
         this->SetDataScalarTypeToDouble();
      }
      //Set number of scalar components:
      this->SetNumberOfScalarComponents(this->NumComponents);

      this->fileTime=this->MTime;
   }

   this->Superclass::ExecuteInformation();
}

/*
 * Update => ouput->Update => UpdateData => Execute => ExecuteData 
 * (see vtkSource.cxx for last step).
 * This function (redefinition of vtkImageReader::ExecuteData, see 
 * VTK/IO/vtkImageReader.cxx) reads a data from a file. The datas
 * extent/axes are assumed to be the same as the file extent/order.
 */
void vtkGdcmReader::ExecuteData(vtkDataObject *output)
{
   if (this->InternalFileNameList.empty())
   {
      vtkErrorMacro(<< "A least a valid FileName must be specified.");
      return;
   }

   // FIXME : extraneous parsing of header is made when allocating OuputData
   vtkImageData *data = this->AllocateOutputData(output);
   data->SetExtent(this->DataExtent);
   data->GetPointData()->GetScalars()->SetName("DicomImage-Volume");

   // Test if output has valid extent
   // Prevent memory errors
   if((this->DataExtent[1]-this->DataExtent[0]>=0) &&
      (this->DataExtent[3]-this->DataExtent[2]>=0) &&
      (this->DataExtent[5]-this->DataExtent[4]>=0))
   {
      // The memory size for a full stack of images of course depends
      // on the number of planes and the size of each image:
      //size_t StackNumPixels = this->NumColumns * this->NumLines
      //                      * this->TotalNumberOfPlanes * this->NumComponents;
      //size_t stack_size = StackNumPixels * this->PixelSize; //not used
      // Allocate pixel data space itself.

      // Variables for the UpdateProgress. We shall use 50 steps to signify
      // the advance of the process:
      unsigned long UpdateProgressTarget = (unsigned long) ceil (this->NumLines
                                         * this->TotalNumberOfPlanes
                                         / 50.0);
      // The actual advance measure:
      unsigned long UpdateProgressCount = 0;

      // Feeling the allocated memory space with each image/volume:
      unsigned char *Dest = (unsigned char *)data->GetScalarPointer();
      for (std::list<std::string>::iterator filename  = InternalFileNameList.begin();
           filename != InternalFileNameList.end();
           ++filename)
      { 
         // Images that were tagged as unreadable in CheckFileCoherence()
         // are substituted with a black image to let the caller visually
         // notice something wrong is going on:
         if (*filename != "GDCM_UNREADABLE")
         {
            // Update progress related for good files is made in LoadImageInMemory
            Dest += this->LoadImageInMemory(*filename, Dest,
                                            UpdateProgressTarget,
                                            UpdateProgressCount);
         } 
         else 
         {
            // We insert a black image in the stack for the user to be aware that
            // this image/volume couldn't be loaded. We simply skip one image
            // size:
            Dest += this->NumColumns * this->NumLines * this->PixelSize;

            // Update progress related for bad files:
            UpdateProgressCount += this->NumLines;
            if (UpdateProgressTarget > 0)
            {
               if (!(UpdateProgressCount%UpdateProgressTarget))
               {
                  this->UpdateProgress(UpdateProgressCount/(50.0*UpdateProgressTarget));
               }
            }
         } // Else, file not loadable
      } // Loop on files
   }
}

/*
 * vtkGdcmReader can have the file names specified through two ways:
 * (1) by calling the vtkImageReader2::SetFileName(), SetFilePrefix() and
 *     SetFilePattern()
 * (2) By successive calls to vtkGdcmReader::AddFileName()
 * When the first method was used by caller we need to update the local
 * filename list
 */
void vtkGdcmReader::BuildFileListFromPattern()
{
   this->RemoveAllInternalFileName();

   if ((! this->FileNameList.empty()) && this->FileName )
   {
      vtkErrorMacro(<< "Both AddFileName and SetFileName schemes were used");
      vtkErrorMacro(<< "No images loaded ! ");
      return;
   }

   if ((! this->FileNameList.empty()) && this->FilePrefix )
   {
      vtkErrorMacro(<< "Both AddFileName and SetFilePrefix schemes were used");
      vtkErrorMacro(<< "No images loaded ! ");
      return;
   }

   if (this->FileName && this->FilePrefix)
   {
      vtkErrorMacro(<< "Both SetFileName and SetFilePrefix schemes were used");
      vtkErrorMacro(<< "No images loaded ! ");
      return;
   }

   if (! this->FileNameList.empty()  )
   {
      vtkDebugMacro(<< "Using the AddFileName specified files");
      this->InternalFileNameList=this->FileNameList;
      return;
   }

   if (!this->FileName && !this->FilePrefix)
   {
      vtkErrorMacro(<< "FileNames are not set. Either use AddFileName() or");
      vtkErrorMacro(<< "specify a FileName or FilePrefix.");
      return;
   }

   if( this->FileName )
   {
      // Single file loading (as given with ::SetFileName()):
      // Case of multi-frame file considered here
      this->ComputeInternalFileName(this->DataExtent[4]);
      vtkDebugMacro(<< "Adding file " << this->InternalFileName);
      this->AddInternalFileName(this->InternalFileName);
   }
   else
   {
      // Multi file loading (as given with ::SetFilePattern()):
      for (int idx = this->DataExtent[4]; idx <= this->DataExtent[5]; ++idx)
      {
         this->ComputeInternalFileName(idx);
         vtkDebugMacro(<< "Adding file " << this->InternalFileName);
         this->AddInternalFileName(this->InternalFileName);
      }
   }
}

/*
 * When more than one filename is specified (i.e. we expect loading
 * a stack or volume) we need to check that the corresponding images/volumes
 * to be loaded are coherent i.e. to make sure:
 *     - they all share the same X dimensions
 *     - they all share the same Y dimensions
 *     - they all share the same ImageType ( 8 bit signed, or unsigned...)
 *
 * Eventually, we emit a warning when all the files do NOT share the
 * Z dimension, since we can still build a stack but the
 * files are not coherent in Z, which is probably a source a trouble...
 *   When files are not readable (either the file cannot be opened or
 * because gdcm cannot parse it), they are flagged as "GDCM_UNREADABLE".  
 *   This method returns the total number of planar images to be loaded
 * (i.e. an image represents one plane, but a volume represents many planes)
 */
int vtkGdcmReader::CheckFileCoherence()
{
   int ReturnedTotalNumberOfPlanes = 0;   // The returned value.

   this->BuildFileListFromPattern();
   if (this->InternalFileNameList.empty())
   {
      vtkErrorMacro(<< "FileNames are not set.");
      return 0;
   }

   bool FoundReferenceFile = false;
   int  ReferenceNZ = 0;

   // Loop on the filenames:
   // - check for their existence and gdcm "parsability"
   // - get the coherence check done:
   for (std::list<std::string>::iterator filename = InternalFileNameList.begin();
        filename != InternalFileNameList.end();
        ++filename)
   {
      // The file is always added in the number of planes
      //  - If file doesn't exist, it will be replaced by a black plane in the 
      //    ExecuteData method
      //  - If file has more than 1 plane, other planes will be added later to
      //    to the ReturnedTotalNumberOfPlanes variable counter
      ReturnedTotalNumberOfPlanes += 1;

      /////// Stage 0: check for file name:
      if(*filename == std::string("GDCM_UNREADABLE"))
         continue;

      /////// Stage 1: check for file readability:
      // Stage 1.1: check for file existence.
      FILE *fp;
      fp = fopen(filename->c_str(),"rb");
      if (!fp)
      {
         vtkErrorMacro(<< "Unable to open file " << filename->c_str());
         vtkErrorMacro(<< "Removing this file from readed files "
                     << filename->c_str());
         *filename = "GDCM_UNREADABLE";
         continue;
      }
      fclose(fp);

      // Stage 1.2: check for Gdcm parsability
      gdcm::Header GdcmHeader(filename->c_str() );
      if (!GdcmHeader.IsReadable())
      {
         vtkErrorMacro(<< "Gdcm cannot parse file " << filename->c_str());
         vtkErrorMacro(<< "Removing this file from readed files "
                        << filename->c_str());
         *filename = "GDCM_UNREADABLE";
         continue;
      }

      // Stage 1.3: further gdcm compatibility on PixelType
      std::string type = GdcmHeader.GetPixelType();
      if (   (type !=  "8U") && (type !=  "8S")
          && (type != "16U") && (type != "16S")
          && (type != "32U") && (type != "32S") )
      {
         vtkErrorMacro(<< "Bad File Type for file " << filename->c_str() << "\n"
                       << "   File type found : " << type.c_str() 
                       << " (might be 8U, 8S, 16U, 16S, 32U, 32S) \n"
                       << "   Removing this file from readed files");
         *filename = "GDCM_UNREADABLE";
         continue;
      }

      // Stage 2: check coherence of the set of files
      int NX = GdcmHeader.GetXSize();
      int NY = GdcmHeader.GetYSize();
      int NZ = GdcmHeader.GetZSize();
      if (FoundReferenceFile) 
      {
         // Stage 2.1: mandatory coherence stage:
         if (   ( NX   != this->NumColumns )
             || ( NY   != this->NumLines )
             || ( type != this->ImageType ) ) 
         {
            vtkErrorMacro(<< "This file is not coherent with previous ones"
                           << filename->c_str());
            vtkErrorMacro(<< "Removing this file from readed files "
                           << filename->c_str());
            *filename = "GDCM_UNREADABLE";
            continue;
         }

         // Stage 2.2: optional coherence stage
         if ( NZ != ReferenceNZ )
         {
            vtkErrorMacro(<< "File is not coherent in Z with previous ones"
                           << filename->c_str());
         }
         else
         {
            vtkDebugMacro(<< "File is coherent with previous ones"
                           << filename->c_str());
         }

         // Stage 2.3: when the file contains a volume (as opposed to an image),
         // notify the caller.
         if (NZ > 1)
         {
            vtkErrorMacro(<< "This file contains multiple planes (images)"
                           << filename->c_str());
         }

         // Eventually, this file can be added on the stack. Update the
         // full size of the stack
         vtkDebugMacro("Number of planes added to the stack: " << NZ);
         ReturnedTotalNumberOfPlanes += NZ - 1; // First plane already added
         continue;

      } 
      else 
      {
         // We didn't have a workable reference file yet. Set this one
         // as the reference.
         FoundReferenceFile = true;
         vtkDebugMacro(<< "This file taken as coherence reference:"
                        << filename->c_str());
         vtkDebugMacro(<< "Image dimension of reference file as read from Gdcm:" 
                        << NX << " " << NY << " " << NZ);
         vtkDebugMacro(<< "Number of planes added to the stack: " << NZ);
         // Set aside the size of the image
         this->NumColumns = NX;
         this->NumLines   = NY;
         ReferenceNZ      = NZ;
         ReturnedTotalNumberOfPlanes += NZ - 1; // First plane already added
         this->ImageType = type;
         this->PixelSize = GdcmHeader.GetPixelSize();

         if( GdcmHeader.HasLUT() && this->AllowLookupTable )
         {
            // I could raise an error is AllowLookupTable is on and HasLUT() off
            this->NumComponents = GdcmHeader.GetNumberOfScalarComponentsRaw();
         }
         else
         {
            this->NumComponents = GdcmHeader.GetNumberOfScalarComponents(); //rgb or mono
         }
       
         //Set image spacing
         this->DataSpacing[0] = GdcmHeader.GetXSpacing();
         this->DataSpacing[1] = GdcmHeader.GetYSpacing();
         this->DataSpacing[2] = GdcmHeader.GetZSpacing();

         //Set image origin
         this->DataOrigin[0] = GdcmHeader.GetXOrigin();
         this->DataOrigin[1] = GdcmHeader.GetYOrigin();
         this->DataOrigin[2] = GdcmHeader.GetZOrigin();

      }
   } // End of loop on filename

   ///////// The files we CANNOT load are flaged. On debugging purposes
   // count the loadable number of files and display their number:
   int NumberCoherentFiles = 0;
   for (std::list<std::string>::iterator it = InternalFileNameList.begin();
        it != InternalFileNameList.end();
        ++it)
   {
      if (*it != "GDCM_UNREADABLE")
      {
         NumberCoherentFiles++;
      }
   }
   vtkDebugMacro(<< "Number of coherent files: " << NumberCoherentFiles);

   if (ReturnedTotalNumberOfPlanes == 0)
   {
      vtkErrorMacro(<< "No loadable file.");
   }

   vtkDebugMacro(<< "Total number of planes on the stack: "
                  << ReturnedTotalNumberOfPlanes);
   
   return ReturnedTotalNumberOfPlanes;
}

//-----------------------------------------------------------------------------
// Private
/*
 * Remove all file names to the internal list of images to read.
 */
void vtkGdcmReader::RemoveAllInternalFileName(void)
{
   this->InternalFileNameList.clear();
}

/*
 * Adds a file name to the internal list of images to read.
 */
void vtkGdcmReader::AddInternalFileName(const char* name)
{
   char * LocalName = new char[strlen(name) + 1];
   strcpy(LocalName, name);
   this->InternalFileNameList.push_back(LocalName);
   delete[] LocalName;
}

/*
 * Loads the contents of the image/volume contained by Filename at
 * the Dest memory address. Returns the size of the data loaded.
 */
size_t vtkGdcmReader::LoadImageInMemory(
             std::string fileName, 
             unsigned char * dest,
             const unsigned long updateProgressTarget,
             unsigned long & updateProgressCount)
{
   vtkDebugMacro(<< "Copying to memory image [" << fileName.c_str() << "]");
   gdcm::File file( fileName.c_str() );
   size_t size;

   // If the data structure of vtk for image/volume representation
   // were straigthforwards the following would be enough:
   //    GdcmFile.GetImageDataIntoVector((void*)Dest, size);
   // But vtk chooses to invert the lines of an image, that is the last
   // line comes first (for some axis related reasons?). Hence we need
   // to load the image line by line, starting from the end.

   int numColumns = file.GetHeader()->GetXSize();
   int numLines   = file.GetHeader()->GetYSize();
   int numPlanes  = file.GetHeader()->GetZSize();
   int lineSize   = NumComponents * numColumns * file.GetHeader()->GetPixelSize();

   unsigned char * source;
   
   if( file.GetHeader()->HasLUT() && AllowLookupTable )
   {
      size               = file.GetImageDataSize();
      source             = (unsigned char*) file.GetImageDataRaw();
      unsigned char *lut = (unsigned char*) file.GetLutRGBA();

      if(!this->LookupTable)
      {
         this->LookupTable = vtkLookupTable::New();
      }

      this->LookupTable->SetNumberOfTableValues(256);
      for (int tmp=0; tmp<256; tmp++)
      {
         this->LookupTable->SetTableValue(tmp,
         (float)lut[4*tmp+0]/255.0,
         (float)lut[4*tmp+1]/255.0,
         (float)lut[4*tmp+2]/255.0,
         1);
      }
      this->LookupTable->SetRange(0,255);
      vtkDataSetAttributes *a = this->GetOutput()->GetPointData();
      a->GetScalars()->SetLookupTable(this->LookupTable);
      free(lut);
   }
   else
   {
      size        = file.GetImageDataSize();
      source      = (unsigned char*)file.GetImageData();
   } 
   
   unsigned char * destination = dest + size - lineSize;

   for (int plane = 0; plane < numPlanes; plane++)
   {
      for (int line = 0; line < numLines; line++)
      {
         // Copy one line at proper destination:
         memcpy((void*)destination, (void*)source, lineSize);
         source      += lineSize;
         destination -= lineSize;
         // Update progress related:
         if (!(updateProgressCount%updateProgressTarget))
         {
            this->UpdateProgress(updateProgressCount/(50.0*updateProgressTarget));
         }
         updateProgressCount++;
      }
   }
   
// DO NOT remove this commented out code .
// Nobody knows what's expecting you ...
// Just to 'see' what was actually read on disk :-(

//   FILE * f2;
//   f2 = fopen("SpuriousFile.RAW","wb");
//   fwrite(Dest,size,1,f2);
//   fclose(f2); 
   
   return size;
}

//-----------------------------------------------------------------------------
