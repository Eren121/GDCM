// vtkGdcmWriter.cxx
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

#include "gdcmHeader.h"
#include "gdcmFile.h"
#include "gdcmDebug.h"
#include "vtkGdcmWriter.h"

#include <vtkObjectFactory.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>

vtkCxxRevisionMacro(vtkGdcmWriter, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkGdcmWriter);

//-----------------------------------------------------------------------------
// Constructor / Destructor
vtkGdcmWriter::vtkGdcmWriter()
{
   this->LookupTable = NULL;
}

vtkGdcmWriter::~vtkGdcmWriter()
{
}

//-----------------------------------------------------------------------------
// Print
void vtkGdcmWriter::PrintSelf(ostream& os, vtkIndent indent)
{
   this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected
/**
 * Copy the image and reverse the Y axis
 */
// The output datas must be deleted by the user of the method !!!
size_t ReverseData(vtkImageData *img,unsigned char **data)
{
   int *dim = img->GetDimensions();
   size_t lineSize = dim[0] * img->GetScalarSize()
                   * img->GetNumberOfScalarComponents();
   size_t planeSize = dim[1] * lineSize;
   size_t size = dim[2] * planeSize;

   *data = new unsigned char[size];

   unsigned char *src = (unsigned char *)img->GetScalarPointer();
   unsigned char *dst = *data + planeSize - lineSize;
   for (int plane = 0; plane < dim[2]; plane++)
   {
      for (int line = 0; line < dim[1]; line++)
      {
         // Copy one line at proper destination:
         memcpy((void*)dst, (void*)src, lineSize);

         src += lineSize;
         dst -= lineSize;
      }
      dst += 2 * planeSize;
   }

   return size;
}

/**
 * Set the datas informations in the file
 */
void SetImageInformation(gdcm::File *file,vtkImageData *image)
{
   std::ostringstream str;

   // Image size
   int *dim = image->GetDimensions();

   str.str("");
   str<<dim[0];
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0011);

   str.str("");
   str<<dim[1];
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0010);

   if(dim[2]>1)
   {
      str.str("");
      str<<dim[2];
      file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0012);
   }

   // Pixel type
   str.str("");
   str<<image->GetScalarSize()*8;
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0100);
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0101);

   str.str("");
   str<<image->GetScalarSize()*8-1;
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0102);

   // Pixel Repr
   // FIXME : what do we do when the ScalarType is 
   // VTK_UNSIGNED_INT or VTK_UNSIGNED_LONG
   str.str("");
   if( image->GetScalarType() == VTK_UNSIGNED_CHAR ||
       image->GetScalarType() == VTK_UNSIGNED_SHORT ||
       image->GetScalarType() == VTK_UNSIGNED_INT ||
       image->GetScalarType() == VTK_UNSIGNED_LONG )
   {
      str<<"0"; // Unsigned
   }
   else
   {
      str<<"1"; // Signed
   }
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0103);

   // Samples per pixel
   str.str("");
   str<<image->GetNumberOfScalarComponents();
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0002);

   // Spacing
   double *sp = image->GetSpacing();

   str.str("");
   str<<sp[0]<<"\\"<<sp[1];
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0030);
   str.str("");
   str<<sp[2];
   file->ReplaceOrCreateByNumber(str.str(),0x0018,0x0088);

   // Origin
   double *org = image->GetOrigin();

   str.str("");
   str<<org[0]<<"\\"<<org[1]<<"\\"<<org[2];
   file->ReplaceOrCreateByNumber(str.str(),0x0020,0x0032);

   // Window / Level
   double *rng=image->GetScalarRange();

   str.str("");
   str<<rng[1]-rng[0];
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x1051);
   str.str("");
   str<<(rng[1]+rng[0])/2.0;
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x1050);

   // Pixels
   unsigned char *data;
   size_t size = ReverseData(image,&data);
   file->SetImageData(data,size);
}

/**
 * Write of the files
 * The call to this method is recursive if there is some files to write
 */ 
void vtkGdcmWriter::RecursiveWrite(int axis, vtkImageData *image, ofstream *file)
{
   if(file)
   {
      vtkErrorMacro(<< "File musn't be opened");
      return;
   }

   if( image->GetScalarType() == VTK_FLOAT || 
       image->GetScalarType() == VTK_DOUBLE )
   {
      vtkErrorMacro(<< "Bad input type. Scalar type musn't be of type "
                    << "VTK_FLOAT or VTKDOUBLE (found:"
                    << image->GetScalarTypeAsString());
      return;
   }

   WriteFile(this->FileName,image);
}

void vtkGdcmWriter::WriteFile(char *fileName,vtkImageData *image)
{
   // From here, the write of the file begins
   gdcm::File *dcmFile = new gdcm::File();

   // Set the image informations
   SetImageInformation(dcmFile,image);

   // Write the image
   if(!dcmFile->Write(FileName))
   {
      vtkErrorMacro(<< "File " << this->FileName << "couldn't be written by "
                    << " the gdcm library");
      std::cerr<<"not written \n";
   }

   delete dcmFile;
}

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
