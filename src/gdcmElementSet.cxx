/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmElementSet.cxx,v $
  Language:  C++
  Date:      $Date: 2004/11/16 13:20:34 $
  Version:   $Revision: 1.29 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmElementSet.h"
#include "gdcmDebug.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"
#include "gdcmSeqEntry.h"

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup ElementSet
 * \brief   Constructor from a given ElementSet
 */
//BOZ depthLevel is not usefull anymore
ElementSet::ElementSet(int depthLevel) 
              : DocEntrySet()
{
  (void)depthLevel;
}

/**
 * \ingroup ElementSet
 * \brief   Canonical destructor.
 */
ElementSet::~ElementSet() 
{
  for(TagDocEntryHT::iterator cc = TagHT.begin();cc != TagHT.end(); ++cc)
   {
      DocEntry* entryToDelete = cc->second;
      if ( entryToDelete )
      {
         // FIXME
         // Because the gdcmFile links the datas of gdcmPixelConvert with the
         // data in a binArea, these datas are deleted 2 times... very bad...
         //delete entryToDelete;
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
void ElementSet::Print(std::ostream& os)
{
   for( TagDocEntryHT::const_iterator i = TagHT.begin(); i != TagHT.end(); ++i)
   {
      DocEntry* entry = i->second;
      entry->Print(os);   
      if ( SeqEntry* seqEntry = dynamic_cast<SeqEntry*>(entry) )
      {
         (void)seqEntry;
         // Avoid the newline for a sequence:
         continue;
      }
      os << std::endl;
   }
}

/**
  * \brief   Writes the Header Entries (Dicom Elements)
  *          from the H Table
  * @return
  */ 
void ElementSet::Write(std::ofstream* fp, FileType filetype)
{
   for (TagDocEntryHT::const_iterator i = TagHT.begin(); 
                                     i != TagHT.end(); 
                                    ++i)
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
bool ElementSet::AddEntry(DocEntry* newEntry)
{
   const TagKey& key = newEntry->GetKey();

   if( TagHT.count(key) == 1 )
   {
      dbg.Verbose(1, "ElementSet::AddEntry key already present: ",
                  key.c_str());
      return false;
   }
   else
   {
      TagHT.insert(TagDocEntryHT::value_type(newEntry->GetKey(), newEntry));
      return true;
   }
}

/**
 * \brief   Clear the hash table from given entry BUT keep the entry.
 * @param   entryToRemove Entry to remove.
 */
bool ElementSet::RemoveEntryNoDestroy(DocEntry* entryToRemove)
{
   const TagKey& key = entryToRemove->GetKey();
   if( TagHT.count(key) == 1 )
   {
      TagHT.erase(key);
      dbg.Verbose(0, "ElementSet::RemoveEntry: one element erased.");
      return true;
   }

   dbg.Verbose(0, "ElementSet::RemoveEntry: key not present: ");
   return false ;
}

/**
 * \brief   Clear the hash table from given entry AND delete the entry.
 * @param   entryToRemove Entry to remove AND delete.
 */
bool ElementSet::RemoveEntry( DocEntry* entryToRemove)
{
   const TagKey& key = entryToRemove->GetKey();
   if( TagHT.count(key) == 1 )
   {
      TagHT.erase(key);
      dbg.Verbose(0, "ElementSet::RemoveEntry: one element erased.");
      delete entryToRemove;
      return true;
   }

   dbg.Verbose(0, "ElementSet::RemoveEntry: key not present: ");
   return false ;
}
} // end namespace gdcm
