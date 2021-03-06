/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDict.h,v $
  Language:  C++
  Date:      $Date: 2007/08/22 16:14:03 $
  Version:   $Revision: 1.51 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef _GDCMDICT_H_
#define _GDCMDICT_H_

#include "gdcmRefCounter.h"
#include "gdcmDictEntry.h"

#include <iostream>
#include <fstream> // for ifstream
#include <list>
#include <map>

namespace GDCM_NAME_SPACE 
{

//-----------------------------------------------------------------------------
typedef std::string DictKey;
typedef std::map<TagKey, DictEntry *>  TagKeyHT;

//-----------------------------------------------------------------------------
/**
 * \brief    Dict acts a memory representation of a dicom dictionary i.e.
 *           it is a container for a collection of dictionary entries.
 *           The dictionary is loaded from in an ascii file.
 *           There should be a single public dictionary (THE dictionary of
 *           the actual DICOM v3) but as many shadow dictionaries as imagers 
 *           combined with all software versions...
 * \see DictSet
 */
class GDCM_EXPORT Dict : public RefCounter
{
   gdcmTypeMacro(Dict);

public:
/// \brief Contructs an empty Dict with a RefCounter
   static Dict *New() {return new Dict();}
/// \brief Contructs a Dict with a RefCounter
   static Dict *New(std::string const &filename) {return new Dict(filename);}

   bool AddDict(std::string const &filename);
   bool RemoveDict(std::string const &filename);
// Print
   void Print(std::ostream &os = std::cout, std::string const &indent = "");

// Entries
   bool AddEntry(DictEntry *newEntry);
  // bool ReplaceEntry(DictEntry *newEntry); // useless ?
   bool RemoveEntry (TagKey const &key);
   bool RemoveEntry (uint16_t group, uint16_t elem);
   void ClearEntry();
   
// Tag
   DictEntry *GetEntry(uint16_t group, uint16_t elem);
   DictEntry *GetEntry(TagKey const &key);

   DictEntry *GetFirstEntry();
   DictEntry *GetNextEntry();

protected:
   Dict();
   Dict(std::string const &filename);
   ~Dict();

private:
   void DoTheLoadingJob(std::ifstream &ifs);

   /// ASCII file holding the Dictionnary
   std::string Filename;

   /// Access through TagKey
   TagKeyHT KeyHt;
   /// Iterator for the entries
   TagKeyHT::iterator ItKeyHt;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
