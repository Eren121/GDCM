/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exXCoherentFileSet.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/14 15:55:17 $
  Version:   $Revision: 1.6 $
                                                                                
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
   "                       { tag= group-elem | pos | ori }                    ",
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
   "       noshadowseq: user doesn't want to load Private Sequences           ",
   "       noshadow   : user doesn't want to load Private groups (odd number) ",
   "       noseq      : user doesn't want to load Sequences                   ",
   "       debug      : user wants to run the program in 'debug mode'         ",
   FINISH_USAGE
   
   // ----- Initialize Arguments Manager ------
  
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);
  
   if (am->ArgMgrDefined("usage") || argc == 1) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();
      
   int loadMode = gdcm::LD_ALL;
   if ( am->ArgMgrDefined("noshadowseq") )
      loadMode |= gdcm::LD_NOSHADOWSEQ;
   else 
   {
      if ( am->ArgMgrDefined("noshadow") )
         loadMode |= gdcm::LD_NOSHADOW;
      if ( am->ArgMgrDefined("noseq") )
         loadMode |= gdcm::LD_NOSEQ;
   }

   char *dirName  = am->ArgMgrGetString("dirin",(char *)0);
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

   int pos = am->ArgMgrDefined("pos");
   int ori = am->ArgMgrDefined("ori");
   
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
   
                 
   gdcm::SerieHelper *s;
  
   s = gdcm::SerieHelper::New();
   s->SetLoadMode(gdcm::LD_ALL);     // Load everything for each File
   //gdcm::TagKey t(0x0020,0x0013);
   //s->AddRestriction(t, "340", gdcm::GDCM_LESS); // Keep only files where
                                              // restriction is true
   s->SetDirectory(dirName, true); // true : recursive exploration

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
   // For all the Single SerieUID Files Sets of the gdcm::Serie
   gdcm::FileList *l = s->GetFirstSingleSerieUIDFileSet();
   
   gdcm::XCoherentFileSetmap xcm;
   while (l)
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
    
         for (gdcm::XCoherentFileSetmap::iterator i = xcm.begin();
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
  
   s->Delete();

   return 0;
}
