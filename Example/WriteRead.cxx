/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: WriteRead.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/02 10:06:32 $
  Version:   $Revision: 1.13 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"

#include <iostream>

int main(int argc, char *argv[])
{  
   std::string fileNameToWrite;

   gdcm::File *e1;
   gdcm::File *e2;
   gdcm::FileHelper *f1;
   gdcm::FileHelper *f2;
   uint8_t* imageData, *imageData2;
   int dataSize, dataSize2;
     
   if( argc < 2 )
    {
    std::cerr << "Usage " << argv[0] << " image.dcm" << std::endl;
    return 1;
    }

   std::string fileName = argv[1];

// --------------------- we read the input image

   std::cout << argv[1] << std::endl;

   e1 = new gdcm::File( fileName );
   if (!e1->IsReadable()) {
       std::cerr << "Sorry, " << fileName <<"  not a Readable DICOM / ACR File"
                 <<std::endl;
       return 0;
   }
   
   f1 = new gdcm::FileHelper(e1);
   imageData= f1->GetImageData();
   dataSize = f1->GetImageDataSize();

// --------------------- we write it as an Explicit VR DICOM file

      fileNameToWrite = "temp.XDCM";
      std::cout << "WriteDCM Explicit VR" << std::endl;
      f1->WriteDcmExplVR(fileNameToWrite);

// --------------------- we read the written image
      
   e2 = new gdcm::File( fileNameToWrite );
   if (!e2->IsReadable()) {
       std::cerr << "Sorry, " << fileNameToWrite << " not a Readable DICOM / ACR File"  
                 <<std::endl;
       return 0;
   }
   f2 = new gdcm::FileHelper(e2);
   imageData2= f2->GetImageData();
   dataSize2 = f2->GetImageDataSize();

// --------------------- we compare the pixel areas

  if (dataSize != dataSize2) {
     std::cout << " ----------------------------------------- " 
          << "Bad shot! Lengthes are different : " 
          << dataSize << " # " << dataSize2
          << " for file : " << fileName << std::endl;

     return 0;
  }
  if (int res=memcmp(imageData,imageData2,dataSize) !=0) {
     std::cout << " ----------------------------------------- " 
          << "Bad shot! Pixels are different : " 
          << " for file : " << fileName << std::endl;
     std::cout << "memcmp(imageData,imageData2,dataSize) = " << res << std::endl;
     return 1;
  }
  
  //If we reach here everything is fine, return 0 then:
  return 0;
}

