/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestWriteSimple.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/21 08:34:26 $
  Version:   $Revision: 1.41 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

/**
 * Write a dicom file from nothing
 * The written image is 256x256, 8 bits, unsigned char
 * The image content is a horizontal grayscale from 
 * 
 */
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmDebug.h"

#include <iostream>
#include <sstream>

typedef struct
{
   int sizeX;         // Size X of the image
   int sizeY;         // Size Y of the image
   int sizeZ;         // Size Z of the image
   int components;    // Number of components for a pixel
   int componentSize; // Component size (in bits : 8, 16)
   int componentUse ; // Component size (in bits)
   int sign;          // Sign of components
   char writeMode;    // Write mode
                      //  - 'a' : ACR
                      //  - 'e' : Explicit VR
                      //  - 'i' : Implicit VR
} Image;

Image Images [] = {
   {256, 256, 1, 1, 8,  8,  0, 'a'},
   {256, 256, 1, 1, 8,  8,  0, 'e'},
   {256, 256, 1, 1, 8,  8,  0, 'i'},

   {512, 256, 1, 1, 8,  8,  0, 'a'},
   {512, 256, 1, 1, 8,  8,  0, 'e'},
   {512, 256, 1, 1, 8,  8,  0, 'i'},

   {256, 512, 1, 1, 8,  8,  0, 'a'},
   {256, 512, 1, 1, 8,  8,  0, 'e'},
   {256, 512, 1, 1, 8,  8,  0, 'i'},

   {256, 512, 1, 1, 16, 16, 0, 'a'},
   {256, 512, 1, 1, 16, 16, 0, 'e'},
   {256, 512, 1, 1, 16, 16, 0, 'i'},
   {256, 512, 1, 1, 16, 16, 0, 'a'},
   {256, 512, 1, 1, 16, 16, 0, 'e'},
   {256, 512, 1, 1, 16, 16, 0, 'i'},

   {512, 256, 10, 1, 8, 8,  0, 'a'},
   {512, 256, 10, 1, 8, 8,  0, 'e'},
   {512, 256, 10, 1, 8, 8,  0, 'i'},
   {512, 256, 10, 3, 8, 8,  0, 'a'},
   {512, 256, 10, 3, 8, 8,  0, 'e'},
   {512, 256, 10, 3, 8, 8,  0, 'i'},

   {256, 256, 1, 1, 8,  8,  1, 'a'},
   {256, 256, 1, 1, 8,  8,  1, 'e'},
   {256, 256, 1, 1, 8,  8,  1, 'i'},

   {512, 256, 1, 1, 8,  8,  1, 'a'},
   {512, 256, 1, 1, 8,  8,  1, 'e'},
   {512, 256, 1, 1, 8,  8,  1, 'i'},

   {256, 512, 1, 1, 8,  8,  1, 'a'},
   {256, 512, 1, 1, 8,  8,  1, 'e'},
   {256, 512, 1, 1, 8,  8,  1, 'i'},

   {256, 512, 1, 1, 16, 16, 1, 'a'},
   {256, 512, 1, 1, 16, 16, 1, 'e'},
   {256, 512, 1, 1, 16, 16, 1, 'i'},
   {256, 512, 1, 1, 16, 16, 1, 'a'},
   {256, 512, 1, 1, 16, 16, 1, 'e'},
   {256, 512, 1, 1, 16, 16, 1, 'i'},

   {512, 256, 10, 1, 8, 8,  1, 'a'},
   {512, 256, 10, 1, 8, 8,  1, 'e'},
   {512, 256, 10, 1, 8, 8,  1, 'i'},
   {512, 256, 10, 3, 8, 8,  1, 'a'},
   {512, 256, 10, 3, 8, 8,  1, 'e'},
   {512, 256, 10, 3, 8, 8,  1, 'i'},
   {0,   0,   1,  1, 8, 8,  0, 'i'} // to find the end
};

const unsigned int MAX_NUMBER_OF_DIFFERENCE = 10;

int WriteSimple(Image &img)
{
   std::ostringstream fileName;
   fileName.str("");
   fileName << "TestWriteSimple";

// Step 1 : Create the header of the image

   std::cout << "        1...";
   gdcm::File *fileToBuild = new gdcm::File();
   std::ostringstream str;

   // Set the image size
   str.str("");
   str << img.sizeX;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0011); // Columns
   str.str("");
   str << img.sizeY;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0010); // Rows

   if(img.sizeZ>1)
   {
      str.str("");
      str << img.sizeZ;
      fileToBuild->InsertEntryString(str.str(),0x0028,0x0008); // Number of Frames
   }

   fileName << "-" << img.sizeX << "-" << img.sizeY << "-" << img.sizeZ;

   // Set the pixel type
   str.str("");
   str << img.componentSize;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0100); // Bits Allocated

   str.str("");
   str << img.componentUse;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0101); // Bits Stored

   str.str("");
   str << ( img.componentSize - 1 );
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0102); // High Bit

   // Set the pixel representation
   str.str("");
   str << img.sign;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0103); // Pixel Representation

   fileName << "-" << img.componentSize;
   if(img.sign == 0)
      fileName << "U";
   else
      fileName << "S";
   
   switch (img.writeMode)
   {
      case 'a' :
         fileName << ".ACR";  break; 
      case 'e' :
         fileName << ".EXPL"; break; 
      case 'i' :
         fileName << ".IMPL"; break;
   } 

   std::cout << "[" << fileName.str() << "]...";

   // Set the samples per pixel
   str.str("");
   str << img.components;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0002); // Samples per Pixel

// Step 2 : Create the output image
   std::cout << "2...";
   if( img.componentSize%8 > 0 )
   {
      img.componentSize += 8-img.componentSize%8;
   }
   size_t size = img.sizeX * img.sizeY * img.sizeZ 
               * img.components * img.componentSize / 8;
   unsigned char *imageData = new unsigned char[size];

   // FIXME : find a better heuristic to create the image
   unsigned char *tmp = imageData;
   for(int k=0;k<img.sizeZ;k++)
   {
      for(int j=0;j<img.sizeY;j++)
      {
         for(int i=0;i<img.sizeX;i++)
         {
            for(int c=0;c<img.components;c++)
            {
               *tmp = (unsigned char)(j%256);
               if( img.componentSize>8 )
               {
                  *(tmp+1) = (unsigned char)(j/256);
               }
               tmp += img.componentSize/8;
            }
         }
      }
   }

// Step 3 : Create the file of the image
   std::cout << "3...";
   gdcm::FileHelper *fileH = new gdcm::FileHelper(fileToBuild);
   fileH->SetImageData(imageData,size);

// Step 4 : Set the writting mode and write the image
   std::cout << "4...";

   fileH->SetWriteModeToRaw();
   switch (img.writeMode)
   {
      case 'a' : // Write an ACR file
         fileH->SetWriteTypeToAcr();
         break;

      case 'e' : // Write a DICOM Explicit VR file
         fileH->SetWriteTypeToDcmExplVR();
         break;

      case 'i' : // Write a DICOM Implicit VR file
         fileH->SetWriteTypeToDcmImplVR();
         break;

      default :
         std::cout << "Failed for [" << fileName.str() << "]\n"
                   << "        Write mode '"<<img.writeMode<<"' is undefined\n";

         delete fileH;
         delete fileToBuild;
         delete[] imageData;
         return 1;
   }

   if( !fileH->Write(fileName.str()) )
   {
      std::cout << "Failed for [" << fileName.str() << "]\n"
                << "           File is unwrittable\n";

      delete fileH;
      delete fileToBuild;
      delete[] imageData;
      return 1;
   }

// Step 5 : Read the written image
   std::cout << "5...";
   // old form.
   //gdcm::FileHelper *reread = new gdcm::FileHelper( fileName.str() );
   // Better use :
   gdcm::FileHelper *reread = new gdcm::FileHelper( );
   reread->SetFileName( fileName.str() );
   reread->SetLoadMode(gdcm::LD_ALL); // Load everything
                           // Possible values are 
                           //              gdcm::LD_ALL, 
                           //              gdcm::LD_NOSEQ, 
                           //              gdcm::LD_NOSHADOW,
                           //              gdcm::LD_NOSEQ|gdcm::LD_NOSHADOW, 
                           //              gdcm::LD_NOSHADOWSEQ
   reread->Load();

   if( !reread->GetFile()->IsReadable() )
   {
      std::cerr << "Failed" << std::endl
                << "Could not read written image : " << fileName.str() << std::endl;
      delete fileToBuild;
      delete fileH;
      delete reread;
      delete[] imageData;
      return 1;
   }

// Step 6 : Compare to the written image
   std::cout << "6...";
   size_t dataSizeWritten = reread->GetImageDataSize();
   uint8_t *imageDataWritten = reread->GetImageData();

   // Test the image write mode
   if (reread->GetFile()->GetFileType() != fileH->GetWriteType())
   {
      std::cout << "Failed" << std::endl
         << "        File type differ: "
         << fileH->GetWriteType() << " # " 
         << reread->GetFile()->GetFileType() << std::endl;
      delete fileToBuild;
      delete fileH;
      delete reread;
      delete[] imageData;

      return 1;
   }

   // Test the image size
   if (fileToBuild->GetXSize() != reread->GetFile()->GetXSize() ||
       fileToBuild->GetYSize() != reread->GetFile()->GetYSize() ||
       fileToBuild->GetZSize() != reread->GetFile()->GetZSize())
   {
      std::cout << "Failed for [" << fileName.str() << "]" << std::endl
         << "        X Size differs: "
         << "X: " << fileToBuild->GetXSize() << " # " 
                  << reread->GetFile()->GetXSize() << " | "
         << "Y: " << fileToBuild->GetYSize() << " # " 
                  << reread->GetFile()->GetYSize() << " | "
         << "Z: " << fileToBuild->GetZSize() << " # " 
                  << reread->GetFile()->GetZSize() << std::endl;
      delete fileToBuild;
      delete fileH;
      delete reread;
      delete[] imageData;

      return 1;
   }

   // Test the data size
   if (size != dataSizeWritten)
   {
      std::cout << "Failed" << std::endl
         << "        Pixel areas lengths differ: "
         << size << " # " << dataSizeWritten << std::endl;
      delete fileToBuild;
      delete fileH;
      delete reread;
      delete[] imageData;

      return 1;
   }

   // Test the data's content
   if ( memcmp(imageData, imageDataWritten, size) !=0 )
   {
      std::cout << "Failed" << std::endl
                << "        Pixel differ (as expanded in memory)." << std::endl;
      std::cout << "        list of the first " << MAX_NUMBER_OF_DIFFERENCE
                  << " pixels differing (pos : test - ref) :" 
                  << std::endl;
      unsigned int i;
      unsigned int j;
      for(i=0, j=0;i<dataSizeWritten && j<MAX_NUMBER_OF_DIFFERENCE;i++)
      {
         if(imageDataWritten[i]!=imageData[i])
            {
            std::cout << std::hex << "(" << i << " : " 
                        << std::hex << (int)(imageDataWritten[i]) << " - "
                        << std::hex << (int)(imageData[i]) << ") "
                        << std::dec;
            ++j;
            }
      }
      std::cout << std::endl;
      delete fileToBuild;
      delete fileH;
      delete reread;
      delete[] imageData;

      return 1;
   }

   std::cout << "OK" << std::endl;

   delete fileToBuild;
   delete fileH;
   delete reread;
   delete[] imageData;

   return 0;
}

int TestWriteSimple(int argc, char *argv[])
{
   if (argc < 1) 
   {
      std::cerr << "usage: \n" 
                << argv[0] << " (without parameters) " << std::endl 
                << std::endl;
      return 1;
   }

   //gdcm::Debug::DebugOn();
       
   int ret=0;
   int i=0;
   while( Images[i].sizeX>0 && Images[i].sizeY>0 )
   {
      std::cout << "Test n :" << i; 
      ret += WriteSimple(Images[i] );
      i++;
   }

   return ret;
}
