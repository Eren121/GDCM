/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDict.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/23 10:12:33 $
  Version:   $Revision: 1.69 $
                                                                                
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
void FillDefaultDataDict(Dict *d);
//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor
 */
Dict::Dict( )
{
   Filename="";
}

/**
 * \brief   Constructor
 * @param   filename from which to build the dictionary.
 */
Dict::Dict(std::string const &filename)
{
   uint16_t group;
   uint16_t element;
   TagName vr;
   TagName vm;
   TagName name;

   std::ifstream from( filename.c_str() );
   if( !from )
   {
      gdcmVerboseMacro( "Can't open dictionary" << filename.c_str());
      // Using default embeded one:
      FillDefaultDataDict( this );
   }
   else
   {
      while (!from.eof())
      {
         from >> std::hex;
         from >> group;
         from >> element;
         from >> vr;
         from >> vm;
         from >> std::ws;  //remove white space
         std::getline(from, name);
   
         const DictEntry newEntry(group, element, vr, vm, name);
         AddNewEntry(newEntry);
      }

      Filename = filename;
      from.close();
   }
}

/**
 * \brief  Destructor 
 */
Dict::~Dict()
{
   ClearEntry();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by tag i.e. the couple (group, element).
 * @param   os The output stream to be written to.
 * @param indent Indentation string to be prepended during printing
 */
void Dict::Print(std::ostream &os, std::string const & )
{
   os << "Dict file name : " << Filename << std::endl;
   std::ostringstream s;

   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag)
   {
      s << "Entry : ";
      s << "(" << std::hex << std::setw(4) << tag->second.GetGroup() << ',';
      s << std::hex << std::setw(4) << tag->second.GetElement() << ") = "
        << std::dec;
      s << tag->second.GetVR() << ", ";
      s << tag->second.GetVM() << ", ";
      s << tag->second.GetName() << "."  << std::endl;
   }
   os << s.str();
}


//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Remove all Dicom Dictionary Entries
 */
void Dict::ClearEntry()
{
   // we assume all the pointed DictEntries are already cleaned-up
   // when we clean KeyHt.
   KeyHt.clear();
}

/**
 * \brief  adds a new Dicom Dictionary Entry 
 * @param   newEntry entry to add 
 * @return  false if Dicom Element already exists
 */
bool Dict::AddNewEntry(DictEntry const &newEntry) 
{
   const TagKey &key = newEntry.GetKey();

   if(KeyHt.count(key) == 1)
   {
      gdcmVerboseMacro( "Already present" << key.c_str());
      return false;
   } 
   else 
   {
      KeyHt.insert( TagKeyHT::value_type(newEntry.GetKey(), newEntry));
      return true;
   }
}

/**
 * \brief  replaces an already existing Dicom Element by a new one
 * @param   newEntry new entry (overwrites any previous one with same tag)
 * @return  false if Dicom Element doesn't exist
 */
bool Dict::ReplaceEntry(DictEntry const &newEntry)
{
   if ( RemoveEntry(newEntry.GetKey()) )
   {
       KeyHt.insert( TagKeyHT::value_type(newEntry.GetKey(), newEntry));
       return true;
   } 
   return false;
}

/**
 * \brief  removes an already existing Dicom Dictionary Entry,
 *         identified by its Tag
 * @param   key (group|element)
 * @return  false if Dicom Dictionary Entry doesn't exist
 */
bool Dict::RemoveEntry (TagKey const &key) 
{
   TagKeyHT::const_iterator it = KeyHt.find(key);
   if(it != KeyHt.end()) 
   {
      KeyHt.erase(key);

      return true;
   } 
   else 
   {
      gdcmVerboseMacro( "Unfound entry" << key.c_str());
      return false;
  }
}

/**
 * \brief  removes an already existing Dicom Dictionary Entry, 
 *          identified by its group,element number
 * @param   group   Dicom group number of the Dicom Element
 * @param   elem Dicom element number of the Dicom Element
 * @return  false if Dicom Dictionary Entry doesn't exist
 */
bool Dict::RemoveEntry (uint16_t group, uint16_t elem)
{
   return RemoveEntry(DictEntry::TranslateToKey(group, elem));
}

/**
 * \brief   Get the dictionary entry identified by a given tag (group,element)
 * @param   group   group of the entry to be found
 * @param   elem element of the entry to be found
 * @return  the corresponding dictionary entry when existing, NULL otherwise
 */
DictEntry *Dict::GetDictEntry(uint16_t group, uint16_t elem)
{
   TagKey key = DictEntry::TranslateToKey(group, elem);
   TagKeyHT::iterator it = KeyHt.find(key);
   if ( it == KeyHt.end() )
   {
      return 0;
   }
   return &(it->second);
}

/** 
 * \brief   Consider all the entries of the public dicom dictionary. 
 *          Build all list of all the tag names of all those entries.
 * \sa      DictSet::GetPubDictTagNamesByCategory
 * @return  A list of all entries of the public dicom dictionary.
 */

 
 // Probabely useless
  
 
//EntryNamesList *Dict::GetDictEntryNames() 
//{
//   EntryNamesList *result = new EntryNamesList;
//   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag)
//   {
//      result->push_back( tag->second.GetName() );
//   }
//   return result;
//}

/** 
 * \brief   Consider all the entries of the public dicom dictionary.
 *          Build an hashtable whose keys are the names of the groups
 *          (fourth field in each line of dictionary) and whose corresponding
 *          values are lists of all the dictionary entries among that
 *          group. Note that apparently the Dicom standard doesn't explicitely
 *          define a name (as a string) for each group.
 *          A typical usage of this method would be to enable a dynamic
 *          configuration of a Dicom file browser: the admin/user can
 *          select in the interface which Dicom tags should be displayed.
 * \warning Dicom *doesn't* define any name for any 'categorie'
 *          (the dictionary fourth field was formerly NIH defined
 *           - and no longer he is-
 *           and will be removed when Dicom provides us a text file
 *           with the 'official' Dictionary, that would be more friendly
 *           than asking us to perform a line by line check of the dictionary
 *           at the beginning of each year to -try to- guess the changes)
 *           Therefore : please NEVER use that fourth field :-(
 *
 * @return  An hashtable: whose keys are the names of the groups and whose
 *          corresponding values are lists of all the dictionary entries
 *          among that group.
 */
 
 // Probabely useless
 
//EntryNamesByCatMap *Dict::GetDictEntryNamesByCategory() 
//{
//   EntryNamesByCatMap *result = new EntryNamesByCatMap;
//
//   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag)
//   {
//      (*result)[tag->second.GetFourth()].push_back(tag->second.GetName());
//   }
//
//   return result;
//}

/**
 * \brief   Get the first entry while visiting the Dict entries
 * \return  The first DicEntry if found, otherwhise NULL
 */
DictEntry *Dict::GetFirstEntry()
{
   ItKeyHt = KeyHt.begin();
   if( ItKeyHt != KeyHt.end() )
      return &(ItKeyHt->second);
   return NULL;
}

/**
 * \brief   Get the next entry while visiting the Hash table (KeyHt)
 * \note : meaningfull only if GetFirstEntry already called
 * \return  The next DictEntry if found, otherwhise NULL
 */
DictEntry *Dict::GetNextEntry()
{
   gdcmAssertMacro (ItKeyHt != KeyHt.end());

   {
      ++ItKeyHt;
      if (ItKeyHt != KeyHt.end())
         return &(ItKeyHt->second);
   }
   return NULL;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
