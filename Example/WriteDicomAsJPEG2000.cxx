/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: WriteDicomAsJPEG2000.cxx,v $
  Language:  C++
  Date:      $Date: 2006/07/20 17:47:34 $
  Version:   $Revision: 1.3 $
                                                                                
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

// Open a dicom file and compress it as JPEG 2000 stream
int main(int argc, char *argv[])
{
  if( argc < 2)
    {
    std::cerr << argv[0] << " inputfilename.dcm\n";
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

// Step 1 : Create the header of the image
   //gdcm::File *f = new gdcm::File();
   // gdcm1.3 syntax. Sorry
   gdcm::File *f = gdcm::File::New();
   f->SetLoadMode ( gdcm::LD_ALL ); // Load everything
   f->SetFileName( filename );
   f->Load();

   //gdcm::FileHelper *tested = new gdcm::FileHelper( f );
   // gdcm1.3 syntax. Sorry   
   gdcm::FileHelper *tested = gdcm::FileHelper::New( f );
   std::string PixelType = tested->GetFile()->GetPixelType();
   int xsize = f->GetXSize();
   int ysize = f->GetYSize();
   int zsize = f->GetZSize();
   //tested->Print( std::cout );

   int samplesPerPixel = f->GetSamplesPerPixel();
   size_t testedDataSize    = tested->GetImageDataSize();
   uint8_t *testedImageData = tested->GetImageData();

// Step 1 : Create the header of the image

//   gdcm::File *fileToBuild = new gdcm::File();
   // gdcm1.3 syntax. Sorry !
   gdcm::File *fileToBuild = gdcm::File::New();
   
   std::ostringstream str;

   // Set the image size
   str.str("");
   str << xsize;
   //fileToBuild->InsertValEntry(str.str(),0x0028,0x0011); // Columns
   // gdcm1.3 syntax. Sorry !
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0011, "US"); // Columns
   str.str("");
   str << ysize;
   //fileToBuild->InsertValEntry(str.str(),0x0028,0x0010); // Rows
   // gdcm1.3 syntax. Sorry !
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0010, "US"); // Rows

   if(zsize>1)
   {
      str.str("");
      str << zsize;
      //fileToBuild->InsertValEntry(str.str(),0x0028,0x0008); // Number of Frames
   // gdcm1.3 syntax. Sorry !
      fileToBuild->InsertEntryString(str.str(),0x0028,0x0008, "IS"); // Number of Frames
   }
   int bitsallocated = f->GetBitsAllocated();
   int bitsstored = f->GetBitsStored();
   int highbit = f->GetHighBitPosition();
   //std::string pixtype = f->GetPixelType();
   int sign = f->IsSignedPixelData();

   // Set the pixel type
   str.str("");
   str << bitsallocated;
   //fileToBuild->InsertValEntry(str.str(),0x0028,0x0100); // Bits Allocated
   // gdcm1.3 syntax. Sorry !
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0100,"US"); // Bits Allocated
   
   str.str("");
   str << bitsstored;  
   //fileToBuild->InsertValEntry(str.str(),0x0028,0x0101); // Bits Stored   
   // gdcm1.3 syntax. Sorry !
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0101, "US"); // Bits Stored
   str.str("");
   str << highbit;
   //fileToBuild->InsertValEntry(str.str(),0x0028,0x0102); // High Bit
   // gdcm1.3 syntax. Sorry !
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0102, "US"); // High Bit

   // Set the pixel representation
   str.str("");
   str << sign;
   //fileToBuild->InsertValEntry(str.str(),0x0028,0x0103); // Pixel Representation
   // gdcm1.3 syntax. Sorry !
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0103, "US"); // Pixel Representation


   // Set the samples per pixel
   str.str("");
   str << samplesPerPixel; //img.components;
   //fileToBuild->InsertValEntry(str.str(),0x0028,0x0002); // Samples per Pixel
   // gdcm1.3 syntax. Sorry !
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0002, "US"); // Samples per Pixel

// Step 2 : Create the output image
   //gdcm::FileHelper *fileH = new gdcm::FileHelper(fileToBuild);
   // gdcm1.3 syntax. Sorry !
   gdcm::FileHelper *fileH = gdcm::FileHelper::New(fileToBuild);
   fileH->SetWriteTypeToJPEG2000(  );
   fileH->SetImageData(testedImageData, testedDataSize);
   if( !fileH->Write(outfilename) )
     {
     std::cerr << "write fails" << std::endl;
     }

   //delete f;
   // gdcm1.3 syntax. Sorry !   
   f->Delete();
   //delete tested;
   tested->Delete();
   //delete fileToBuild;
   fileToBuild->Delete();
   //delete fileH;
   fileH->Delete();

   return 0;
}
