/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmElementSet.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/22 14:37:04 $
  Version:   $Revision: 1.11 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmElementSet.h"
#include "gdcmDebug.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"
#include "gdcmSeqEntry.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmElementSet
 * \brief   Constructor from a given gdcmElementSet
 */
gdcmElementSet::gdcmElementSet(int depthLevel) 
              : gdcmDocEntrySet(depthLevel) {
}

/**
 * \ingroup gdcmElementSet
 * \brief   Canonical destructor.
 */
gdcmElementSet::~gdcmElementSet() 
{
  gdcmDocEntry* EntryToDelete;  
  for(TagDocEntryHT::iterator cc = tagHT.begin();cc != tagHT.end();++cc)
   {
      EntryToDelete = cc->second;
      if ( EntryToDelete )
         delete EntryToDelete;
   }
   tagHT.clear();
}


//-----------------------------------------------------------------------------
// Public


//-----------------------------------------------------------------------------
// Print
/**
  * \brief   Prints the Header Entries (Dicom Elements)
  *          from the H Table
  * @return
  */ 
void gdcmElementSet::Print(std::ostream & os) {
   for (TagDocEntryHT::iterator i = tagHT.begin(); i != tagHT.end(); ++i)  
   {
      //(i)->second->SetPrintLevel(printLevel);
      (i->second)->Print(os);   
   } 
}

/**
  * \brief   Writes the Header Entries (Dicom Elements)
  *          from the H Table
  * @return
  */ 
void gdcmElementSet::Write(FILE *fp, FileType filetype) {

// Troubles expected : BinEntries ARE ValEntries :-(
// BinEntry is checked first, then ValEntry;

   gdcmDocEntry *e;
   for (TagDocEntryHT::iterator i = tagHT.begin(); i != tagHT.end(); ++i)  
   {
      e=i->second;
      e->WriteCommonPart(fp, filetype);
      std::cout<<e->GetKey() << " " << std::hex << e->GetVR() << " " 
               << e->GetName()
               << std::endl;

// e->Write(fp,filetype); // This will be the right way to proceed !

      if (gdcmBinEntry* BinEntry = dynamic_cast< gdcmBinEntry* >(e) ) {
         BinEntry->Write(fp,filetype);
         continue;
      }
     if (gdcmValEntry* ValEntry = dynamic_cast< gdcmValEntry* >(e) ) {
         ValEntry->Write(fp);
         continue;
      }

      if (gdcmSeqEntry* SeqEntry = dynamic_cast< gdcmSeqEntry* >(e) ) {
         SeqEntry->Write(fp,filetype);
         continue;
      } 
   } 
}
//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private


/**
 * \brief   add a new Dicom Element pointer to the H Table
 * @param   NewEntry entry to add
 */
bool gdcmElementSet::AddEntry( gdcmDocEntry *NewEntry) {
   TagKey key;
   key = NewEntry->GetKey();

   if(tagHT.count(key) == 1)
   {
      dbg.Verbose(1, "gdcmElementSet::AddEntry key already present: ",
                  key.c_str());
      return(false);
   } 
   else 
   {
      tagHT[NewEntry->GetKey()] = NewEntry;
      return(true);
   }   
}

/**
 * \brief   Clear the hash table from given entry.
 * @param   EntryToRemove Entry to remove.
 */
bool gdcmElementSet::RemoveEntry( gdcmDocEntry *EntryToRemove)
{
   TagKey key = EntryToRemove->GetKey();
   if(tagHT.count(key) == 1)
   {
      tagHT.erase(key);
      dbg.Verbose(0, "gdcmElementSet::RemoveEntry: one element erased.");
      return true;
   }

   dbg.Verbose(0, "gdcmElementSet::RemoveEntry: key not present: ");
   return(false);
}
