/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntryArchive.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/26 10:29:17 $
  Version:   $Revision: 1.9 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDocEntryArchive.h"
#include "gdcmDebug.h"
#include "gdcmDocEntry.h"

#include <string>

namespace gdcm 
{
//-----------------------------------------------------------------------------
/**
 * \brief Constructor
 */
DocEntryArchive::DocEntryArchive(File *header):
   HeaderHT(header->TagHT)
{
}

//-----------------------------------------------------------------------------
/**
 * \brief Destructor
 */
DocEntryArchive::~DocEntryArchive()
{
   ClearArchive();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Print all 
 * @param   os The output stream to be written to.
 */
void DocEntryArchive::Print(std::ostream &os) 
{
   os << "Elements in archives :" << std::endl;
   for(TagDocEntryHT::iterator it = Archive.begin();
       it!=Archive.end();
       ++it)
   {
      if(it->second)
         it->second->Print(os);
   }
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Replace in the Header a DocEntry by the new DocEntry. The last
 *          DocEntry is kept in archive
 * @param   newEntry New entry to substitute to an other entry of the Header
 * @return  FALSE when an other DocEntry is already archived with the same
 *          generalized key, TRUE otherwise
 */
bool DocEntryArchive::Push(DocEntry *newEntry)
{
   if(!newEntry)
      return false;

   uint16_t gr = newEntry->GetDictEntry()->GetGroup();
   uint16_t elt = newEntry->GetDictEntry()->GetElement();
   std::string key = DictEntry::TranslateToKey(gr,elt);

   if( Archive.find(key)==Archive.end() )
   {
      // Save the old DocEntry if any
      TagDocEntryHT::iterator it = HeaderHT.find(key);
      if( it!=HeaderHT.end() )
      {
         Archive[key] = it->second;
      }
      else
      {
         Archive[key] = NULL;
      }

      // Set the new DocEntry
      HeaderHT[key] = newEntry;

      return true;
   }
   return false;
}

/**
 * \brief   Replace in the Header a DocEntry by the new DocEntry. The last
 *          DocEntry is kept in archive
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * @return  FALSE when an other DocEntry is already archived with the same
 *          generalized key, TRUE otherwise
 */
bool DocEntryArchive::Push(uint16_t group,uint16_t elem)
{
   std::string key = DictEntry::TranslateToKey(group,elem);

   if( Archive.find(key)==Archive.end() )
   {
      // Save the old DocEntry if any
      TagDocEntryHT::iterator it = HeaderHT.find(key);
      if( it!=HeaderHT.end() )
      {
         Archive[key] = it->second;
         HeaderHT.erase(it);
      }

      return true;
   }
   return false;
}

/**
 * \brief   Restore in the Header the DocEntry that have the generalized key. 
 *          The old entry is destroyed.
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * @return  FALSE when the generalized key isn't in the archive, 
 *          TRUE otherwise
 */
bool DocEntryArchive::Restore(uint16_t group,uint16_t elem)
{
   std::string key=DictEntry::TranslateToKey(group,elem);

   TagDocEntryHT::iterator restoreIt=Archive.find(key);
   if( restoreIt!=Archive.end() )
   {
      TagDocEntryHT::iterator restorePos = HeaderHT.find(key);
      if( restorePos!=HeaderHT.end() )
      {
         delete restorePos->second;
      }

      if( Archive[key] )
      {
         HeaderHT[key] = Archive[key];
      }
      else
      {
         HeaderHT.erase(restorePos);
      }

      Archive.erase(restoreIt);

      return true;
   }
   return false;
}

/**
 * \brief   Remove all DocEntry that are in the archive.  
 *          The entries aren't restored but only destroyed.
 */
void DocEntryArchive::ClearArchive( )
{
   for(TagDocEntryHT::iterator it = Archive.begin();
       it!=Archive.end();
       ++it)
   {
      delete it->second;
   }
   Archive.clear();
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
