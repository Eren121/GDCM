// $Header: /cvs/public/gdcm/vtk/vtkGdcmReader.cxx,v 1.6 2003/05/29 16:58:24 frog Exp $
//CLEANME#include <vtkByteSwap.h>
#include <stdio.h>
#include <vtkObjectFactory.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include "vtkGdcmReader.h"
#include "gdcm.h"

vtkGdcmReader::vtkGdcmReader()
{
  // Constructor
}

//----------------------------------------------------------------------------
vtkGdcmReader::~vtkGdcmReader()
{ 
  // FIXME free memory
}

//----------------------------------------------------------------------------
// Adds a file name to the list of images to read.
void vtkGdcmReader::AddFileName(const char* name)
{
   // We need to bypass the const pointer [since list<>.push_bash() only
   // takes a char* (but not a const char*)] by making a local copy:
   char * LocalName = new char[strlen(name) + 1];
   strcpy(LocalName, name);
   this->FileNameList.push_back(LocalName);
   // Starting from two files we have a stack of images:
   if(this->FileNameList.size() >= 2)
      this->SetFileDimensionality(3);
}

//----------------------------------------------------------------------------
// Sets up a filename to be read.
void vtkGdcmReader::SetFileName(const char *name) {
   vtkImageReader2::SetFileName(name);
   // Since we maintain a list of filenames (when building a volume)
   // we additionaly need to maintain this list. First we clean-up the
   // list and then positionate the incoming filename:
   this->FileNameList.empty();
   this->AddFileName(name);
}

//----------------------------------------------------------------------------
// vtkGdcmReader can have the file names specified through two ways:
// (1) by calling the vtkImageReader2::SetFileName(), SetFilePrefix() and
//     SetFilePattern()
// (2) By successive calls to vtkGdcmReader::SetFileName()
// When the first method was used by caller we need to update the local
// filename list
void vtkGdcmReader::BuilFileListFromPattern()
{
   if (! this->FileNameList.empty())
      return;
   if (!this->FileName && !this->FilePattern)
     {
     vtkErrorMacro("FileNames are not set. Either use AddFileName() or");
     vtkErrorMacro("specify a FileName or FilePattern.");
     return;
     }
   for (int idx = this->DataExtent[4]; idx <= this->DataExtent[5]; ++idx)
     {
     this->ComputeInternalFileName(idx);
     vtkDebugMacro("Adding file " << this->InternalFileName);
     this->AddFileName(this->InternalFileName);
     }
}

//----------------------------------------------------------------------------
// When more than one filename is specified (i.e. we expect loading
// a stack or volume) we need to check the corresponding images are
// coherent:
//  - they all share the same X dimensions
//  - they all share the same Y dimensions
//  - each file a Z dimension of 1
//  - they all share the same type ( 8 bit signed, or unsigned...)
bool vtkGdcmReader::CheckFileCoherence()
{
   this->BuilFileListFromPattern();
   if (this->FileNameList.empty())
     {
     vtkErrorMacro("FileNames are not set.");
     return false;
     }
   if (this->FileNameList.size() == 1)
     {
     vtkDebugMacro("Single file specified.");
     return true;
     }

   // Loop on the filenames:
   // - check for their existence and gdcm "parasability"
   // - get the coherence check done:
   bool FoundReferenceFile = false;
   int ReferenceNX;
   int ReferenceNY;
   int ReferenceNZ;
   std::string ReferenceType;
   for (std::list<std::string>::iterator FileName  = FileNameList.begin();
                                        FileName != FileNameList.end();
                                      ++FileName)
     {
     // Check for file existence.
     FILE *fp;
     fp = fopen(FileName->c_str(),"rb");
     if (!fp)
       {
       vtkErrorMacro("Unable to open file " << *FileName);
       vtkErrorMacro("Removing this file from readed files " << *FileName);
       FileNameList.remove(*FileName);
       continue;
       }
     fclose(fp);
   
     // Check for Gdcm parsability
     gdcmHeader GdcmHeader(FileName->c_str());
     if (!GdcmHeader.IsReadable())
       {
       vtkErrorMacro("Gdcm cannot parse file " << *FileName);
       vtkErrorMacro("Removing this file from readed files " << *FileName);
       FileNameList.remove(*FileName);
       continue;
       }

     // Coherence stage:
     int NX = GdcmHeader.GetXSize();
     int NY = GdcmHeader.GetYSize();
     int NZ = GdcmHeader.GetZSize();
     std::string type = GdcmHeader.GetPixelType();
     if (FoundReferenceFile) 
       {
       if (   ( NX != ReferenceNX )
           || ( NY != ReferenceNY )
           || ( NZ != ReferenceNZ )
           || ( type != ReferenceType ) ) 
         {
            vtkErrorMacro("This file is not coherent with previous ones"
                          << *FileName);
            vtkErrorMacro("Removing this file from readed files " << *FileName);
            FileNameList.remove(*FileName);
            continue;
         } else {
            vtkDebugMacro("File is coherent with previous ones" << *FileName);
         }
       } else {
         // This file shall be the reference:
         FoundReferenceFile = true;
         ReferenceNX = NX;
         ReferenceNY = NY;
         ReferenceNZ = NZ;
         ReferenceType = type;
         vtkDebugMacro("File is taken a coherence references" << *FileName);
       }
     } // End of loop on FileName

   if (this->FileNameList.empty())
     {
     vtkDebugMacro("No gdcm parsable file.");
     return false;
     }
   if (this->FileNameList.size() == 1)
     {
     vtkDebugMacro("Single parsable file left after coherence test.");
     return true;
     }
   return true;
}

//----------------------------------------------------------------------------
// Configure the output e.g. WholeExtent, spacing, origin, scalar type...
void vtkGdcmReader::ExecuteInformation()
{
  //FIXME free any old memory
      
  // if the user has not set the extent, but has set the VOI
  // set the zaxis extent to the VOI z axis
  if (this->DataExtent[4]==0 && this->DataExtent[5] == 0 &&
     (this->DataVOI[4] || this->DataVOI[5]))
    {
    this->DataExtent[4] = this->DataVOI[4];
    this->DataExtent[5] = this->DataVOI[5];
    }

  this->ComputeInternalFileName(this->DataExtent[4]);
  
  // Check for file existence.
  FILE *fp;
  fp = fopen(this->InternalFileName,"rb");
  if (!fp)
    {
    vtkErrorMacro("Unable to open file " << this->InternalFileName);
    return;
    }
  fclose(fp);

  // Check for Gdcm parsability
  gdcmHeader GdcmHeader(this->InternalFileName);
  if (!GdcmHeader.IsReadable())
    {
    vtkErrorMacro("Gdcm cannot parse file " << this->InternalFileName);
    return;
    }

  int NX = GdcmHeader.GetXSize();
  int NY = GdcmHeader.GetYSize();
  int NZ = GdcmHeader.GetZSize();
  vtkDebugMacro("Image dimension as read from Gdcm:" <<
                NX << " " << NY << " " << NZ);

  if(NZ>1) this->SetFileDimensionality(3);

  // When the user has set the VOI, check it's coherence with the file content.
  if (this->DataVOI[0] || this->DataVOI[1] || 
      this->DataVOI[2] || this->DataVOI[3] ||
      this->DataVOI[4] || this->DataVOI[5])
    { 
    if ((this->DataVOI[0] < 0) ||
        (this->DataVOI[1] >= NX) ||
        (this->DataVOI[2] < 0) ||
        (this->DataVOI[3] >= NY) ||
        (this->DataVOI[4] < 0) ||
        (this->DataVOI[5] >= NZ))
      {
      vtkWarningMacro("The requested VOI is larger than the file's ("
                      << this->InternalFileName << ") extent ");
      this->DataVOI[0] = 0;
      this->DataVOI[1] = NX - 1;
      this->DataVOI[2] = 0;
      this->DataVOI[3] = NY - 1;
      this->DataVOI[4] = 0;
      this->DataVOI[5] = NZ - 1;
      }
    }

  // Positionate the Extent.
  this->DataExtent[0] = 0;
  this->DataExtent[1] = NX - 1;
  this->DataExtent[2] = 0;
  this->DataExtent[3] = NY - 1;
  if(this->GetFileDimensionality()==3)
    {
      this->DataExtent[4] = 0;
      this->DataExtent[5] = NZ - 1;
    }
  
  // We don't need to positionate the Endian related stuff (by using
  // this->SetDataByteOrderToBigEndian() or SetDataByteOrderToLittleEndian()
  // since the reading of the file is done by gdcm.
  // But we do need to set up the data type for downstream filters:
  std::string type = GdcmHeader.GetPixelType();
  if      ( type == "8U" )
    {
    vtkDebugMacro("8 bits unsigned image");
    this->SetDataScalarTypeToUnsignedChar(); 
    }
  else if ( type == "8S" )
    {
    vtkErrorMacro("Cannot handle 8 bit signed files");
    return;
    }
  else if ( type == "16U" )
    {
    vtkDebugMacro("16 bits unsigned image");
    this->SetDataScalarTypeToUnsignedShort();
    }
  else if ( type == "16S" )
    {
    vtkDebugMacro("16 bits signed image");
    this->SetDataScalarTypeToShort();
    //vtkErrorMacro("Cannot handle 16 bit signed files");
    }
  else if ( type == "32U" )
    {
    vtkDebugMacro("32 bits unsigned image");
    vtkDebugMacro("WARNING: forced to signed int !");
    this->SetDataScalarTypeToInt();
    }
  else if ( type == "32S" )
    {
    vtkDebugMacro("32 bits signed image");
    this->SetDataScalarTypeToInt();
    }
  else
    {
    vtkErrorMacro("Bad File Type " << this->InternalFileName
                                   << "Type " << type.c_str());
    return;
    }

  vtkImageReader::ExecuteInformation();
}

//----------------------------------------------------------------------------
// Update -> UpdateData -> Execute -> ExecuteData (see vtkSource.cxx for
// last step.
// This function (redefinition of vtkImageReader::ExecuteData, see 
// VTK/IO/vtkImageReader.cxx) reads a data from a file. The datas
// extent/axes are assumed to be the
// same as the file extent/order.
void vtkGdcmReader::ExecuteData(vtkDataObject *output)
{
  if (!this->FileName)
    {
    vtkErrorMacro("A valid FileName must be specified.");
    return;
    }

  vtkImageData *data = this->AllocateOutputData(output);
  data->SetExtent(this->DataExtent);
  data->GetPointData()->GetScalars()->SetName("ImageFile");

  // First check the coherence between the DataExtent and the
  // size of the pixel data as annouced by gdcm (looks a bit paranoid).
  gdcmFile GdcmFile(this->InternalFileName);
  int NumColumns = this->DataExtent[1] - this->DataExtent[0] + 1;
  int NumLines   = this->DataExtent[3] - this->DataExtent[2] + 1;
  int NumPlanes  = this->DataExtent[5] - this->DataExtent[4] + 1;
  size_t size = NumColumns * NumLines * NumPlanes * GdcmFile.GetPixelSize();
  if ( size != GdcmFile.GetImageDataSize() )
    {
    vtkDebugMacro("Inconsistency with GetImageDataSize");
    vtkDebugMacro("Number of scalar components"
                  << this->NumberOfScalarComponents);
    }
  // Allocate pixel data space itself.
  unsigned char *mem = new unsigned char [size];

  // If the data structure of vtk for image/volume representation
  // were straigthforwards the following would suffice:
  //    GdcmFile.GetImageDataIntoVector((void*)mem, size);
  // But vtk chose to invert the lines of an image, that is the last
  // line comes first (for some axis related reasons?). Hence we need
  // to load the image line by line, starting from the end:
  int LineSize = NumColumns * GdcmFile.GetPixelSize();
  unsigned char * Source      = (unsigned char*)GdcmFile.GetImageData();
  unsigned char * Destination = mem + size - LineSize;
  for (int i = 0; i < NumLines; i++)
    {
    memcpy((void*)Destination, (void*)Source, LineSize);
    Source      += LineSize;
    Destination -= LineSize;
    }
  // The "size" of the vtkScalars data is expressed in number of points,
  // and is not the memory size representing those points:
  size = size / GdcmFile.GetPixelSize();
  data->GetPointData()->GetScalars()->SetVoidArray(mem, size, 0);
  this->Modified();

}

//----------------------------------------------------------------------------
void vtkGdcmReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageReader::PrintSelf(os,indent);
  os << indent << "Filenames  : " << endl;
  vtkIndent nextIndent = indent.GetNextIndent();
  for (std::list<std::string>::iterator FileName  = FileNameList.begin();
                                        FileName != FileNameList.end();
                                      ++FileName)
    {
    os << nextIndent << *FileName << endl ;
    }
}
