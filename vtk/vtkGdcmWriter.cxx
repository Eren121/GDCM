/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkGdcmWriter.cxx,v $
  Language:  C++
  Date:      $Date: 2004/12/09 11:31:52 $
  Version:   $Revision: 1.5 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
                                                                                
#include "gdcmHeader.h"
#include "gdcmFile.h"
#include "gdcmDebug.h"
#include "vtkGdcmWriter.h"

#include <vtkObjectFactory.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>

vtkCxxRevisionMacro(vtkGdcmWriter, "$Revision: 1.5 $");
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
   str << dim[0];
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0011);

   str.str("");
   str << dim[1];
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0010);

   if(dim[2]>1)
   {
      str.str("");
      str << dim[2];
      file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0012);
   }

   // Pixel type
   str.str("");
   str << image->GetScalarSize()*8;
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0100);
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0101);

   str.str("");
   str << image->GetScalarSize()*8-1;
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
      str << "0"; // Unsigned
   }
   else
   {
      str << "1"; // Signed
   }
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0103);

   // Samples per pixel
   str.str("");
   str << image->GetNumberOfScalarComponents();
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0002);

   // Spacing
   double *sp = image->GetSpacing();

   str.str("");
   str << sp[0] << "\\" << sp[1];
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x0030);
   str.str("");
   str << sp[2];
   file->ReplaceOrCreateByNumber(str.str(),0x0018,0x0088);

   // Origin
   double *org = image->GetOrigin();

   str.str("");
   str << org[0] << "\\" << org[1] << "\\" << org[2];
   file->ReplaceOrCreateByNumber(str.str(),0x0020,0x0032);

   // Window / Level
   double *rng=image->GetScalarRange();

   str.str("");
   str << rng[1]-rng[0];
   file->ReplaceOrCreateByNumber(str.str(),0x0028,0x1051);
   str.str("");
   str << (rng[1]+rng[0])/2.0;
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
   (void)axis; // To avoid warning
   if(file)
   {
      vtkErrorMacro( <<  "File musn't be opened");
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

   WriteDcmFile(this->FileName,image);
}

void vtkGdcmWriter::RecursiveWrite(int axis, vtkImageData *image, 
                                   vtkImageData *cache, ofstream *file)
{
   (void)axis; // To avoid warning
   (void)image; // To avoid warning
   (void)cache; // To avoid warning
   (void)file; // To avoid warning
}

void vtkGdcmWriter::WriteDcmFile(char *fileName,vtkImageData *image)
{
   // From here, the write of the file begins
   gdcm::File *dcmFile = new gdcm::File();

   // Set the image informations
   SetImageInformation(dcmFile,image);

   // Write the image
   if(!dcmFile->Write(fileName))
   {
      vtkErrorMacro( << "File "  <<  this->FileName  <<  "couldn't be written by "
                     << " the gdcm library");
      std::cerr << "not written \n";
   }

   delete dcmFile;
}

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
