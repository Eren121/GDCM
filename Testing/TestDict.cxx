/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDict.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/18 07:56:21 $
  Version:   $Revision: 1.4 $
                                                                                
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

int TestDict(int , char* [])
{  
   // Print the DictSet
   std::cout<<"#######################################################\n";
   gdcm::DictSet *dicts=gdcm::Global::GetDicts();
   if(!dicts)
   {
      std::cout<<"DictSet hasn't be found... Failed\n";
      return(1);
   }

   std::cout<<"DictSet content :\n";

   gdcm::Dict *d = dicts->GetFirstEntry();
   if (!d)
   {
      std::cout << "Dictset is empty" << std::endl;
      return 1;
   }
   while (d)
   {
      std::cout << "------------- a Dict is found : ----------" << std::endl;
      d->Print();
      d = dicts->GetNextEntry();
   }

   // Print the Dict (public)
   std::cout<<"#######################################################\n";
   gdcm::Dict *pubDict=dicts->GetDefaultPubDict();
   if(!pubDict)
   {
      std::cout<<"The public Dict hasn't be found... Failed\n";
      return(1);
   }
   std::cout<<"Public Dict content :\n";
//   pubDict->Print();

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
   entry->Print();

   // Print all the DictEntry
   std::cout<<"#######################################################\n";
   entry=pubDict->GetFirstEntry();
   while(entry)
   {
      entry->Print();
      entry=pubDict->GetNextEntry();
   }

   return(0);
}
