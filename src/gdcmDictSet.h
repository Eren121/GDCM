/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDictSet.h,v $
  Language:  C++
  Date:      $Date: 2005/01/13 16:35:37 $
  Version:   $Revision: 1.35 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICTSET_H
#define GDCMDICTSET_H

#include "gdcmBase.h"
#include "gdcmDict.h"

#include <map>
#include <list>

namespace gdcm 
{
typedef std::map<DictKey, Dict*> DictSetHT;

//-----------------------------------------------------------------------------
/**
 * \ingroup DictSet
 * \brief  Container for managing a set of loaded dictionaries (Dict).
 * \note   Hopefully, sharing dictionaries should avoid
 * \par    reloading an already loaded dictionary (saving time)
 * \par    having many in memory representations of the same dictionary
 *        (saving memory).
 */
class GDCM_EXPORT DictSet : public Base
{
public:
   DictSet();
   ~DictSet();

   void Print(std::ostream &os = std::cout);

   // Probabely useless !
   //EntryNamesList *GetPubDictEntryNames();
   //EntryNamesByCatMap* GetPubDictEntryNamesByCategory();

   Dict *LoadDictFromFile( std::string const &fileName,
                           DictKey const &name );

   Dict *GetDict( DictKey const &DictName );

   /// \brief   Retrieve the default reference DICOM V3 public dictionary.
   Dict* GetDefaultPubDict() { return GetDict(PUB_DICT_NAME); };

   // \brief   Retrieve the virtual reference DICOM dictionary.
   // \warning : not end user intended
   // Dict *GetVirtualDict() { return &VirtualEntry; };

   DictEntry *NewVirtualDictEntry(uint16_t group, uint16_t element,
                                  TagName vr     = GDCM_UNKNOWN,
                                  TagName vm     = GDCM_UNKNOWN,
                                  TagName name   = GDCM_UNKNOWN);

   static std::string BuildDictPath();

protected:
   bool AppendDict(Dict *NewDict, DictKey const &name);

private:
   /// Hash table of all dictionaries contained in this DictSet
   DictSetHT Dicts;

   /// Directory path to dictionaries
   std::string DictPath;

   /// H table for the on the fly created DictEntries  
   TagKeyHT VirtualEntry; 
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
