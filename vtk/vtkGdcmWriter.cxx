/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkGdcmWriter.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/08 15:04:00 $
  Version:   $Revision: 1.9 $
                                                                                
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

vtkCxxRevisionMacro(vtkGdcmWriter, "$Revision: 1.9 $");
vtkStandardNewMacro(vtkGdcmWriter);

//-----------------------------------------------------------------------------
// Constructor / Destructor
vtkGdcmWriter::vtkGdcmWriter()
{
   this->LookupTable = NULL;
   this->FileDimensionality = 3;
   this->WriteType = VTK_GDCM_WRITE_TYPE_EXPLICIT_VR;
}

vtkGdcmWriter::~vtkGdcmWriter()
{
}

//-----------------------------------------------------------------------------
// Print
void vtkGdcmWriter::PrintSelf(ostream& os, vtkIndent indent)
{
   this->Superclass::PrintSelf(os,indent);

   os << indent << "Write type : " << this->GetWriteTypeAsString();
}

//-----------------------------------------------------------------------------
// Public
const char *vtkGdcmWriter::GetWriteTypeAsString()
{
   switch(WriteType)
   {
      case VTK_GDCM_WRITE_TYPE_EXPLICIT_VR :
         return "Explicit VR";
      case VTK_GDCM_WRITE_TYPE_IMPLICIT_VR :
         return "Implicit VR";
      case VTK_GDCM_WRITE_TYPE_ACR :
         return "ACR";
      case VTK_GDCM_WRITE_TYPE_ACR_LIBIDO :
         return "ACR Libido";
      default :
         return "Unknow type";
   }
}

//-----------------------------------------------------------------------------
// Protected
/**
 * Copy the image and reverse the Y axis
 */
// The output datas must be deleted by the user of the method !!!
size_t ReverseData(vtkImageData *image,unsigned char **data)
{
   int inc[3];
   int *extent = image->GetUpdateExtent();
   int dim[3] = {extent[1]-extent[0]+1,
                 extent[3]-extent[2]+1,
                 extent[5]-extent[4]+1};

   size_t lineSize = dim[0] * image->GetScalarSize()
                   * image->GetNumberOfScalarComponents();
   size_t planeSize = dim[1] * lineSize;
   size_t size = dim[2] * planeSize;

   if( size>0 )
   {
      *data = new unsigned char[size];

      image->GetIncrements(inc);
      unsigned char *src = (unsigned char *)image->GetScalarPointerForExtent(extent);
      unsigned char *dst = *data + planeSize - lineSize;
      for (int plane = extent[4]; plane <= extent[5]; plane++)
      {
         for (int line = extent[2]; line <= extent[3]; line++)
         {
            // Copy one line at proper destination:
            memcpy((void*)dst, (void*)src, lineSize);

            src += inc[1] * image->GetScalarSize();
            dst -= lineSize;
         }
         dst += 2 * planeSize;
      }
   }
   else
   {
      *data = NULL;
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
   int *extent = image->GetUpdateExtent();
   int dim[3] = {extent[1]-extent[0]+1,
                 extent[3]-extent[2]+1,
                 extent[5]-extent[4]+1};

   str.str("");
   str << dim[0];
   file->ReplaceOrCreate(str.str(),0x0028,0x0011); // Columns

   str.str("");
   str << dim[1];
   file->ReplaceOrCreate(str.str(),0x0028,0x0010); // Rows

   if(dim[2]>1)
   {
      str.str("");
      str << dim[2];
      //file->ReplaceOrCreate(str.str(),0x0028,0x0012); // Planes
      file->ReplaceOrCreate(str.str(),0x0028,0x0008); // Number of Frames
   }

   // Pixel type
   str.str("");
   str << image->GetScalarSize()*8;
   file->ReplaceOrCreate(str.str(),0x0028,0x0100); // Bits Allocated
   file->ReplaceOrCreate(str.str(),0x0028,0x0101); // Bits Stored

   str.str("");
   str << image->GetScalarSize()*8-1;
   file->ReplaceOrCreate(str.str(),0x0028,0x0102); // High Bit

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
   file->ReplaceOrCreate(str.str(),0x0028,0x0103); // Pixel Representation

   // Samples per pixel
   str.str("");
   str << image->GetNumberOfScalarComponents();
   file->ReplaceOrCreate(str.str(),0x0028,0x0002); // Samples per Pixel

   // Spacing
   double *sp = image->GetSpacing();

   str.str("");
   str << sp[0] << "\\" << sp[1];
   file->ReplaceOrCreate(str.str(),0x0028,0x0030); // Pixel Spacing
   str.str("");
   str << sp[2];
   file->ReplaceOrCreate(str.str(),0x0018,0x0088); // Spacing Between Slices

   // Origin
   double *org = image->GetOrigin();

   str.str("");
   str << org[0] << "\\" << org[1] << "\\" << org[2];
   file->ReplaceOrCreate(str.str(),0x0020,0x0032); // Image Position Patient

   // Window / Level
   double *rng=image->GetScalarRange();

   str.str("");
   str << rng[1]-rng[0];
   file->ReplaceOrCreate(str.str(),0x0028,0x1051); // Window Width
   str.str("");
   str << (rng[1]+rng[0])/2.0;
   file->ReplaceOrCreate(str.str(),0x0028,0x1050); // Window Center

   // Pixels
   unsigned char *data;
   size_t size = ReverseData(image,&data);
   file->SetUserData(data,size);
}

/**
 * Write of the files
 * The call to this method is recursive if there is some files to write
 */ 
void vtkGdcmWriter::RecursiveWrite(int axis, vtkImageData *image, ofstream *file)
{
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

   RecursiveWrite(axis,image,image,file);
   //WriteDcmFile(this->FileName,image);
}

void vtkGdcmWriter::RecursiveWrite(int axis, vtkImageData *cache, 
                                   vtkImageData *image, ofstream *file)
{
   int idx, min, max;

   // if the file is already open then just write to it
   if( file )
   {
      vtkErrorMacro( <<  "File musn't be opened");
      return;
   }

   // if we need to open another slice, do it
   if( (axis + 1) == this->FileDimensionality )
   {
      // determine the name
      if (this->FileName)
      {
         sprintf(this->InternalFileName,"%s",this->FileName);
      }
      else 
      {
         if (this->FilePrefix)
         {
            sprintf(this->InternalFileName, this->FilePattern, 
            this->FilePrefix, this->FileNumber);
         }
         else
         {
            sprintf(this->InternalFileName, this->FilePattern,this->FileNumber);
         }
         if (this->FileNumber < this->MinimumFileNumber)
         {
            this->MinimumFileNumber = this->FileNumber;
         }
         else if (this->FileNumber > this->MaximumFileNumber)
         {
            this->MaximumFileNumber = this->FileNumber;
         }
      }

      // Write the file
      WriteDcmFile(this->InternalFileName,image);
      ++this->FileNumber;
      return;
   }

   // if the current region is too high a dimension forthe file
   // the we will split the current axis
   cache->GetAxisUpdateExtent(axis, min, max);

   // if it is the y axis then flip by default
   if (axis == 1 && !this->FileLowerLeft)
   {
      for(idx = max; idx >= min; idx--)
      {
         cache->SetAxisUpdateExtent(axis, idx, idx);
         this->RecursiveWrite(axis - 1, cache, image, file);
      }
   }
   else
   {
      for(idx = min; idx <= max; idx++)
      {
         cache->SetAxisUpdateExtent(axis, idx, idx);
         this->RecursiveWrite(axis - 1, cache, image, file);
      }
   }

   // restore original extent
   cache->SetAxisUpdateExtent(axis, min, max);
}

void vtkGdcmWriter::WriteDcmFile(char *fileName,vtkImageData *image)
{
   // From here, the write of the file begins
   gdcm::File *dcmFile = new gdcm::File();

   // Set the image informations
   SetImageInformation(dcmFile,image);

   // Write the image
   switch(this->WriteType)
   {
      case VTK_GDCM_WRITE_TYPE_EXPLICIT_VR :
         dcmFile->SetWriteTypeToDcmExplVR();
         break;
      case VTK_GDCM_WRITE_TYPE_IMPLICIT_VR :
         dcmFile->SetWriteTypeToDcmImplVR();
         break;
      case VTK_GDCM_WRITE_TYPE_ACR :
         dcmFile->SetWriteTypeToAcr();
         break;
      case VTK_GDCM_WRITE_TYPE_ACR_LIBIDO :
         dcmFile->SetWriteTypeToAcrLibido();
         break;
      default :
         dcmFile->SetWriteTypeToDcmExplVR();
   }

   if(!dcmFile->Write(fileName))
   {
      vtkErrorMacro( << "File "  <<  this->FileName  <<  "couldn't be written by "
                     << " the gdcm library");
      std::cerr << "not written \n";
   }

   // Clean up
   if( dcmFile->GetUserData() && dcmFile->GetUserDataSize()>0 )
   {
      delete[] dcmFile->GetUserData();
   }
   delete dcmFile;
}

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
