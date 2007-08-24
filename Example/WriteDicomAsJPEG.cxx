/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: WriteDicomAsJPEG.cxx,v $
  Language:  C++
  Date:      $Date: 2007/08/24 10:48:08 $
  Version:   $Revision: 1.16 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"

// Open a dicom file and compress it as JPEG stream
int main(int argc, char *argv[])
{
  if( argc < 2)
    {
    std::cerr << argv[0] << " inputfilename.dcm [ outputfilename.dcm"
              << "quality debug]\n";
    return 1;
    }

   std::string filename = argv[1];
   std::string outfilename = "/tmp/bla.dcm";
   if( argc >= 3 )
     outfilename = argv[2];
   int quality = 100;
   if( argc >= 4 )
     quality = atoi(argv[3]);
   std::cerr << "Using quality: " << quality << std::endl;
   
   if (argc > 4)
      GDCM_NAME_SPACE::Debug::DebugOn();
      
// Step 1 : Read the image
   GDCM_NAME_SPACE::File *f = GDCM_NAME_SPACE::File::New();
   f->SetLoadMode ( GDCM_NAME_SPACE::LD_ALL ); // Load everything
   f->SetFileName( filename );
   f->Load();

   GDCM_NAME_SPACE::FileHelper *tested = GDCM_NAME_SPACE::FileHelper::New( f );
   std::string PixelType = tested->GetFile()->GetPixelType();
   int xsize = f->GetXSize();
   int ysize = f->GetYSize();
   int zsize = f->GetZSize();

   int samplesPerPixel = f->GetSamplesPerPixel();
   size_t testedDataSize    = tested->GetImageDataSize();
   std::cerr << "testedDataSize:" << testedDataSize << std::endl;
   uint8_t *testedImageData = tested->GetImageData();
   
   if( GDCM_NAME_SPACE::Debug::GetDebugFlag() )  
      tested->Print( std::cout );

// Step 1 : Create the header of the new file
   GDCM_NAME_SPACE::File *fileToBuild = GDCM_NAME_SPACE::File::New();
   std::ostringstream str;

   // Set the image size
   str.str("");
   str << xsize;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0011,"US"); // Columns
   str.str("");
   str << ysize;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0010,"US"); // Rows
 
   if(zsize>1)
   {
      str.str("");
      str << zsize;
      fileToBuild->InsertEntryString(str.str(),0x0028,0x0008,"IS"); // Number of Frames
   }
   
   int bitsallocated = f->GetBitsAllocated();
   int bitsstored = f->GetBitsStored();
   int highbit = f->GetHighBitPosition();
   //std::string pixtype = f->GetPixelType();
   int sign = f->IsSignedPixelData();

   // Set the pixel type
   str.str("");
   str << bitsallocated;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0100,"US");// Bits Allocated
   str.str("");
   str << bitsstored;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0101,"US");  // Bits Stored

   str.str("");
   str << highbit;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0102,"US"); // High Bit

   // Set the pixel representation
   str.str("");
   str << sign;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0103,"US"); // Pixel Representation

   // Set the samples per pixel
   str.str("");
   str << samplesPerPixel; //img.components;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0002,"US"); // Samples per Pixel

// Step 2 : Create the output image
   size_t size = xsize * ysize * zsize
               * samplesPerPixel  * bitsallocated / 8;

   GDCM_NAME_SPACE::FileHelper *fileH = GDCM_NAME_SPACE::FileHelper::New(fileToBuild);

   assert( size == testedDataSize );
   fileH->SetWriteTypeToJPEG(  );

   //fileH->SetImageData(testedImageData, testedDataSize);
   
   // SetUserData will ensure the compression
   fileH->SetUserData(testedImageData, testedDataSize);
   if( !fileH->Write(outfilename) )
     {
     std::cerr << "write fails" << std::endl;
     }
   
   f->Delete();
   tested->Delete();
   fileToBuild->Delete();
   fileH->Delete();

   return 0;
}

