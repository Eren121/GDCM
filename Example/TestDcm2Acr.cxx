/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDcm2Acr.cxx,v $
  Language:  C++
  Date:      $Date: 2004/11/16 04:26:18 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include <iostream>
#include "gdcm.h"
#include <stdio.h>

int main(int argc, char* argv[])
{  
   std::string toto;
   char zozo[100];

   gdcm::File  * f1;

   printf (" Before gdcmFile() \n");
   printf ("\n\n---------------------------------------\n\n");

   if (argc > 1) {
      toto = argv[1];
      f1 = new gdcm::File(toto);
   } else {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/test.acr";
      f1 = new gdcm::File(filename.c_str());
   }
   printf (" Sortie gdcmFile() \n");

   //e1.PrintPubDict(std::cout);
   f1->GetHeader()->Print();

   //cle = gdcmDictEntry::TranslateToKey(0x0028,0x0008);

   int dataSize = f1->GetImageDataSize();
   printf ("dataSize %d\n",dataSize);

   // void* imageData= f1->GetHeader()->GetImageData();

   // Ecriture d'un Raw File, a afficher avec affim filein= dim= nbit= signe=
   //f1->WriteRawData("image.raw");

   // ecriture d'un fichier DICOM à partir d'un dcmHeader correct.

   //f1->WriteDcmImplVR("image.dcm");

   // ecriture d'un fichier ACR à partir d'un dcmHeader correct.

   sprintf(zozo, "%s.nema", toto.c_str());
   f1->WriteAcr(zozo);
   printf ("\n\n---------------------------------------\n\n");

   f1->GetHeader()->Print();
   
   return 0;
}
