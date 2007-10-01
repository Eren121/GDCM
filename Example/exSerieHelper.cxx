/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exSerieHelper.cxx,v $
  Language:  C++
  Date:      $Date: 2007/10/01 09:33:20 $
  Version:   $Revision: 1.14 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmSerieHelper.h"
#include "gdcmFile.h"
#include "gdcmDirList.h" // for FileList
#include "gdcmDebug.h"
#include <iostream>

int main(int argc, char *argv[])
{  
   GDCM_NAME_SPACE::SerieHelper *s;
  
   std::string dirName; 
   if (argc > 1) 
      dirName = argv[1];    
   else 
   {
      dirName = GDCM_DATA_ROOT;
   }

   if (argc > 2)
      GDCM_NAME_SPACE::Debug::DebugOn();

  
   std::cout << "Dir Name :[" << dirName << "]" << std::endl;
   //   
   // Sometimes using only SerieHelper is not enought !
   // See also exXcoherentFileSet
   //

   s = GDCM_NAME_SPACE::SerieHelper::New();
   s->SetLoadMode(GDCM_NAME_SPACE::LD_ALL);     // Load everything for each File
   //GDCM_NAME_SPACE::TagKey t(0x0020,0x0013);
   //s->AddRestriction(t, "340", GDCM_NAME_SPACE::GDCM_LESS); // Keep only files where
                                              // restriction is true
   s->SetDirectory(dirName, true); // true : recursive exploration

   std::cout << " ---------------------------------------- Finish parsing :["
             << dirName << "]" << std::endl;

   s->Print();
   std::cout << " ---------------------------------------- Finish printing (1)"
             << std::endl;


   GDCM_NAME_SPACE::FileList::const_iterator it;
   GDCM_NAME_SPACE::FileList *l;
   std::cout << " ---------------------------------------- Recap"
             << std::endl;  
   l = s->GetFirstSingleSerieUIDFileSet();
   while (l)
   { 
      it = l->begin();
      std::cout << "SerieUID [" <<  (*it)->GetEntryString(0x0020,0x000e) <<"]   Serie Description ["
                << (*it)->GetEntryString(0x0008,0x103e) << "] "  
                << " : " << l->size() << " files" << std::endl;
      l = s->GetNextSingleSerieUIDFileSet();
   } 
    std::cout << " ----------------------------------------End Recap"
             << std::endl;

   int nbFiles;
   double zspacing = 0.;
   // For all the Single SerieUID Files Sets of the GDCM_NAME_SPACE::Serie
   l = s->GetFirstSingleSerieUIDFileSet();
   while (l)
   { 
      nbFiles = l->size() ;
      if ( l->size() > 5 ) // Why not ? Just an example, for testing
      {
         std::cout << "Sort list : " << nbFiles << " long" << std::endl; 
 
         //---------------------------------------------------------
         s->OrderFileList(l);  // sort the list (and compute ZSpacing !)
         //---------------------------------------------------------
 
          zspacing = s->GetZSpacing();
         // Just to show : GetZSpacing from a GDCM_NAME_SPACE::SerieHelper is right  
         std::cout << "GetZSpacing() of sorted SingleSerieUIDFileSet "
                   << "from GDCM_NAME_SPACE::SerieHelper: " << zspacing << std::endl;
         std::cout << " ('-1' means all the files have the same position)" << std::endl;
         for (std::vector<GDCM_NAME_SPACE::File* >::iterator it2 =  l->begin();
                                            it2 != l->end();
                                          ++it2)
         {
          // Just to show : GetZSpacing from a GDCM_NAME_SPACE::File may be different        
             std::cout << (*it2)->GetFileName() << " -->  GetZSpacing() from GDCM_NAME_SPACE::File : " 
                       << (*it2)->GetZSpacing() << std::endl;      
         }  

         break; // we only deal with the first one ... Why not ?
      }
      l = s->GetNextSingleSerieUIDFileSet();
   } 
   std::cout << " ------------------Prints all the Single SerieUID File Sets (sorted or not) -----"
             << std::endl;
   s->Print(); // Prints all the Single SerieUID File Sets (sorted or not)
   std::cout << " -------------------------------------------- Finish printing"
             << std::endl;
     
   s->Delete();

   return 0;
}
