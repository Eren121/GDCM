/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exSerieHelper.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/21 04:55:30 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmSerieHelper.h"
#include "gdcmFile.h"
#include "gdcmDebug.h"
#include <iostream>

int main(int argc, char *argv[])
{  
   gdcm::SerieHelper *s;
  
   std::string dirName; 
   if (argc > 1) 
      dirName = argv[1];    
   else 
   {
      dirName = GDCM_DATA_ROOT;
   }

   if (argc > 2)
      gdcm::Debug::DebugOn();

  
   std::cout << "Dir Name :[" << dirName << "]" << std::endl;

   s = new gdcm::SerieHelper();
   s->SetLoadMode(0x00000000);     // Load everything for each File
   //s->AddRestriction(tagKey, valueToCheck); // Keep only files where
                                           // restriction is true
   s->SetDirectory(dirName, true); // true : recursive exploration

   std::cout << " ---------------------------------------- Finish parsing :["
             << dirName << "]" << std::endl;

   s->Print();
   std::cout << " ---------------------------------------- Finish printing (1)"
             << std::endl;

   int nbFiles;
   // For all the Coherent Files lists of the gdcm::Serie
   gdcm::FileList *l = s->GetFirstCoherentFileList();
   while (l)
   { 
      nbFiles = l->size() ;
      if ( l->size() > 3 ) // Why not ? Just an example, for testing
      {
         std::cout << "Sort list : " << nbFiles << " long" << std::endl;
         s->OrderFileList(l);  // sort the list
      }
      l = s->GetNextCoherentFileList();
   } 
   std::cout << " -------------------------------------------- Finish sorting"
             << std::endl;
   s->Print(); // Prints all the Coherent Files lists (sorted or not)
   std::cout << " -------------------------------------------- Finish printing"
             << std::endl;


   // Only for the first Coherent File List 
   // ( Why not ? Just an example, for testing )
   // Display all the file names

   std::string fileName; 
   l = s->GetFirstCoherentFileList();
   for (std::vector<gdcm::File* >::iterator it =  l->begin();
                                            it != l->end();
                                          ++it)
   {
      fileName = (*it)->GetFileName();
      std::cout << fileName << std::endl;
   } 
     

   delete s;

   return 0;
}