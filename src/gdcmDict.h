/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDict.h,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:06 $
  Version:   $Revision: 1.18 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICT_H
#define GDCMDICT_H

#include "gdcmCommon.h"
#include "gdcmDictEntry.h"

#include <iostream>
#include <list>
#include <map>

//-----------------------------------------------------------------------------
typedef std::map<gdcmTagKey, gdcmDictEntry*> TagKeyHT;
typedef std::map<TagName, gdcmDictEntry*> TagNameHT;

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmDict
 * \brief    gdcmDict acts a memory representation of a dicom dictionary i.e.
 *           it is a container for a collection of dictionary entries. The
 *           dictionary is loaded from in an ascii file.
 *           There should be a single public dictionary (THE dictionary of
 *           the actual DICOM v3) but as many shadow dictionaries as imagers 
 *           combined with all software versions...
 * \see gdcmDictSet
 */
class GDCM_EXPORT gdcmDict
{
public:
   gdcmDict(std::string const & FileName);
   ~gdcmDict();

// Print
   void Print(std::ostream &os = std::cout);
   void PrintByKey(std::ostream &os = std::cout);
   void PrintByName(std::ostream &os = std::cout);

// Entries
   bool AddNewEntry (gdcmDictEntry *NewEntry);
   bool ReplaceEntry(gdcmDictEntry *NewEntry);
   bool RemoveEntry (gdcmTagKey key);
   bool RemoveEntry (uint16_t group, uint16_t element);
   
// Tag
   gdcmDictEntry *GetDictEntryByName(TagName name);
   gdcmDictEntry *GetDictEntryByNumber(uint16_t group, uint16_t element);

   std::list<std::string> *GetDictEntryNames();
   std::map<std::string, std::list<std::string> > *
        GetDictEntryNamesByCategory();

   /// \brief  Returns a ref to the Dicom Dictionary H table (map)
   /// @return the Dicom Dictionary H table
   TagKeyHT & GetEntriesByKey()  { return KeyHt; }

   /// \brief  Returns a ref to the Dicom Dictionary H table (map)
   /// @return the Dicom Dictionary H table
   TagNameHT & GetEntriesByName()  { return NameHt; }
 
private:
   /// ASCII file holding the Dictionnary
   std::string Filename;
   /// Access through gdcmTagKey (see alternate access with NameHt)
   TagKeyHT  KeyHt;
   /// Access through TagName (see alternate access with KeyHt)
   TagNameHT NameHt;
};

//-----------------------------------------------------------------------------
#endif
