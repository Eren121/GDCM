/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDcm2Acr.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/02 10:06:31 $
  Version:   $Revision: 1.9 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFileHelper.h"
#include "gdcmFile.h"

#include <iostream>

int main(int argc, char *argv[])
{  
   std::string fileName;
   std::string fileNameToWrite;

   gdcm::FileHelper *f1;

   std::cout << " Before gdcmFileHelper()" << std::endl;
   std::cout << "\n\n---------------------------------------" << std::endl;

   if (argc > 1)
   {
      fileName = argv[1];
      f1 = new gdcm::FileHelper(fileName);
   }
   else
   {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/test.acr";
      f1 = new gdcm::FileHelper(filename);
   }
   std::cout << " After gdcmFileHelper()" << std::endl;

   f1->GetFile()->Print();

   int dataSize = f1->GetImageDataSize();
   std::cout << "dataSize:" << dataSize << std::endl;

   // void *imageData= f1->GetFile()->GetImageData();

   // Ecriture d'un Raw File, a afficher avec affim filein= dim= nbit= signe=
   //f1->WriteRawData("image.raw");

   // ecriture d'un fichier DICOM à partir d'un dcmFile correct.

   //f1->WriteDcmImplVR("image.dcm");

   // ecriture d'un fichier ACR à partir d'un dcmFile correct.

   fileNameToWrite = fileName + ".nema";
   f1->WriteAcr(fileNameToWrite);
   std::cout << "\n\n---------------------------------------\n\n" << std::endl;

   f1->GetFile()->Print();
   
   return 0;
}
