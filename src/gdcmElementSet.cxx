/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmElementSet.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/24 16:10:52 $
  Version:   $Revision: 1.49 $
                                                                                
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
 * \brief   Constructor for a given ElementSet
 */
//BOZ depthLevel is not usefull anymore
ElementSet::ElementSet(int depthLevel) 
              : DocEntrySet()
{
  (void)depthLevel;
}

/**
 * \brief   Canonical destructor.
 */
ElementSet::~ElementSet() 
{
   for(TagDocEntryHT::iterator cc = TagHT.begin();cc != TagHT.end(); ++cc)
   {
      if ( cc->second )
      {
         delete cc->second;
      }
   }
   TagHT.clear();
}

//-----------------------------------------------------------------------------
// Print
/**
  * \brief   Prints the Header Entries (Dicom Elements) from the H Table
  * @param os ostream to write to  
  * @param indent Indentation string to be prepended during printing
  */ 
void ElementSet::Print(std::ostream &os, std::string const & )
{
   for( TagDocEntryHT::const_iterator i = TagHT.begin(); i != TagHT.end(); ++i)
   {
      DocEntry* entry = i->second;

      entry->SetPrintLevel(PrintLevel);
      entry->Print(os);   

      if ( dynamic_cast<SeqEntry*>(entry) )
      {
         // Avoid the newline for a sequence:
         continue;
      }
      os << std::endl;
   }
}

//-----------------------------------------------------------------------------
// Public
/**
  * \brief   Writes the Header Entries (Dicom Elements)
  *          from the H Table
  * @param fp ofstream to write to  
  * @param filetype filetype
  */ 
void ElementSet::WriteContent(std::ofstream *fp, FileType filetype)
{
   for (TagDocEntryHT::const_iterator i = TagHT.begin(); 
                                     i != TagHT.end(); 
                                    ++i)
   {
      i->second->WriteContent(fp, filetype);
   } 
}

/**
 * \brief  retrieves a Dicom Element using (group, element)
 * @param   group  Group number of the searched Dicom Element 
 * @param   elem Element number of the searched Dicom Element 
 * @return  
 */
DocEntry *ElementSet::GetDocEntry(uint16_t group, uint16_t elem) 
{
   TagKey key = DictEntry::TranslateToKey(group, elem);
   if ( !TagHT.count(key))
   {
      return NULL;
   }
   return TagHT.find(key)->second;
}

/**
 * \brief  Same as \ref Document::GetDocEntry except it only
 *         returns a result when the corresponding entry is of type
 *         ValEntry.
 * @param   group  Group number of the searched Dicom Element 
 * @param   elem Element number of the searched Dicom Element  
 * @return When present, the corresponding ValEntry. 
 */
ValEntry *ElementSet::GetValEntry(uint16_t group, uint16_t elem)
{
   DocEntry *currentEntry = GetDocEntry(group, elem);
   if ( !currentEntry )
   {
      return 0;
   }
   if ( ValEntry *entry = dynamic_cast<ValEntry*>(currentEntry) )
   {
      return entry;
   }
   gdcmVerboseMacro( "Unfound ValEntry.");

   return 0;
}

/**
 * \brief  Same as \ref Document::GetDocEntry except it only
 *         returns a result when the corresponding entry is of type
 *         BinEntry.
 * @param   group  Group number of the searched Dicom Element 
 * @param   elem Element number of the searched Dicom Element  
 * @return When present, the corresponding BinEntry. 
 */
BinEntry *ElementSet::GetBinEntry(uint16_t group, uint16_t elem)
{
   DocEntry *currentEntry = GetDocEntry(group, elem);
   if ( !currentEntry )
   {
      return 0;
   }
   if ( BinEntry *entry = dynamic_cast<BinEntry*>(currentEntry) )
   {
      return entry;
   }
   gdcmVerboseMacro( "Unfound BinEntry.");

   return 0;
}

/**
 * \brief  Same as \ref Document::GetDocEntry except it only
 *         returns a result when the corresponding entry is of type
 *         SeqEntry.
 * @param   group  Group number of the searched Dicom Element 
 * @param   elem Element number of the searched Dicom Element  
 * @return When present, the corresponding SeqEntry. 
 */
SeqEntry *ElementSet::GetSeqEntry(uint16_t group, uint16_t elem)
{
   DocEntry *currentEntry = GetDocEntry(group, elem);
   if ( !currentEntry )
   {
      return 0;
   }
   if ( SeqEntry *entry = dynamic_cast<SeqEntry*>(currentEntry) )
   {
      return entry;
   }
   gdcmVerboseMacro( "Unfound SeqEntry.");

   return 0;
}


//-----------------------------------------------------------------------------
// Protected

/**
 * \brief   Checks if a given Dicom Element exists within the H table
 * @param   group   Group number of the searched Dicom Element 
 * @param   elem  Element number of the searched Dicom Element 
 * @return true is found
 */
bool ElementSet::CheckIfEntryExist(uint16_t group, uint16_t elem )
{
   const std::string &key = DictEntry::TranslateToKey(group, elem );
   return TagHT.count(key) != 0;
}

/**
 * \brief   Get the (std::string representable) value of the Dicom entry
 * @param   group  Group number of the searched tag.
 * @param   elem Element number of the searched tag.
 * @return  Corresponding element value when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string ElementSet::GetEntryValue(uint16_t group, uint16_t elem)
{
   TagKey key = DictEntry::TranslateToKey(group, elem);
   if ( !TagHT.count(key))
   {
      return GDCM_UNFOUND;
   }

   return ((ValEntry *)TagHT.find(key)->second)->GetValue();
}


//-----------------------------------------------------------------------------
// Private

/**
 * \brief   add a new Dicom Element pointer to the H Table
 * @param   newEntry entry to add
 */
bool ElementSet::AddEntry(DocEntry *newEntry)
{
   const TagKey &key = newEntry->GetKey();

   if( TagHT.count(key) == 1 )
   {
      gdcmVerboseMacro( "Key already present: " << key.c_str());
      return false;
   }
   else
   {
      TagHT.insert(TagDocEntryHT::value_type(newEntry->GetKey(), newEntry));
      return true;
   }
}

/**
 * \brief   Clear the hash table from given entry AND delete the entry.
 * @param   entryToRemove Entry to remove AND delete.
 */
bool ElementSet::RemoveEntry( DocEntry *entryToRemove)
{
   const TagKey &key = entryToRemove->GetKey();
   if( TagHT.count(key) == 1 )
   {
      TagHT.erase(key);
      //gdcmVerboseMacro( "One element erased.");
      delete entryToRemove;
      return true;
   }

   gdcmVerboseMacro( "Key not present");
   return false ;
}

/**
 * \brief   Clear the hash table from given entry BUT keep the entry.
 * @param   entryToRemove Entry to remove.
 */
bool ElementSet::RemoveEntryNoDestroy(DocEntry *entryToRemove)
{
   const TagKey &key = entryToRemove->GetKey();
   if( TagHT.count(key) == 1 )
   {
      TagHT.erase(key);
      //gdcmVerboseMacro( "One element erased.");
      return true;
   }

   gdcmVerboseMacro( "Key not present");
   return false ;
}

/**
 * \brief   Get the first entry while visiting the DocEntrySet
 * \return  The first DocEntry if found, otherwhise NULL
 */
DocEntry *ElementSet::GetFirstEntry()
{
   ItTagHT = TagHT.begin();
   if (ItTagHT != TagHT.end())
      return  ItTagHT->second;
   return NULL;
}

/**
 * \brief   Get the next entry while visiting the Hash table (TagHT)
 * \note : meaningfull only if GetFirstEntry already called 
 * \return  The next DocEntry if found, otherwhise NULL
 */
DocEntry *ElementSet::GetNextEntry()
{
   gdcmAssertMacro (ItTagHT != TagHT.end());

   ++ItTagHT;
   if (ItTagHT != TagHT.end())
      return  ItTagHT->second;
   return NULL;
}


/**
 * \brief   Get the larst entry while visiting the DocEntrySet
 * \return  The last DocEntry if found, otherwhise NULL
 */
DocEntry *ElementSet::GetLastEntry()
{
   ItTagHT = TagHT.end();
   if ( ItTagHT != TagHT.begin() )
      return  ItTagHT->second;
   return NULL;
}

/**
 * \brief   Get the previous entry while visiting the Hash table (TagHT)
 * \note : meaningfull only if GetFirstEntry already called 
 * \return  The previous DocEntry if found, otherwhise NULL
 */
DocEntry *ElementSet::GetPreviousEntry()
{
   gdcmAssertMacro (ItTagHT != TagHT.begin());

   --ItTagHT;
   if (ItTagHT != TagHT.begin())
      return  ItTagHT->second;
   return NULL;
}


//-----------------------------------------------------------------------------
} // end namespace gdcm
