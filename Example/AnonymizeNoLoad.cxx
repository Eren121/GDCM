/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: AnonymizeNoLoad.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/07 13:10:57 $
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
#include "gdcmDebug.h"

#include "gdcmArgMgr.h"

#include <iostream>

int main(int argc, char *argv[])
{
   START_USAGE(usage)
   " \n AnonymizeNoLOad :\n",
   " Anonymize a gdcm-readable Dicom image even if pixels aren't gdcm readable",
   "          Warning : Warning : the image is overwritten",
   "                    to preserve image integrity, use a copy.",
   " usage: AnonymizeNoLoad filein=inputFileName fileout=[debug] ",
   "        debug    : user wants to run the program in 'debug mode' ",
   FINISH_USAGE

   // ----- Initialize Arguments Manager ------   
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);
  
   if (am->ArgMgrDefined("usage")) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();

   char *fileName = am->ArgMgrWantString("filein",usage);

   int loadMode;
   if ( am->ArgMgrDefined("noshadow") && am->ArgMgrDefined("noseq") )
       loadMode = NO_SEQ | NO_SHADOW;  
   else if ( am->ArgMgrDefined("noshadow") )
      loadMode = NO_SHADOW;
   else if ( am->ArgMgrDefined("noseq") )
      loadMode = NO_SEQ;
   else
      loadMode = 0;

   delete am;  // we don't need Argument Manager any longer

   // ============================================================
   //   Parse the input file.
   // ============================================================

   gdcm::File *f1;
   f1 = new gdcm::File( );
   f1->SetLoadMode(loadMode);
   int res = f1->Load(fileName);

   // gdcm::File::IsReadable() is no usable here, because we deal with
   // any kind of gdcm::Readable *document*
   // not only gdcm::File (as opposed to gdcm::DicomDir)
   if ( !res ) 
   {
       std::cout <<std::endl
           << "Sorry, " << fileName <<"  not a gdcm-readable "
           << "DICOM / ACR Document"
           << std::endl;
        delete f1;
        return 1;
   }
   std::cout << fileName << " is readable " << std::endl;

   // ============================================================
   //   No need to load the pixels in memory.
   //   File will be overwritten
   // ============================================================


   // ============================================================
   //  Choose the fields to anonymize.
   // ============================================================
   // Institution name 
   f1->AddAnonymizeElement( 0x0008, 0x0080, "Xanadoo" ); 
   // Patient's name 
   f1->AddAnonymizeElement( 0x0010, 0x0010, "Fantomas" );   
   // Patient's ID
   f1->AddAnonymizeElement( 0x0010, 0x0020,"1515" );
   // Patient's Birthdate
   f1->AddAnonymizeElement( 0x0010, 0x0030,"11.11.1111" );
   // Patient's Adress
   f1->AddAnonymizeElement( 0x0010, 0x1040,"Sing-sing" );
   // Patient's Mother's Birth Name
   f1->AddAnonymizeElement( 0x0010, 0x1060,"Vampirella" );   
   // Study Instance UID
   f1->AddAnonymizeElement( 0x0020, 0x000d, "9.99.999.9999" );
   // Telephone
   f1->AddAnonymizeElement(0x0010, 0x2154, "3615" );

  // Aware use will add new fields here

   // ============================================================
   //   Overwrite the file
   // ============================================================

   std::cout <<"Let's AnonymizeNoLoad " << std::endl;;

   // The gdcm::File remains untouched in memory

   f1->AnonymizeNoLoad();

   // No need to write the File : modif were done on disc !
   // File was overwritten ...

   std::cout <<"End AnonymizeNoLoad" << std::endl;

   // ============================================================
   //   Remove the Anonymize list
   // ============================================================  
   f1->ClearAnonymizeList();
    
   delete f1;
   return 0;
}

