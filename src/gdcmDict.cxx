/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDict.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/18 02:31:58 $
  Version:   $Revision: 1.47 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDict.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"

#include <fstream>
#include <iostream>
#include <iomanip>

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Construtor
 * @param   filename from which to build the dictionary.
 */
Dict::Dict(std::string const & filename)
{
   uint16_t group;
   uint16_t element;
   TagName vr;
   TagName fourth;
   TagName name;

   std::ifstream from( filename.c_str() );
   dbg.Error(!from, "Dict::Dict: can't open dictionary",
                    filename.c_str());

   while (!from.eof())
   {
      from >> std::hex;
      from >> group;
      from >> element;
      from >> vr;
      from >> fourth;
      from >> std::ws;  //remove white space
      std::getline(from, name);

      DictEntry * newEntry = new DictEntry(group, element, vr, fourth, name);
      AddNewEntry(newEntry);
   }
   from.close();

   Filename = filename;
}

/**
 * \brief  Destructor 
 */
Dict::~Dict()
{
   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag)
   {
      DictEntry* entryToDelete = tag->second;
      if ( entryToDelete )
      {
         delete entryToDelete;
      }
   }
   // Since AddNewEntry adds symetrical in both KeyHt and NameHT we can
   // assume all the pointed DictEntries are already cleaned-up when
   // we cleaned KeyHt.
   KeyHt.clear();
   NameHt.clear();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by tag i.e. the couple (group, element).
 * @param   os The output stream to be written to.
 */
void Dict::Print(std::ostream &os)
{
   os << "Dict file name : " << Filename << std::endl;
   PrintByKey(os);
}

/**
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by tag i.e. the couple (group, element).
 * @param   os The output stream to be written to.
 */
void Dict::PrintByKey(std::ostream &os)
{
   std::ostringstream s;

   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag)
   {
      s << "Entry : ";
      s << "(" << std::hex << std::setw(4) << tag->second->GetGroup() << ',';
      s << std::hex << std::setw(4) << tag->second->GetElement() << ") = "
        << std::dec;
      s << tag->second->GetVR() << ", ";
      s << tag->second->GetFourth() << ", ";
      s << tag->second->GetName() << "."  << std::endl;
   }
   os << s.str();
}

/**
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by the name of the dictionary entries.
 * \warning AVOID USING IT : the name IS NOT an identifier; 
 *                           unpredictable result
 * @param   os The output stream to be written to.
 */
void Dict::PrintByName(std::ostream& os)
{
   std::ostringstream s;

   for (TagNameHT::iterator tag = NameHt.begin(); tag != NameHt.end(); ++tag)
   {
      s << "Entry : ";
      s << tag->second->GetName() << ",";
      s << tag->second->GetVR() << ", ";
      s << tag->second->GetFourth() << ", ";
      s << "(" << std::hex << std::setw(4) << tag->second->GetGroup() << ',';
      s << std::hex << std::setw(4) << tag->second->GetElement() << ") = ";
      s << std::dec << std::endl;
   }
   os << s.str();
}

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup Dict
 * \brief  adds a new Dicom Dictionary Entry 
 * @param   newEntry entry to add 
 * @return  false if Dicom Element already exists
 */
bool Dict::AddNewEntry(DictEntry *newEntry) 
{
   TagKey key = newEntry->GetKey();

   if(KeyHt.count(key) == 1)
   {
      dbg.Verbose(1, "Dict::AddNewEntry already present", key.c_str());
      return false;
   } 
   else 
   {
      KeyHt[newEntry->GetKey()] = newEntry;
      NameHt[newEntry->GetName()] = newEntry;
      return true;
   }
}

/**
 * \ingroup Dict
 * \brief  replaces an already existing Dicom Element by a new one
 * @param   newEntry new entry (overwrites any previous one with same tag)
 * @return  false if Dicom Element doesn't exist
 */
bool Dict::ReplaceEntry(DictEntry *newEntry)
{
   if ( RemoveEntry(newEntry->DictEntry::GetKey()) )
   {
       KeyHt[newEntry->GetKey()] = newEntry;
       NameHt[newEntry->GetName()] = newEntry;
       return true;
   } 
   return false;
}

/**
 * \ingroup Dict
 * \brief  removes an already existing Dicom Dictionary Entry,
 *         identified by its Tag
 * @param   key (group|element)
 * @return  false if Dicom Dictionary Entry doesn't exist
 */
bool Dict::RemoveEntry(TagKey const & key) 
{
   TagKeyHT::const_iterator it = KeyHt.find(key);
   if(it != KeyHt.end()) 
   {
      DictEntry* entryToDelete = it->second;

      if ( entryToDelete )
      {
         NameHt.erase(entryToDelete->GetName());
         delete entryToDelete;
      }

      KeyHt.erase(key);
      return true;
   } 
   else 
   {
      dbg.Verbose(1, "Dict::RemoveEntry unfound entry", key.c_str());
      return false;
  }
}

/**
 * \brief  removes an already existing Dicom Dictionary Entry, 
 *          identified by its group,element number
 * @param   group   Dicom group number of the Dicom Element
 * @param   element Dicom element number of the Dicom Element
 * @return  false if Dicom Dictionary Entry doesn't exist
 */
bool Dict::RemoveEntry (uint16_t group, uint16_t element)
{
   return RemoveEntry(DictEntry::TranslateToKey(group, element));
}

/**
 * \brief   Get the dictionnary entry identified by it's name.
 * @param   name element of the ElVal to modify
 * \warning : NEVER use it !
 *            the 'name' IS NOT an identifier within the Dicom Dicom Dictionary
 *            the name MAY CHANGE between two versions !
 * @return  the corresponding dictionnary entry when existing, NULL otherwise
 */
DictEntry* Dict::GetDictEntryByName(TagName const & name)
{
   TagNameHT::const_iterator it = NameHt.find(name);
   if ( it == NameHt.end() )
   {
      return 0;
   }
   return it->second;
}

/**
 * \brief   Get the dictionnary entry identified by a given tag (group,element)
 * @param   group   group of the entry to be found
 * @param   element element of the entry to be found
 * @return  the corresponding dictionnary entry when existing, NULL otherwise
 */
DictEntry* Dict::GetDictEntryByNumber(uint16_t group, uint16_t element)
{
   TagKey key = DictEntry::TranslateToKey(group, element);
   TagKeyHT::const_iterator it = KeyHt.find(key);
   if ( it == KeyHt.end() )
   {
      return 0;
   }
   return it->second;
}

/** 
 * \brief   Consider all the entries of the public dicom dictionnary. 
 *          Build all list of all the tag names of all those entries.
 * \sa      DictSet::GetPubDictTagNamesByCategory
 * @return  A list of all entries of the public dicom dictionnary.
 */
EntryNamesList* Dict::GetDictEntryNames() 
{
   EntryNamesList *result = new EntryNamesList;
   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag)
   {
      result->push_back( tag->second->GetName() );
   }
   return result;
}

/** 
 * \ingroup Dict
 * \brief   Consider all the entries of the public dicom dictionnary.
 *          Build an hashtable whose keys are the names of the groups
 *          (fourth field in each line of dictionary) and whose corresponding
 *          values are lists of all the dictionnary entries among that
 *          group. Note that apparently the Dicom standard doesn't explicitely
 *          define a name (as a string) for each group.
 *          A typical usage of this method would be to enable a dynamic
 *          configuration of a Dicom file browser: the admin/user can
 *          select in the interface which Dicom tags should be displayed.
 * \warning Dicom *doesn't* define any name for any 'categorie'
 *          (the dictionnary fourth field was formerly NIH defined
 *           - and no longer he is-
 *           and will be removed when Dicom provides us a text file
 *           with the 'official' Dictionnary, that would be more friendly
 *           than asking us to perform a line by line check of the dictionnary
 *           at the beginning of each year to -try to- guess the changes)
 *           Therefore : please NEVER use that fourth field :-(
 *
 * @return  An hashtable: whose keys are the names of the groups and whose
 *          corresponding values are lists of all the dictionnary entries
 *          among that group.
 */
EntryNamesByCatMap *Dict::GetDictEntryNamesByCategory() 
{
   EntryNamesByCatMap *result = new EntryNamesByCatMap;

   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag)
   {
      (*result)[tag->second->GetFourth()].push_back(tag->second->GetName());
   }

   return result;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
