/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDict.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/14 11:28:29 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmGlobal.h"
#include "gdcmDictSet.h"
#include "gdcmDict.h"
#include "gdcmDictEntry.h"

#include <iostream>
#include <iomanip>

int TestDict(int argc, char* argv[])
{  
   // Print the DictSet
   std::cout<<"#######################################################\n";
   gdcm::DictSet *dicts=gdcm::Global::GetDicts();
   if(!dicts)
   {
      std::cout<<"The DictSet hasn't be found... Failed\n";
      return(1);
   }
   std::cout<<"DictSet content :\n";
//   dicts->Print(std::cout);

   // Print the Dict (public)
   std::cout<<"#######################################################\n";
   gdcm::Dict *pubDict=dicts->GetDefaultPubDict();
   if(!pubDict)
   {
      std::cout<<"The public Dict hasn't be found... Failed\n";
      return(1);
   }
   std::cout<<"Public Dict content :\n";
//   pubDict->Print(std::cout);

   // Print the DictEntry (0x10,0x20)
   std::cout<<"#######################################################\n";
   const int ENTRY_GR = 0x10;
   const int ENTRY_EL = 0x20;
   std::string key=gdcm::DictEntry::TranslateToKey(ENTRY_GR,ENTRY_EL);
   gdcm::DictEntry *entry=pubDict->GetDictEntry(ENTRY_GR,ENTRY_EL);
   if(!entry)
   {
      std::cout<<"The DictEntry hasn't be found... Failed\n";
      return(1);
   }
   std::cout<<"Entry "<<key<<" content :\n";
   entry->Print(std::cout);

   // Print all the DictEntry
   std::cout<<"#######################################################\n";
   pubDict->InitTraversal();
   entry=pubDict->GetNextEntry();
   while(entry)
   {
      entry->Print(std::cout);
      entry=pubDict->GetNextEntry();
   }

   return(0);
}
