/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmElementSet.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/23 13:02:36 $
  Version:   $Revision: 1.13 $
                                                                                
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
void gdcmElementSet::Print(std::ostream & os)
{
   gdcmDocEntry* Entry;
   for (TagDocEntryHT::iterator i = tagHT.begin(); i != tagHT.end(); ++i)  
   {
      Entry = i->second;
      Entry->Print(os);   
      bool PrintEndLine = true;
      if ( gdcmSeqEntry* SeqEntry = dynamic_cast<gdcmSeqEntry*>(Entry) )
         PrintEndLine = false;
      if (PrintEndLine)
         os << std::endl;
   } 
}

/**
  * \brief   Writes the Header Entries (Dicom Elements)
  *          from the H Table
  * @return
  */ 
void gdcmElementSet::Write(FILE *fp, FileType filetype)
{

   for (TagDocEntryHT::iterator i = tagHT.begin(); i != tagHT.end(); ++i)  
   {
      i->second->Write(fp, filetype);
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
