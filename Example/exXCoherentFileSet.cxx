/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exXCoherentFileSet.cxx,v $
  Language:  C++
  Date:      $Date: 2007/06/15 13:18:51 $
  Version:   $Revision: 1.10 $
                                                                                
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

#include "gdcmArgMgr.h"

int main(int argc, char *argv[])
{  

   START_USAGE(usage)
   "\n exXCoherentFileSet :\n                                                 ",
   "Shows the various 'XCoherent' Filesets within a directory                 ",
   "usage: exXCoherentFileSet {dirin=inputDirectoryName}                      ",
   "                       { tag= group-elem | pos | ori }  sort              ",
   "                       [ { [noshadowseq] | [noshadow][noseq] } ] [debug]  ",
   "                                                                          ",
   "       inputDirectoryName : user wants to analyze *all* the files         ",
   "                            within the directory                          ",
   "       pos  : user wants to split each Single SerieUID Fileset on the     ",
   "                         'Image Position '                                ",
   "       ori  : user wants to split each Single SerieUID Fileset on the     ",
   "                         'Image Orientation '                             ",
   "       tag : group-elem    (in hexa, no space)                            ",
   "                       the user wants to split on                         ",
   "       sort :  user wants FileHelper to sort the images                   ",
   "               Warning : will probabely crah if sort has no meaning       ",
   "       noshadowseq: user doesn't want to load Private Sequences           ",
   "       noshadow   : user doesn't want to load Private groups (odd number) ",
   "       noseq      : user doesn't want to load Sequences                   ",
   "       debug      : user wants to run the program in 'debug mode'         ",
   FINISH_USAGE
   
   // ----- Initialize Arguments Manager ------
  
   GDCM_NAME_SPACE::ArgMgr *am = new GDCM_NAME_SPACE::ArgMgr(argc, argv);
  
   if (am->ArgMgrDefined("usage") || argc == 1) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }

   if (am->ArgMgrDefined("debug"))
      GDCM_NAME_SPACE::Debug::DebugOn();
      
   int loadMode = GDCM_NAME_SPACE::LD_ALL;
   if ( am->ArgMgrDefined("noshadowseq") )
      loadMode |= GDCM_NAME_SPACE::LD_NOSHADOWSEQ;
   else 
   {
      if ( am->ArgMgrDefined("noshadow") )
         loadMode |= GDCM_NAME_SPACE::LD_NOSHADOW;
      if ( am->ArgMgrDefined("noseq") )
         loadMode |= GDCM_NAME_SPACE::LD_NOSEQ;
   }

   const char *dirName  = am->ArgMgrGetString("dirin");
   if (dirName == 0)
   {
       std::cout <<std::endl
                 << "'dirin=' must be present;" 
                 <<  std::endl;
       am->ArgMgrUsage(usage); // Display 'usage'  
       delete am;
       return 0;
 }

   // FIXME : check only one of them is set !

   int pos  = am->ArgMgrDefined("pos");
   int ori  = am->ArgMgrDefined("ori");
   int sort = am->ArgMgrDefined("sort");
      
   int nb;
   uint16_t *groupelem;
   groupelem = am->ArgMgrGetXInt16Enum("tag", &nb);

   if (groupelem != 0)
   {
      if (nb != 1)
      {
         std::cout << "TAG : one and only one group,elem!" << std::endl;
         delete am;
         return 0;
      }
   }      
     
       
   /* if unused Param we give up */
   if ( am->ArgMgrPrintUnusedLabels() )
   {
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   } 
 
   delete am;  // ------ we don't need Arguments Manager any longer ------
   
                 
   GDCM_NAME_SPACE::SerieHelper *s;
  
   s = GDCM_NAME_SPACE::SerieHelper::New();
   s->SetLoadMode(GDCM_NAME_SPACE::LD_ALL);     // Load everything for each File
   //GDCM_NAME_SPACE::TagKey t(0x0020,0x0013);
   //s->AddRestriction(t, "340", GDCM_NAME_SPACE::GDCM_LESS); // Keep only files where
                                              // restriction is true
   s->SetDirectory(dirName, true); // true : recursive exploration

   // The Dicom file set is splitted into several 'Single SerieUID Files Sets'
   // (a 'Single SerieUID Files Set' per SerieUID)
   // In some cases, it's not enough, since, in some cases
   // we can find scout view with the same SerieUID
   
/*
   std::cout << " ---------------------------------------- "
             << "'Single UID' Filesets found in :["
             << dirName << "]" << std::endl;

   s->Print();
   std::cout << " ------------------------------------- Result after splitting"
             << std::endl;
*/
   int nbFiles;
   std::string fileName;
   // For all the Single SerieUID Files Sets of the GDCM_NAME_SPACE::Serie
   GDCM_NAME_SPACE::FileList *l = s->GetFirstSingleSerieUIDFileSet();
   
   GDCM_NAME_SPACE::XCoherentFileSetmap xcm;
   while (l) // for each 'Single SerieUID FileSet'
   { 
      nbFiles = l->size() ;
      if ( l->size() > 3 ) // Why not ? Just an example, for testing
      {
         std::cout << "Split the 'Single SerieUID' FileSet :[" 
                   << s->GetCurrentSerieUIDFileSetUID()
                   << "]  " << nbFiles << " long" << std::endl;
         std::cout << "-----------------------------------" << std::endl;
  
         if (ori) 
            xcm = s->SplitOnOrientation(l);
         if (pos)
            xcm = s->SplitOnPosition(l);
         if (groupelem != 0)
            xcm = s->SplitOnTagValue(l, groupelem[0],groupelem[1] );
    
         for (GDCM_NAME_SPACE::XCoherentFileSetmap::iterator i = xcm.begin();
                                                  i != xcm.end();
                                                ++i)
         {
            if (ori) 
               std::cout << "Orientation : ";
            if (pos) 
               std::cout << "Position : ";
            if (groupelem != 0)    
               std::cout << "Tag (" << std::hex << groupelem[0] 
                                 << "|" << groupelem[1] << ") value : ";
    
             std::cout << "[" << (*i).first << "]" << std::endl;
    
           // Within a 'just to see' program, 
           // OrderFileList() causes trouble, since some files
           // (eg:MIP views) don't have 'Position', now considered as mandatory
           // Commented out for the moment.
           
           if (sort) {   
              s->OrderFileList((*i).second);  // sort the XCoherent Fileset
              std::cout << "ZSpacing for the file set " << s->GetZSpacing()
                        << std::endl;
            } 

            for (GDCM_NAME_SPACE::FileList::iterator it =  ((*i).second)->begin();
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
  
   s->Delete();

   return 0;
}
