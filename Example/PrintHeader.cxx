/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PrintHeader.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/08 15:03:57 $
  Version:   $Revision: 1.13 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmHeader.h"
#include <iostream>

int main(int argc, char* argv[])
{ 
   std::string fileName;   

   if (argc == 1)
   {
      std::cout << argv[0] << " fileName" << std::endl;
   }

   if (argc > 1)
   {
      fileName=argv[1];
   }
   else
   {
      fileName += GDCM_DATA_ROOT;
      fileName += "/test.acr";
   }

   gdcm::Header *e1 = new gdcm::Header( fileName.c_str() );  

   if (argc > 2) 
   {
      int level = atoi(argv[2]);   
      e1->SetPrintLevel(level);
   }

   e1->Print();   

   std::cout << "\n\n" << std::endl; 

   if ( e1->GetEntry(0x0002,0x0010) == gdcm::GDCM_NOTLOADED ) 
   {
      std::cout << "Transfert Syntax not loaded. " << std::endl
                << "Better you increase MAX_SIZE_LOAD_ELEMENT_VALUE"
                << std::endl;
      return 0;
   }

   std::string transferSyntaxName = e1->GetTransfertSyntaxName();
   std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" << std::endl;

   if(e1->IsReadable())
      std::cout <<std::endl<<fileName<<" is Readable"<<std::endl;
   else
      std::cout <<std::endl<<fileName<<" is NOT Readable"<<std::endl;
   std::cout<<std::flush;
   delete e1;

   return 0;
}
