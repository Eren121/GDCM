/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: WriteDicomSimple.cxx,v $
  Language:  C++
  Date:      $Date: 2004/12/10 13:49:06 $
  Version:   $Revision: 1.3 $
                                                                                
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
#include "gdcmHeader.h"
#include "gdcmFile.h"

#include <iostream>

// Image size
#define SIZE_X          256
#define SIZE_Y          256
// Number of components in the image (3 for RGB)
#define COMPONENT       1
// Size of each component (in byte)
#define COMPONENT_SIZE  1
// Window / Level
#define COLOR_WINDOW    256
#define COLOR_LEVEL     128

int main(int argc, char* argv[])
{
   if (argc < 3) 
   {
      std::cerr << "usage: \n" 
                << argv[0] << " Output Mode " << std::endl 
                << "Output : output file name\n"
                << "Mode : \n"
                << "   a : ACR, produces a file named Output.ACR\n"
                << "   e : DICOM Explicit VR, produces a file named Output.E.DCM\n"
                << "   i : DICOM Implicit VR, produces a file named Output.I.DCM\n"
                << "   r : RAW, produces a file named Output.RAW\n"
                << std::endl;
      return 1;
   }


// Step 1 : Create the header of the image
   gdcm::Header *header = new gdcm::Header();
   std::ostringstream str;

   // Set the image size
   str.str("");
   str << SIZE_X;
   header->ReplaceOrCreateByNumber(str.str(),0x0028,0x0011); // Columns

   str.str("");
   str << SIZE_Y;
   header->ReplaceOrCreateByNumber(str.str(),0x0028,0x0010); // Rows

   // Set the pixel type
   str.str("");
   str << COMPONENT_SIZE * 8;
   header->ReplaceOrCreateByNumber(str.str(),0x0028,0x0100); // Bits Allocated
   header->ReplaceOrCreateByNumber(str.str(),0x0028,0x0101); // Bits Stored

   str.str("");
   str << COMPONENT_SIZE * 8 - 1;
   header->ReplaceOrCreateByNumber(str.str(),0x0028,0x0102); // High Bit

   // Set the pixel representation
   str.str("");
   str << "0"; // Unsigned
   header->ReplaceOrCreateByNumber(str.str(),0x0028,0x0103); // Pixel Representation

   // Set the samples per pixel
   str.str("");
   str << COMPONENT;
   header->ReplaceOrCreateByNumber(str.str(),0x0028,0x0002); // Samples per Pixel

   // Set the Window / Level
   str.str("");
   str << COLOR_WINDOW;
   header->ReplaceOrCreateByNumber(str.str(),0x0028,0x1051); // Window Width
   str.str("");
   str << COLOR_LEVEL;
   header->ReplaceOrCreateByNumber(str.str(),0x0028,0x1050); // Window Center

   if( !header->IsReadable() )
   {
      std::cerr << "-------------------------------\n"
                << "Error while creating the file\n"
                << "This file is considered to be not readable\n";

      return 1;
   }

// Step 2 : Create the output image
   size_t size = SIZE_X * SIZE_Y * COMPONENT * COMPONENT_SIZE;
   unsigned char *imageData = new unsigned char[size];

   unsigned char *tmp = imageData;
   for(int j=0;j<SIZE_Y;j++)
   {
      for(int i=0;i<SIZE_X;i++)
      {
         for(int c=0;c<COMPONENT;c++)
         {
            *tmp = j;
            tmp += COMPONENT_SIZE; 
         }
      }
   }

// Step 3 : Create the file of the image
   gdcm::File *file = new gdcm::File(header);
   file->SetImageData(imageData,size);

// Step 4 : Set the writting mode and write the image
   std::string fileName = argv[1]; 
   std::string mode = argv[2];

   file->SetWriteModeToRaw();
   switch (mode[0])
   {
      case 'a' : // Write an ACR file
         fileName += ".ACR";
         file->SetWriteTypeToAcr();
         std::cout << "Write ACR" << std::endl
                   << "File :" << fileName << std::endl;
         break;

      case 'e' : // Write a DICOM Explicit VR file
         fileName += ".E.DCM";
         file->SetWriteTypeToDcmExplVR();
         std::cout << "Write DCM Explicit VR" << std::endl
                   << "File :" << fileName << std::endl;
         break;

      case 'i' : // Write a DICOM Implicit VR file
         fileName += ".I.DCM";
         file->SetWriteTypeToDcmImplVR();
         std::cout << "Write DCM Implicit VR" << std::endl
                   << "File :" << fileName << std::endl;
         break;

      case 'r' : // Write a RAW file
         fileName += ".RAW";
         file->WriteRawData(fileName);
         std::cout << "Write Raw" << std::endl
                   << "File :" << fileName << std::endl;

         delete[] imageData;
         delete file;
         delete header;
         return 0;

      default :
         std::cout << "-------------------------------\n"
                   << "Write mode undefined...\n"
                   << "No file written\n";

         delete[] imageData;
         delete file;
         delete header;
         return 1;
   }

   if( !file->Write(fileName) )
   {
      std::cout << "-------------------------------\n"
                   << "Error when writting the file " << fileName << "\n"
                << "No file written\n";
   }

   delete[] imageData;
   delete file;
   delete header;

   return 0;
}
