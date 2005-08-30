/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PrintDocument.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/30 15:13:05 $
  Version:   $Revision: 1.19 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmDebug.h"
#include "gdcmCommon.h"

#include <iostream>
#include <stdlib.h> //for atoi

int main(int argc, char *argv[])
{
 
   gdcm::File *e1= new gdcm::File();;
   std::string fileName;   

   if (argc == 1) {
      std::cout << " Usage : "
                << argv[0] 
                << " filename"
                << " printLevel debug "
                << "short (=NOSEQ + NOSHADOW)" 
                << std::endl;
       return 0;
   }

   if (argc > 1) {
      fileName=argv[1];
   } else {
      fileName += GDCM_DATA_ROOT;
      fileName += "/test.acr";
   }

   if (argc > 2) 
   {
      int level = atoi(argv[2]);   
      e1->SetPrintLevel(level);
   }

   if (argc > 3)
      gdcm::Debug::DebugOn(); 

   if (argc > 4)
      e1->SetLoadMode(gdcm::LD_NOSEQ | gdcm::LD_NOSHADOW);
   e1->SetFileName( fileName.c_str() );
   e1->Load( );

// we use PrintDocument, because we want to print what we get

//   if ( !e1->IsReadable() )
//   {
//      delete e1;
//      return 0;
//   }

   e1->Print();
      
   std::cout << "\n\n" << std::endl; 

   if ( e1->GetEntryValue(0x0002,0x0010) == gdcm::GDCM_NOTLOADED ) 
   {
      std::cout << "Transfer Syntax not loaded. " << std::endl
                << "Better you increase MAX_SIZE_LOAD_ELEMENT_VALUE"
                << std::endl;
      return 0;
   }       
   
   if(e1->IsReadable())
      std::cout <<std::endl<<fileName<<" is 'file Readable'"<<std::endl;
   else
      std::cout <<std::endl<<fileName<<" is NOT 'file Readable'"<<std::endl;
   std::cout<<std::flush;
   delete e1;

   return 0;
   
}
