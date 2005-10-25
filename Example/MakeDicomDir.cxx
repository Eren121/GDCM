/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: MakeDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/25 14:52:26 $
  Version:   $Revision: 1.16 $
                                                                                
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
#include "gdcmDirList.h"
#include "gdcmDebug.h"
#include "gdcmArgMgr.h"

#include <iostream>

// ---
void StartMethod(void *toto) {
  (void)toto;
   std::cout<<"Start parsing"<<std::endl;
}

void EndMethod(void *toto) {
  (void)toto;
   std::cout<<"End parsing"<<std::endl;
}
// ---

/**
  * \brief   Explores recursively the given directory
  *          orders the gdcm-readable found Files
  *          according their Patient/Study/Serie/Image characteristics
  *          makes the gdcmDicomDir 
  *          and writes a file named NewDICOMDIR..
  */  

int main(int argc, char *argv[]) 
{
   START_USAGE(usage)
   " \n MakeDicomDir :\n                                                      ",
   " Explores recursively the given directory, makes the relevant DICOMDIR    ",
   "          and writes it as 'NewDICOMDIR'                                  ",
   "                                                                          ", 
   " usage: MakeDicomDir dirname=rootDirectoryName                            ",
   "        [noshadowseq][noshadow][noseq] [debug]                            ",
   "                                                                          ",
   "        noshadowseq: user doesn't want to load Private Sequences          ",
   "        noshadow : user doesn't want to load Private groups (odd number)  ",
   "        noseq    : user doesn't want to load Sequences                    ",
   "        debug    : user wants to run the program in 'debug mode'          ",
   FINISH_USAGE

   // ----- Initialize Arguments Manager ------   
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage")) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }

   char *dirName;   
   dirName  = am->ArgMgrGetString("dirName",(char *)"."); 

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

   gdcm::DicomDir *dcmdir;

   // we ask for Directory parsing

   dcmdir = gdcm::DicomDir::New( );

   dcmdir->SetStartMethod(StartMethod);
   dcmdir->SetEndMethod(EndMethod);

   dcmdir->SetLoadMode(loadMode);
   dcmdir->SetDirectoryName(dirName);
   //dcmdir->SetParseDir(true);
   dcmdir->Load();

    // ----- Check the result
    
   if ( !dcmdir->GetFirstPatient() ) 
   {
      std::cout << "makeDicomDir: no patient found. Exiting."
                << std::endl;
      dcmdir->Delete();
      return 1;
   }
    
   // ----- Create the corresponding DicomDir

   dcmdir->Write("NewDICOMDIR");
   dcmdir->Delete();

   // Read from disc the just written DicomDir
   gdcm::DicomDir *newDicomDir = gdcm::DicomDir::New();
   newDicomDir->SetFileName( "NewDICOMDIR" );
   newDicomDir->Load();
   if( !newDicomDir->IsReadable() )
   {
      std::cout<<"          Written DicomDir 'NewDICOMDIR'"
               <<" is not readable"<<std::endl
               <<"          ...Failed"<<std::endl;

      newDicomDir->Delete();
      return 1;
   }

   if( !newDicomDir->GetFirstPatient() )
   {
      std::cout<<"          Written DicomDir 'NewDICOMDIR'"
               <<" has no patient"<<std::endl
               <<"          ...Failed"<<std::endl;

      newDicomDir->Delete();
      return(1);
   }

   std::cout<<std::flush;

   newDicomDir->Delete();
   return 0;
}
