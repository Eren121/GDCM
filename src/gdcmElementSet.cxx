/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmElementSet.cxx,v $
  Language:  C++
  Date:      $Date: 2004/09/03 14:04:02 $
  Version:   $Revision: 1.18 $
                                                                                
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
              : gdcmDocEntrySet(depthLevel)
{
}

/**
 * \ingroup gdcmElementSet
 * \brief   Canonical destructor.
 */
gdcmElementSet::~gdcmElementSet() 
{
  for(TagDocEntryHT::iterator cc = TagHT.begin();cc != TagHT.end(); ++cc)
   {
      gdcmDocEntry* entryToDelete = cc->second;
      if ( entryToDelete )
      {
         delete entryToDelete;
      }
   }
   TagHT.clear();
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
   for( TagDocEntryHT::const_iterator i = TagHT.begin(); i != TagHT.end(); ++i)
   {
      gdcmDocEntry* entry = i->second;
      entry->Print(os);   
      bool PrintEndLine = true;
      if ( gdcmSeqEntry* seqEntry = dynamic_cast<gdcmSeqEntry*>(entry) )
      {
         (void)seqEntry;  //not used
         PrintEndLine = false;
      }
      if( PrintEndLine )
      {
         os << std::endl;
      }
   }
}

/**
  * \brief   Writes the Header Entries (Dicom Elements)
  *          from the H Table
  * @return
  */ 
void gdcmElementSet::Write(FILE *fp, FileType filetype)
{
   for (TagDocEntryHT::const_iterator i = TagHT.begin(); i != TagHT.end(); ++i)
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
 * @param   newEntry entry to add
 */
bool gdcmElementSet::AddEntry( gdcmDocEntry *newEntry)
{
   gdcmTagKey key = newEntry->GetKey();

   if( TagHT.count(key) == 1 )
   {
      dbg.Verbose(1, "gdcmElementSet::AddEntry key already present: ",
                  key.c_str());
      return(false);
   } 
   else 
   {
      TagHT[newEntry->GetKey()] = newEntry;
      return true;
   }   
}

/**
 * \brief   Clear the hash table from given entry BUT keep the entry.
 * @param   entryToRemove Entry to remove.
 */
bool gdcmElementSet::RemoveEntryNoDestroy( gdcmDocEntry *entryToRemove)
{
   gdcmTagKey key = entryToRemove->GetKey();
   if( TagHT.count(key) == 1 )
   {
      TagHT.erase(key);
      dbg.Verbose(0, "gdcmElementSet::RemoveEntry: one element erased.");
      return true;
   }

   dbg.Verbose(0, "gdcmElementSet::RemoveEntry: key not present: ");
   return false ;
}

/**
 * \brief   Clear the hash table from given entry AND delete the entry.
 * @param   entryToRemove Entry to remove AND delete.
 */
bool gdcmElementSet::RemoveEntry( gdcmDocEntry *entryToRemove)
{
   gdcmTagKey key = entryToRemove->GetKey();
   if( TagHT.count(key) == 1 )
   {
      TagHT.erase(key);
      dbg.Verbose(0, "gdcmElementSet::RemoveEntry: one element erased.");
      delete entryToRemove;
      return true;
   }

   dbg.Verbose(0, "gdcmElementSet::RemoveEntry: key not present: ");
   return false ;
}
