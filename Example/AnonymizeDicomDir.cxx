/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: AnonymizeDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/09 19:15:04 $
  Version:   $Revision: 1.1 $
                                                                                
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

#include <iostream>

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
   gdcm::File *f1;
 
   gdcm::Debug::DebugOn();
   std::cout << "------------------------------------------------" << std::endl;
   std::cout << "Anonymize a gdcm-readable DICOMDIR   "            << std::endl;
   std::cout << "even some Objects are not yet taken into account" << std::endl;
   std::cout << "Warning : the DICOMDIR is overwritten"            << std::endl;
   std::cout << "        : to preserve file integrity "
             << " think unto using a copy .. "                     << std::endl;
 
   if( argc < 3 )
    {
    std::cerr << "Usage " << argv[0] << " DICOMDIR to anonymize  " 
              << std::endl;
    return 1;
    }

   std::string fileName       = argv[1];

// ============================================================
//   Read the input DICOMDIR
// ============================================================

   std::cout << argv[1] << std::endl;

   f1 = new gdcm::File( fileName );
   if (!f1->IsReadable()) {
       std::cerr << "Sorry, " << fileName <<"  not a gdcm-readable "
                 << "file" <<std::endl;
   }
   std::cout << " ... is readable " << std::endl;


   // Directory record sequence
   gdcm::DocEntry *e = f1->GetDocEntry(0x0004, 0x1220);
   if ( !e )
   {
      std::cout << "No Directory Record Sequence (0004,1220) found" <<std::endl;;
      return 0;         
   }
   
   gdcm::SeqEntry *s = dynamic_cast<gdcm::SeqEntry *>(e);
   if ( !s )
   {
      std::cout << "Element (0004,1220) is not a Sequence ?!?" <<std::endl;
      return 0;
   }

   // Open the file LTTG (aka ALAP)
   std::fstream *fp = new std::fstream(fileName.c_str(), 
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

      if( v != "PATIENT " )
      {
         continue;          // Work only on PATIENT
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

    // Aware use will add more Entries he wants to rubb out here

      oss << "";
      patientNumber++;
      tmpSI=s->GetNextSQItem();
   }

   // Close the file ASAP

   fp->close();
   delete fp;
    
   delete f1;
   return 0;
}

