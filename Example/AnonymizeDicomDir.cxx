/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: AnonymizeDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/12 14:44:09 $
  Version:   $Revision: 1.5 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmGlobal.h"
#include "gdcmCommon.h"
#include "gdcmDebug.h"
#include "gdcmUtil.h"

#include "gdcmSQItem.h"
#include "gdcmSeqEntry.h"
#include "gdcmValEntry.h"

#include "gdcmDocument.h"
#include "gdcmFile.h"

#include "gdcmArgMgr.h"

#include <iostream>

/**
 * \brief AnonymizeDicomDir
 */

void AnoNoLoad(gdcm::SQItem *s, std::fstream *fp, 
               uint16_t group, uint16_t elem, 
               std::string val);

void AnoNoLoad(gdcm::SQItem *s, std::fstream *fp, 
               uint16_t group, uint16_t elem, 
               std::string val)
{
   gdcm::DocEntry *d;
   uint32_t offset;
   uint32_t lgth;
   uint32_t valLgth = 0;
   std::string *spaces;
   std::string v;

   d = s->GetDocEntry( group, elem);

   if ( d == NULL)
      return;

   if ( ! dynamic_cast<gdcm::ValEntry *>(d) )
      return;

   offset = d->GetOffset();
   lgth =   d->GetLength();
   if (valLgth < lgth)
   {
      spaces = new std::string( lgth-valLgth, ' ');
      v = val + *spaces;
      delete spaces;
   }
   fp->seekp( offset, std::ios::beg );
   fp->write( v.c_str(), lgth );
}


int main(int argc, char *argv[])
{ 

   START_USAGE(usage)
   " \n AnonymizeDicomDir :\n",
   " Anonymize a gdcm-readable DICOMDIR ",
   "           even when some 'Objects' are not yet taken into account",
   "           Warning : the DICOMDIR is overwritten",
   " usage: AnonymizeDicomDir filein=dicomDirName [debug] ",
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
 
   char *fileName  = am->ArgMgrWantString("filein",usage); 

   delete am;  // --- we don't need Argument Manager any longer ---


//   Read the input DICOMDIR
   gdcm::File *f;
   f = new gdcm::File( );
   f->SetLoadMode(0);
   f->SetFileName( fileName );
   bool res = f->Load();  
   if ( !res )
   {
       std::cerr << "Sorry, " << fileName <<"  not a gdcm-readable "
                 << "file" <<std::endl;
   }
   std::cout << " ... is readable " << std::endl;

   // Look for Directory record sequence
   gdcm::DocEntry *e = f->GetDocEntry(0x0004, 0x1220);
   if ( !e )
   {
      std::cout << "No Directory Record Sequence (0004,1220) found" <<std::endl;;
      delete f;
      return 0;         
   }
   
   gdcm::SeqEntry *s = dynamic_cast<gdcm::SeqEntry *>(e);
   if ( !s )
   {
      std::cout << "Element (0004,1220) is not a Sequence ?!?" <<std::endl;
      delete f;
      return 0;
   }

   // Open the file LTTG (aka ALAP)
   std::fstream *fp = new std::fstream(fileName, 
                              std::ios::in | std::ios::out | std::ios::binary);
   gdcm::DocEntry *d;
   std::string v;

   int patientNumber = 0;
   std::ostringstream oss;

   gdcm::SQItem *tmpSI=s->GetFirstSQItem();  // For all the SQItems
   while(tmpSI)
   {
      d = tmpSI->GetDocEntry(0x0004, 0x1430); // Directory Record Type
      if ( gdcm::ValEntry* valEntry = dynamic_cast<gdcm::ValEntry *>(d) )
      {
         v = valEntry->GetValue();
      }
      else
      {
         std::cout << "(0004,1430) not a ValEntry ?!?" << std::endl;
         continue;
      }

      if( v != "PATIENT " )  // Work only on PATIENT
      {
         tmpSI=s->GetNextSQItem();
         continue;
      }

      oss << patientNumber;      

      //   Overwrite the sensitive Entries

      // Patient's Name
      AnoNoLoad(tmpSI, fp, 0x0010, 0x0010, oss.str());
      // Patient's ID
      AnoNoLoad(tmpSI, fp, 0x0010, 0x0020, oss.str());
      // Patient's Birth Date
      AnoNoLoad(tmpSI, fp, 0x0010, 0x0030, oss.str());
     // Telephone
      AnoNoLoad(tmpSI, fp, 0x0010, 0x2154, oss.str()); 

    // Aware use will add here more Entries if he wants to rubb them out

      oss << "";
      patientNumber++;
      tmpSI=s->GetNextSQItem();
   }

   // Close the file ASAP

   fp->close();

   delete f;
   return 0;
}

