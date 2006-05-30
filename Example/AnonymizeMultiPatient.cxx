/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: AnonymizeMultiPatient.cxx,v $
  Language:  C++
  Date:      $Date: 2006/05/30 08:26:36 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmCommon.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"
#include "gdcmDirList.h"

#include "gdcmDicomDir.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirImage.h"

#include "gdcmArgMgr.h"

#include <iostream>


int main(int argc, char *argv[])
{
   START_USAGE(usage)
   " \n AnonymizeMultiPatient :\n                                             ",
   " AnonymizeMultiPatient a full gdcm-readable Dicom image                   ",
   "         Warning : the image is OVERWRITTEN                               ",
   "                   to preserve image integrity, use a copy.               ",
   " usage: AnonymizeMultiPatient dirin=inputDirectoryName dirout=outputDirectoryName",
   "       listOfElementsToRubOut : group-elem,g2-e2,... (in hexa, no space)  ",
   "                                of extra Elements to rub out              ",
   "       noshadowseq: user doesn't want to load Private Sequences           ",
   "       noshadow   : user doesn't want to load Private groups (odd number) ",
   "       noseq      : user doesn't want to load Sequences                   ",
   "       verbose  : user wants to run the program in 'verbose mode'         ",   
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
   const char *dirName  = am->ArgMgrGetString("dirin");
   if ( dirName == NULL )
   {
      delete am;
      return 0;
   }
   
   /*
   char *outputDirName = am->ArgMgrWantString("dirout",usage);
   if ( outputDirName == NULL )
   {
      delete am;
      return 0;
   }
 */
   
   bool verbose  = am->ArgMgrDefined("verbose");
   
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
 
   // if unused Param we give up
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



   gdcm::DicomDirPatient *pa;
   gdcm::DicomDirStudy *st;
   gdcm::DicomDirSerie *se;
   gdcm::DicomDirVisit *vs;
   gdcm::DicomDirImage *im;      
  
   // Test if the DicomDir contains any Patient
   pa = dcmdir->GetFirstPatient();
   if ( pa == 0)
   {
      std::cout<<"          DicomDir '"<< dirName
               <<" has no patient"<<std::endl
               <<"          ...Failed"<<std::endl;

      dcmdir->Delete();
      return 1;
   } 
   
      if ( verbose )
      std::cout << "-------------------------- Directory [" << dirName << "] parsed" << std::endl;

// Since files may be help in a Directory tree-like structure
// we should have to duplicate this structure.
// No time to code that stuff
//  --> We 'AnonymizeNoLoad' (overwrite the files)

// Do not remove the commented out lines.
// They will be usefull in a further program. 
/*
   std::string systemCommand;
   
   std::cout << "Check for output directory :[" << outputDirName << "]."
             <<std::endl;
   if ( ! gdcm::DirList::IsDirectory(outputDirName) )    // dirout not found
   {
      std::string strDirNameout(outputDirName);          // to please gcc 4
      systemCommand = "mkdir " +strDirNameout;        // create it!
      if (verbose)
         std::cout << systemCommand << std::endl;
      system (systemCommand.c_str());
      if ( ! gdcm::DirList::IsDirectory(outputDirName) ) // be sure it worked
      {
          std::cout << "KO : not a dir : [" << outputDirName << "] (creation failure ?)" << std::endl;
          exit(0);
      }
      else
      {
        std::cout << "Directory [" << outputDirName << "] created." << std::endl;
      }
   }
   else
   {
       std::cout << "Output Directory [" << outputDirName << "] already exists; Used as is." << std::endl;
   }
*/

  gdcm::File *f;
  gdcm::FileHelper *fh;
 // std::string outputFileName;

  std::string codedName; 
  while (pa)
  {
     if (verbose)
     {
        std::cout << "Pat.Name:[" << pa->GetEntryString(0x0010, 0x0010) <<"]"; // Patient's Name
        std::cout << " Pat.ID:[";
        std::cout << pa->GetEntryString(0x0010, 0x0020) << "]" << std::endl; // Patient ID
     }     
     std::string patName = pa->GetEntryString(0x0010, 0x0010);
     //codedName = "g^" + gdcm::Util::ConvertToMD5(patName); // just to be sure MD5 is not guilty
     std::string fullFileName;
     if (verbose)
        std::cout << patName << " --> " << codedName << std::endl;
         
     st = pa->GetFirstStudy();
      while ( st ) 
      { // on degouline les STUDY de ce patient
         if (verbose)
         {
            std::cout << "--- Stud.descr:["    << st->GetEntryString(0x0008, 0x1030) << "]"; // Study Description    
            std::cout << " Stud.ID:["          << st->GetEntryString(0x0020, 0x0010) << "]"; // Study ID
            std::cout << std::endl;
         }
         se = st->GetFirstSerie();
         while ( se ) 
         { // on degouline les SERIES de cette study
            if (verbose)
            {
               std::cout << "--- --- Ser.Descr:["<< se->GetEntryString(0x0008, 0x103e) << "]";  // Series Description
               std::cout << " Ser.nb:["          <<  se->GetEntryString(0x0020, 0x0011) << "]";  // Series number
               std::cout << " Mod.:["      <<  se->GetEntryString(0x0008, 0x0060) << "]";  // Modality
               std::cout << " Serie Inst.UID.:[" <<  se->GetEntryString(0x0020, 0x000e) << "]";  // Series Instance UID
               std::cout << std::endl; 
            }
  
            im = se->GetFirstImage();
            std::string ReferencedFileID;
            while ( im ) 
            { // on degouline les IMAGEs de cette serie
               ReferencedFileID = im->GetEntryString(0x0004, 0x1500);
               if (verbose)
               {
                  std::cout << "--- --- --- "<< " IMAGE Ref. File ID :[" << ReferencedFileID 
                            << "]" << std::endl; // File name (Referenced File ID)
               }       
               f = gdcm::File::New( );
               f->SetLoadMode(loadMode);
               fullFileName = dirName;
       
               std::cout << "fullFileName (1) " << fullFileName << std::endl;
               std::cout << "ReferencedFileID " << ReferencedFileID << std::endl;
               fullFileName = fullFileName + "/" + ReferencedFileID;       
               std::cout << "fullFileName (2) " << fullFileName << std::endl;

               f->SetFileName( fullFileName );
       
               if ( !f->Load() )
                   std::cout << "Load failed for [" << fullFileName << "]" << std::endl; 
               else
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

std::cout << "rubOutNb " << rubOutNb << std::endl;
               for (int ri=0; ri<rubOutNb; ri++)
               {
                  f->AddAnonymizeElement((uint32_t)elemsToRubOut[2*ri], 
                       (uint32_t)elemsToRubOut[2*ri+1],"*" ); 
               }
      
               // The gdcm::File is modified in memory
               // f->AnonymizeFile();    

               // 
               //      Overwrite the file
               // 
               // The gdcm::File remains untouched in memory    
               f->AnonymizeNoLoad();     

               // ============================================================
               //               Write a new file
               // ============================================================

               // 
               //      No need to load the pixels in memory.
               //      File will be overwritten

               // Do not remove the commented out lines.
               // They will be usefull in a further program.
               // 
/*     
               // Get the Pixels
               fh = gdcm::FileHelper::New(f);

               // unit8_t DOESN'T mean it's mandatory for the image to be a 8 bits one !
               // Feel free to cast if you know it's not. 

               uint8_t *imageData = fh->GetImageData();

               if ( imageData == 0 )
               {
                  std::cerr << "Sorry, Pixels of" << im->GetEntryString(0x0004, 0x1500) <<"  are not "
                      << " gdcm-readable."       << std::endl
                      << "Use AnonymizeNoLoad" << std::endl;
                  f->Delete();
                  fh->Delete();
                  break;
                } 

                     // Since we just Anonymized the file, we *know* no modification 
                     // was performed on the pixels.
                     // The written image will not appear as a 'Secondary Captured image'
                     // nor as a DERIVED one

                     fh->SetContentType(gdcm::UNMODIFIED_PIXELS_IMAGE);
                     outputFileName = outputDirName + "/" + im->GetEntryString(0x0004, 0x1500);
                     fh->WriteDcmExplVR(outputFileName);
                     std::cout <<"End Anonymize" << std::cout;

                     fh->Delete();       
*/

                     f->Delete();
                     f->ClearAnonymizeList();
     
                     im = se->GetNextImage();   
                  }
                  se = st->GetNextSerie();   
               }  
               st = pa->GetNextStudy();
            }         
     
     pa = dcmdir->GetNextPatient();    
  }
  
}

