/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDcm2Acr.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/20 16:16:58 $
  Version:   $Revision: 1.6 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmHeader.h"

#include <iostream>

int main(int argc, char* argv[])
{  
   std::string toto;
   std::string zozo;

   gdcm::FileHelper  * f1;

   std::cout << " Before gdcmFileHelper()" << std::endl;
   std::cout << "\n\n---------------------------------------" << std::endl;

   if (argc > 1)
   {
      toto = argv[1];
      f1 = new gdcm::FileHelper(toto);
   }
   else
   {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/test.acr";
      f1 = new gdcm::FileHelper(filename);
   }
   std::cout << " Sortie gdcmFileHelper()" << std::endl;

   //e1.PrintPubDict(std::cout);
   f1->GetHeader()->Print();

   //cle = gdcmDictEntry::TranslateToKey(0x0028,0x0008);

   int dataSize = f1->GetImageDataSize();
   std::cout << "dataSize:" << dataSize << std::endl;

   // void* imageData= f1->GetHeader()->GetImageData();

   // Ecriture d'un Raw File, a afficher avec affim filein= dim= nbit= signe=
   //f1->WriteRawData("image.raw");

   // ecriture d'un fichier DICOM à partir d'un dcmHeader correct.

   //f1->WriteDcmImplVR("image.dcm");

   // ecriture d'un fichier ACR à partir d'un dcmHeader correct.

   zozo = toto + ".nema";
   f1->WriteAcr(zozo);
   std::cout << "\n\n---------------------------------------\n\n" << std::endl;

   f1->GetHeader()->Print();
   
   return 0;
}
