/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestWriteSimple.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/25 15:44:23 $
  Version:   $Revision: 1.16 $
                                                                                
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
   {0,   0,   1, 1, 8,  8,  0, 'i'} // to find the end
};

int WriteSimple(Image &img)
{
   std::string fileName = "TestWriteSimple.dcm";

// Step 1 : Create the header of the image
   std::cout << "        1...";
   gdcm::File *header = new gdcm::File();
   std::ostringstream str;

   // Set the image size
   str.str("");
   str << img.sizeX;
   header->InsertValEntry(str.str(),0x0028,0x0011); // Columns

   str.str("");
   str << img.sizeY;
   header->InsertValEntry(str.str(),0x0028,0x0010); // Rows

   if(img.sizeZ>1)
   {
      str.str("");
      str << img.sizeZ;
      header->InsertValEntry(str.str(),0x0028,0x0008); // Number of Frames
   }

   // Set the pixel type
   str.str("");
   str << img.componentSize;
   header->InsertValEntry(str.str(),0x0028,0x0100); // Bits Allocated

   str.str("");
   str << img.componentUse;
   header->InsertValEntry(str.str(),0x0028,0x0101); // Bits Stored

   str.str("");
   str << img.componentSize - 1;
   header->InsertValEntry(str.str(),0x0028,0x0102); // High Bit

   // Set the pixel representation
   str.str("");
   str << img.sign;
   header->InsertValEntry(str.str(),0x0028,0x0103); // Pixel Representation

   // Set the samples per pixel
   str.str("");
   str << img.components;
   header->InsertValEntry(str.str(),0x0028,0x0002); // Samples per Pixel

   if( !header->IsReadable() )
   {
      std::cout << "Failed\n"
                << "        Prepared image isn't readable\n";

      delete header;
      return 1;
   }

// Step 2 : Create the output image
   std::cout << "2...";
   if( img.componentSize%8 > 0 )
   {
      img.componentSize += 8-img.componentSize%8;
   }
   size_t size = img.sizeX * img.sizeY * img.sizeZ 
               * img.components * img.componentSize / 8;
   unsigned char *imageData = new unsigned char[size];

   // FIXME : find a best heuristic to create the image
   unsigned char *tmp = imageData;
   for(int k=0;k<img.sizeZ;k++)
   {
      for(int j=0;j<img.sizeY;j++)
      {
         for(int i=0;i<img.sizeX;i++)
         {
            for(int c=0;c<img.components;c++)
            {
               *tmp = j%256;
               if( img.componentSize>8 )
               {
                  *(tmp+1) = j/256;
               }
               tmp += img.componentSize * img.components/8;
            }
         }
      }
   }

// Step 3 : Create the file of the image
   std::cout << "3...";
   gdcm::FileHelper *file = new gdcm::FileHelper(header);
   file->SetImageData(imageData,size);

// Step 4 : Set the writting mode and write the image
   std::cout << "4...";

   file->SetWriteModeToRaw();
   switch (img.writeMode)
   {
      case 'a' : // Write an ACR file
         file->SetWriteTypeToAcr();
         break;

      case 'e' : // Write a DICOM Explicit VR file
         file->SetWriteTypeToDcmExplVR();
         break;

      case 'i' : // Write a DICOM Implicit VR file
         file->SetWriteTypeToDcmImplVR();
         break;

      default :
         std::cout << "Failed\n"
                   << "        Write mode '"<<img.writeMode<<"' is undefined\n";

         delete file;
         delete header;
         delete[] imageData;
         return 1;
   }

   if( !file->Write(fileName) )
   {
      std::cout << "Failed\n"
                << "File in unwrittable\n";

      delete file;
      delete header;
      delete[] imageData;
      return 1;
   }

// Step 5 : Read the written image
   std::cout << "5...";
   gdcm::FileHelper* reread = new gdcm::FileHelper( fileName );
   if( !reread->GetFile()->IsReadable() )
   {
     std::cerr << "Failed" << std::endl
               << "Test::TestReadWriteReadCompare: Could not reread image "
               << "written:" << fileName << std::endl;
     delete header;
     delete file;
     delete reread;
     return 1;
   }

// Step 6 : Compare to the written image
   std::cout << "6...";
   size_t dataSizeWritten = reread->GetImageDataSize();
   uint8_t* imageDataWritten = reread->GetImageData();

   // Test the image size
   if (header->GetXSize() != reread->GetFile()->GetXSize() ||
       header->GetYSize() != reread->GetFile()->GetYSize() ||
       header->GetZSize() != reread->GetFile()->GetZSize())
   {
      std::cout << "Failed" << std::endl
         << "        X Size differs: "
         << "X: " << header->GetXSize() << " # " 
                  << reread->GetFile()->GetXSize() << " | "
         << "Y: " << header->GetYSize() << " # " 
                  << reread->GetFile()->GetYSize() << " | "
         << "Z: " << header->GetZSize() << " # " 
                  << reread->GetFile()->GetZSize() << std::endl;
      delete header;
      delete file;
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
      delete header;
      delete file;
      delete reread;
      delete[] imageData;

      return 1;
   }

   // Test the data's content
   if (int res = memcmp(imageData, imageDataWritten, size) !=0)
   {
      (void)res;
      std::cout << "Failed" << std::endl
                << "        Pixel differ (as expanded in memory)." << std::endl;
      delete header;
      delete file;
      delete reread;
      delete[] imageData;

      return 1;
   }

   std::cout << "OK" << std::endl;

   delete header;
   delete file;
   delete reread;
   delete[] imageData;

   return 0;
}

int TestWriteSimple(int argc, char* argv[])
{
   if (argc < 1) 
   {
      std::cerr << "usage: \n" 
                << argv[0] << " (without parameters) " << std::endl 
                << std::endl;
      return 1;
   }

   int ret=0;
   int i=0;
   while( Images[i].sizeX>0 && Images[i].sizeY>0 )
   {
      ret += WriteSimple(Images[i]);
      i++;
   }

   return ret;
}
