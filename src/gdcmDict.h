/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDict.h,v $
  Language:  C++
  Date:      $Date: 2005/01/20 11:39:49 $
  Version:   $Revision: 1.34 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICT_H
#define GDCMDICT_H

#include "gdcmBase.h"
#include "gdcmDictEntry.h"

#include <iostream>
#include <list>
#include <map>

namespace gdcm 
{

//-----------------------------------------------------------------------------
typedef std::string DictKey;
typedef std::map<TagKey, DictEntry>  TagKeyHT;
typedef std::list<std::string>       EntryNamesList;
typedef std::map<std::string, 
            std::list<std::string> > EntryNamesByCatMap;
//-----------------------------------------------------------------------------
/**
 * \ingroup Dict
 * \brief    Dict acts a memory representation of a dicom dictionary i.e.
 *           it is a container for a collection of dictionary entries.
 *           The dictionary is loaded from in an ascii file.
 *           There should be a single public dictionary (THE dictionary of
 *           the actual DICOM v3) but as many shadow dictionaries as imagers 
 *           combined with all software versions...
 * \see DictSet
 */
class GDCM_EXPORT Dict : public Base
{
public:
   Dict(void);
   Dict(std::string const & filename);
   ~Dict();

// Print
   void Print(std::ostream &os = std::cout, std::string const & indent = "");

// Entries
   void ClearEntry  ();
   bool AddNewEntry (DictEntry const &newEntry);
   bool ReplaceEntry(DictEntry const &newEntry);
   bool RemoveEntry (TagKey const &key);
   bool RemoveEntry (uint16_t group, uint16_t elem);
   
// Tag
   DictEntry *GetDictEntry(uint16_t group, uint16_t elem);

  // EntryNamesList *GetDictEntryNames();
  // EntryNamesByCatMap *GetDictEntryNamesByCategory();

   DictEntry *GetFirstEntry();
   DictEntry *GetNextEntry();

private:
   /// ASCII file holding the Dictionnary
   std::string Filename;

   /// Access through TagKey
   TagKeyHT KeyHt;
   TagKeyHT::iterator ItKeyHt;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
