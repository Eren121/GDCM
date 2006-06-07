/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: AnonymizeMultiPatient.cxx,v $
  Language:  C++
  Date:      $Date: 2006/06/07 12:22:50 $
  Version:   $Revision: 1.4 $
                                                                                
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
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirVisit.h"
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirImage.h"
#include "gdcmDirList.h"
#include "gdcmDebug.h"
#include "gdcmUtil.h"
#include "gdcmFile.h"
#include "gdcmFileHelper.h"

#include "gdcmArgMgr.h"

#include <iostream>

/**
  * \brief   Explores recursively the given directory
  *          orders the gdcm-readable found Files
  *          according their Patient/Study/Serie/Image characteristics
  *          and anomymizes (NoLoad) them, creates a different name for each Patient.
  */  

int main(int argc, char *argv[]) 
{
   START_USAGE(usage)
   " \n AnonymizeMultiPatient :\n                                             ",
   " AnonymizeMultiPatient a full gdcm-readable Dicom image                   ",
   "         Warning : the image is OVERWRITTEN                               ",
   "                   to preserve image integrity, use a copy.               ",
   " usage: AnonymizeMultiPatient dirin=inputDirectoryName                    ",
   "       listOfElementsToRubOut : group-elem,g2-e2,... (in hexa, no space)  ",
   "                                of extra Elements to rub out              ",
   "       noshadowseq: user doesn't want to load Private Sequences           ",
   "       noshadow   : user doesn't want to load Private groups (odd number) ",
   "       noseq      : user doesn't want to load Sequences                   ",
   "       verbose    : user wants to run the program in 'verbose mode'       ",   
   "       debug      : user wants to run the program in 'debug mode'         ",   
   FINISH_USAGE
   


   // ----- Initialize Arguments Manager ------   
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage")) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }
   const char * name  = am->ArgMgrGetString("dirin");
   if ( name == NULL )
   {
      delete am;
      return 0;
   }
   
   std::string dirName = name;
 
   int verbose  = am->ArgMgrDefined("verbose");
   
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


   int rubOutNb;
   uint16_t *elemsToRubOut = am->ArgMgrGetXInt16Enum("rubout", &rubOutNb);
 
   // ----------- if unused Param we give up
   if ( am->ArgMgrPrintUnusedLabels() )
   { 
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }

   delete am;  // we don't need Argument Manager any longer

 
  // ---------------------------------------------------------- 
  

   // ----- Begin Processing -----

   gdcm::DicomDir *dcmdir;

   // we ask for Directory parsing

   dcmdir = gdcm::DicomDir::New( );
   dcmdir->SetLoadMode(loadMode);
   dcmdir->SetDirectoryName(dirName);
   dcmdir->Load();

   if ( verbose )
      std::cout << "======================= End Parsing Directory" << std::endl;
      
    // ----- Check the result
    
   if ( !dcmdir->GetFirstPatient() ) 
   {
      std::cout << "No patient found (?!?). Exiting."
                << std::endl;
      dcmdir->Delete();
      return 1;
   }

   gdcm::DicomDirPatient *pa;
   gdcm::DicomDirStudy *st;
   gdcm::DicomDirSerie *se;
   gdcm::DicomDirImage *im;

   std::string codedName; 
   std::string fullFileName;
   std::string patName;
      
   gdcm::File *f;
  
   pa = dcmdir->GetFirstPatient(); 
   while ( pa )
   {  // on degouline les PATIENT du DICOMDIR
      patName = pa->GetEntryString(0x0010, 0x0010);
      codedName = "g^" + gdcm::Util::ConvertToMD5(patName);
      if (verbose)
         std::cout << patName << " --> " << codedName << std::endl;         
      st = pa->GetFirstStudy();
      while ( st ) 
      { // on degouline les STUDY de ce patient
         se = st->GetFirstSerie();
         while ( se ) 
         { // on degouline les SERIES de cette study
            im = se->GetFirstImage();
            while ( im ) 
            { // on degouline les Images de cette serie       
               fullFileName = dirName;
               fullFileName +=  gdcm::GDCM_FILESEPARATOR;
               fullFileName += im->GetEntryString(0x0004, 0x1500);
               if (verbose)
                  std::cout << "FileName " << fullFileName << std::endl;

               f = gdcm::File::New( );
               f->SetLoadMode(loadMode);
               f->SetFileName( fullFileName );
               if ( !f->Load() )
                 std::cout << "Load failed for [" << fullFileName << "]" << std::endl; 
               else
                  if (verbose)
                     std::cout << "Load successed for [" << fullFileName << "]" << std::endl;

               // 
               //  Choose the fields to anonymize.
               // 

               // Institution name 
               f->AddAnonymizeElement( 0x0008, 0x0080, "Xanadoo" );

               // Patient's name 
               f->AddAnonymizeElement( 0x0010, 0x0010, codedName ); 
    
               // Patient's ID
               f->AddAnonymizeElement( 0x0010, 0x0020,"1515" );
               // Patient's Birthdate
               f->AddAnonymizeElement( 0x0010, 0x0030,"11111111" );
               // Patient's Adress
               f->AddAnonymizeElement( 0x0010, 0x1040,"Sing-sing" );
               // Patient's Mother's Birth Name
               f->AddAnonymizeElement( 0x0010, 0x1060,"g^Vampirella" );
       
               // Study Instance UID
               // we may not brutaly overwrite it
               //f->AddAnonymizeElement( 0x0020, 0x000d, "9.99.999.9999" );
   
               // Telephone
               f->AddAnonymizeElement(0x0010, 0x2154, "3615" );

               // deal with user defined Elements set

               for (int ri=0; ri<rubOutNb; ri++)
               {
                  f->AddAnonymizeElement((uint32_t)elemsToRubOut[2*ri], 
                                         (uint32_t)elemsToRubOut[2*ri+1],"*" ); 
               }

              // 
              //      Overwrite the file
              // 
              // The gdcm::File remains untouched in memory    
   
              f->AnonymizeNoLoad();     

              f->ClearAnonymizeList();
              f->Delete();
 
              im = se->GetNextImage();   
           }
           se = st->GetNextSerie();   
        }
        st = pa->GetNextStudy();
     }     
     pa = dcmdir->GetNextPatient();    
   }

   dcmdir->Delete();        
   return 0;
   
}
