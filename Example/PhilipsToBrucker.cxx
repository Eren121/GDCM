/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PhilipsToBrucker.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/21 15:01:04 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmDocEntry.h"
#include "gdcmDicomDir.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmFile.h"
#include "gdcmDirList.h"
#include "gdcmDebug.h"
#include "gdcmArgMgr.h"
#include "gdcmUtil.h"
#include "gdcmSerieHelper.h"

#include <iostream>

/**
  * \brief   
  *          - explores recursively the given directory
  *          - keeps the requested series
  *          - orders the gdcm-readable found Files
  *            according their Patient/Study/Serie/Image characteristics
  *          - fills a single level Directory with *all* the files,
  *            converted into a Brucker-like Dicom, Intags compliant
  *          
  */  

typedef std::map<std::string, gdcm::File*> SortedFiles;

int main(int argc, char *argv[]) 
{
/*
   START_USAGE(usage)
   " \n PhilipsToBrucker :\n                                                  ",
   " Explores recursively the given directory,                                ",
   "                                                                          ", 
   " usage: PhilipsToBrucker dirin=rootDirectoryName                          ",
   "                         dirout=outputDirectoryName                       ",
   "        [noshadowseq][noshadow][noseq] [debug]                            ",
   "                                                                          ",
   "        noshadowseq: user doesn't want to load Private Sequences          ",
   "        noshadow : user doesn't want to load Private groups (odd number)  ",
   "        noseq    : user doesn't want to load Sequences                    ",
   "        debug    : user wants to run the program in 'debug mode'          ",
   FINISH_USAGE
*/
const char **usage;

   // ----- Initialize Arguments Manager ------   
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage")) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }

   char *dirNamein;   
   dirNamein  = am->ArgMgrGetString("dirin",(char *)"."); 

   char *dirNameout;   
   dirNameout  = am->ArgMgrGetString("dirout",(char *)".");  
   
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

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();
 
   // if unused Param we give up
   if ( am->ArgMgrPrintUnusedLabels() )
   { 
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }

   delete am;  // we don't need Argument Manager any longer

   // ----- Begin Processing -----

   std::string strDirNamein(dirNamein);
   gdcm::DirList dirList(strDirNamein, true); 
   
 /*  
   std::cout << "---------------File list found ------------" << std::endl;
   dirList.Print();
 */   

   gdcm::DirListType fileNames;
   fileNames = dirList.GetFilenames();
   gdcm::SerieHelper *s;  
   s = gdcm::SerieHelper::New();

/*       
   std::cout << "---------------Print Serie--------------" << std::endl; 
   s->SetDirectory(dirNamein, true); // true : recursive exploration 
   s->SetUseSeriesDetails(true);  
   s->AddSeriesDetail(0x0018, 0x1312);   
   s->Print();
*/
   

   
   gdcm::File *f;

/*    
   std::cout << "---------------Print Unique Series identifiers---------"  << std::endl;     
   std::string uniqueSeriesIdentifier;
 
   for (gdcm::DirListType::iterator it = fileNames.begin();  
                                    it != fileNames.end();
                                  ++it)
   {
      std::cout << "File Name : " << *it << std::endl;
      f = gdcm::File::New();
      f->SetLoadMode(gdcm::LD_ALL);
      f->SetFileName( *it );
      f->Load();
        
      uniqueSeriesIdentifier=s->CreateUniqueSeriesIdentifier(f);
      std::cout << "                           [" <<
               uniqueSeriesIdentifier  << "]" << std::endl;
       
      f->Delete();
   }
*/
   
   std::cout << "------------------Print Break levels-----------------" << std::endl;

   std::string userFileIdentifier; 
   SortedFiles sf;


   s->AddSeriesDetail(0x0010, 0x0010, false); // Patient's Name
   s->AddSeriesDetail(0x0020, 0x000e, false); // Series Instance UID
   s->AddSeriesDetail(0x0020, 0x0032, false); // Image Position (Patient)     
   s->AddSeriesDetail(0x0018, 0x1312, false); // In-plane Phase Encoding Direction 
   s->AddSeriesDetail(0x0018, 0x1060, true);  // Trigger Time 
   
   for (gdcm::DirListType::iterator it = fileNames.begin();  
                                    it != fileNames.end();
                                  ++it)
   {
      //std::cout << "File Name : " << *it << std::endl;
      f = gdcm::File::New();
      f->SetLoadMode(gdcm::LD_ALL);
      f->SetFileName( *it );
      f->Load();

      userFileIdentifier=s->CreateUserDefinedFileIdentifier(f);        
      //std::cout << "                           [" <<
      //        userFileIdentifier  << "]" << std::endl;
      // storing in a map ensures automatic sorting !      
      sf[userFileIdentifier] = f;
   }
      
   std::vector<std::string> tokens;
   std::string fullFilename;
   std::string previousPatientName, currentPatientName;
   std::string previousSerieInstanceUID, currentSerieInstanceUID;
   std::string previousImagePosition, currentImagePosition;
   std::string previousPhaseEncodingDirection, currentPhaseEncodingDirection;
   
   SortedFiles::iterator it2 = sf.begin();
      
   gdcm::Util::Tokenize (it2->first, tokens, "_");
   
   previousPatientName            = tokens[0];
   previousSerieInstanceUID       = tokens[1];
   previousImagePosition          = tokens[2];
   previousPhaseEncodingDirection = tokens[3];
   std::cout << "==== new Patient "                            << currentPatientName      << std::endl;   
   std::cout << "==== === new Serie "                          << currentSerieInstanceUID << std::endl;
   std::cout << "==== === === new Position "                   << currentImagePosition    << std::endl; 
   std::cout << "==== === === === new PhaseEncodingDirection " << currentImagePosition    << std::endl;
   std::cout << "==== === === ===    "                         << it2->first              << std::endl; 
    
   it2++;
   
   for ( ; it2 != sf.end(); ++it2)
   {
      tokens.clear();
      gdcm::Util::Tokenize (it2->first, tokens, "_");
      
      currentPatientName            = tokens[0];
      currentSerieInstanceUID       = tokens[1];
      currentImagePosition          = tokens[2];
      currentPhaseEncodingDirection = tokens[3];     
      
      if (previousPatientName != currentPatientName)
      {
         previousPatientName = currentPatientName;
         std::cout << "==== new Patient " << currentPatientName << std::endl;
         previousPatientName            = currentPatientName;
         previousSerieInstanceUID       = currentSerieInstanceUID;
         previousImagePosition          = currentImagePosition;
         previousPhaseEncodingDirection = currentPhaseEncodingDirection;
      }

      if (previousSerieInstanceUID != currentSerieInstanceUID)
      {        
         std::cout << "==== === new Serie " << currentSerieInstanceUID << std::endl;
         previousSerieInstanceUID       = currentSerieInstanceUID;
         previousImagePosition          = currentImagePosition;
         previousPhaseEncodingDirection = currentPhaseEncodingDirection;
      }

      if (previousImagePosition != currentImagePosition)
      {        
         std::cout << "==== === === new Position " << currentImagePosition << std::endl;
         previousImagePosition          = currentImagePosition;
         previousPhaseEncodingDirection = currentPhaseEncodingDirection;
      }      

      if (previousPhaseEncodingDirection != currentPhaseEncodingDirection)
      {        
         std::cout << "==== === === === new PhaseEncodingDirection " << currentImagePosition << std::endl;
         previousPhaseEncodingDirection = currentPhaseEncodingDirection;
      } 
      fullFilename =  (it2->second)->GetFileName();          
      std::cout << "==== === === ===    " << it2->first << "  " << (it2->second)->GetFileName() << " " 
                << gdcm::Util::GetName( fullFilename ) <<std::endl;
      
   }
 }

