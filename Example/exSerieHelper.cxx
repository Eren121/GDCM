/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exSerieHelper.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/14 15:55:17 $
  Version:   $Revision: 1.7 $
                                                                                
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

   s = gdcm::SerieHelper::New();
   s->SetLoadMode(gdcm::LD_ALL);     // Load everything for each File
   //gdcm::TagKey t(0x0020,0x0013);
   //s->AddRestriction(t, "340", gdcm::GDCM_LESS); // Keep only files where
                                              // restriction is true
   s->SetDirectory(dirName, true); // true : recursive exploration

   std::cout << " ---------------------------------------- Finish parsing :["
             << dirName << "]" << std::endl;

   s->Print();
   std::cout << " ---------------------------------------- Finish printing (1)"
             << std::endl;

   int nbFiles;
   // For all the Single SerieUID Files Sets of the gdcm::Serie
   gdcm::FileList *l = s->GetFirstSingleSerieUIDFileSet();
   while (l)
   { 
      nbFiles = l->size() ;
      if ( l->size() > 3 ) // Why not ? Just an example, for testing
      {
         std::cout << "Sort list : " << nbFiles << " long" << std::endl;
         s->OrderFileList(l);  // sort the list
      }
      l = s->GetNextSingleSerieUIDFileSet();
   } 
   std::cout << " -------------------------------------------- Finish sorting"
             << std::endl;
   s->Print(); // Prints all the Single SerieUID File Sets (sorted or not)
   std::cout << " -------------------------------------------- Finish printing"
             << std::endl;


   // Only for the first Coherent File List 
   // ( Why not ? Just an example, for testing )
   // Display all the file names

   std::string fileName; 
   l = s->GetFirstSingleSerieUIDFileSet();
   for (std::vector<gdcm::File* >::iterator it =  l->begin();
                                            it != l->end();
                                          ++it)
   {
      fileName = (*it)->GetFileName();
      std::cout << fileName << std::endl;
   } 
     

   s->Delete();

   return 0;
}
