/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PrintDocument.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/21 11:40:52 $
  Version:   $Revision: 1.10 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"

#include <iostream>

int main(int argc, char* argv[])
{
 
   gdcm::File *e1;
   std::string fileName;   

   if (argc != 2) {
      std::cout << " Usage : "
                << argv[0] 
                << " filename."
                << std::endl;
   }

   if (argc > 1) {
      fileName=argv[1];
   } else {
      fileName += GDCM_DATA_ROOT;
      fileName += "/test.acr";
   }
   
   e1= new gdcm::File( fileName.c_str() );

   e1->SetPrintLevel(2);
   
   e1->Print();
      
   std::cout << "\n\n" << std::endl;        
   
   if(e1->IsReadable())
      std::cout <<std::endl<<fileName<<" is Readable"<<std::endl;
   else
      std::cout <<std::endl<<fileName<<" is NOT Readable"<<std::endl;
   std::cout<<std::flush;
   delete e1;

   return 0;
   
}
