/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDict.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/29 17:08:54 $
  Version:   $Revision: 1.12 $
                                                                                
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

int TestDict(int , char *[])
{ 

   std::cout << "----- Test Default Dicom Dictionary : ----------" << std::endl;
   // Just to improve test coverage:
   gdcm::Dict *tempDict = gdcm::Dict::New("dummyFileNameThatDoesntExist");
   // Default dict is supposed to be used.
   tempDict->Print();
   std::cout << "---- end Test Default Dicom Dictionary : -------" << std::endl;

   // Lets delete it.
   tempDict->Delete();

 
   // Print the DictSet
   std::cout<<"#######################################################\n";
   gdcm::DictSet *dicts=gdcm::Global::GetDicts();
   if(!dicts)
   {
      std::cout<<"DictSet hasn't be found... Failed\n";
      return(1);
   }

   std::cout<<"DictSet content :\n";

   gdcm::Dict *d = dicts->GetFirstDict();
   if (!d)
   {
      std::cout << "Dictset is empty" << std::endl;
      return 1;
   }

   std::cout << "----------- Print DictSet contents: ----------" << std::endl;
   dicts->Print();
   std::cout << "----------- End Print DictSet contents: ------" << std::endl;

   while (d)
   {
      std::cout << "------------- a Dict is found : ----------" << std::endl;
      d->Print();
      d = dicts->GetNextDict();
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
   gdcm::TagKey key = gdcm::DictEntry::TranslateToKey(ENTRY_GR,ENTRY_EL);
   gdcm::DictEntry *entry=pubDict->GetEntry(ENTRY_GR,ENTRY_EL);
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
      std::cout << std::hex << entry->GetGroup() << "|" << entry->GetElement()
                << " [" << entry->GetVR() << "] - VM [" << entry->GetVM()
                << "] : " << entry->GetName() << " ( " << entry->GetKey() << ")\n";
      entry=pubDict->GetNextEntry();
   }

/*   // Let's play with DicEntry stuff !
   // First, we try to break an Entry.
   entry=pubDict->GetFirstEntry();
   entry->SetVR("PN");
   // Should warn us !*/

   return(0);
}
