/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exXCoherentFileSet.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/25 15:57:20 $
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

   s = new gdcm::SerieHelper();
   s->SetLoadMode(gdcm::LD_ALL);     // Load everything for each File
   //s->AddRestriction(tagKey, valueToCheck); // Keep only files where
                                              // restriction is true
   s->SetDirectory(dirName, true); // true : recursive exploration

   std::cout << " ---------------------------------------- "
             << "'Single UID' Filesets found in :["
             << dirName << "]" << std::endl;

   s->Print();
   std::cout << " ------------------------------------- Result after splitting"
             << std::endl;

   int nbFiles;
   std::string fileName;
   // For all the Single SerieUID Files Sets of the gdcm::Serie
   gdcm::FileList *l = s->GetFirstSingleSerieUIDFileSet();
   while (l)
   { 
      nbFiles = l->size() ;
      if ( l->size() > 3 ) // Why not ? Just an example, for testing
      {
         std::cout << "Split the 'Single SerieUID' FileSet :[" 
                   << s->GetCurrentSerieUIDFileSetUID()
           << "]  " << nbFiles << " long" << std::endl;
         std::cout << "-----------------------------------" << std::endl;  
         gdcm::XCoherentFileSetmap xcm = s->SplitOnOrientation(l);
         //gdcm::XCoherentFileSetmap xcm = s->SplitOnPosition(l);
 
         for (gdcm::XCoherentFileSetmap::iterator i = xcm.begin();
                                                  i != xcm.end();
                                                ++i)
         {
            std::cout << "Orientation : [" << (*i).first << "]" << std::endl;
    
           // Nowadays OrderFileList() causes trouble, since some files
           // (MIP views) don't have 'Position', now considered as mandatory
           // Commented out for the moment. 
           //s->OrderFileList((*i).second);  // sort the XCoherent Fileset
    
            for (std::vector<gdcm::File* >::iterator it =  ((*i).second)->begin();
                                                     it != ((*i).second)->end();
                                                   ++it)
            {
               fileName = (*it)->GetFileName();
               std::cout << "    " << fileName << std::endl;
            } 
            std::cout << std::endl;   
         }
      }
      l = s->GetNextSingleSerieUIDFileSet();
   } 
  
   delete s;

   return 0;
}
