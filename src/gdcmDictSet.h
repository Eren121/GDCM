/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDictSet.h,v $
  Language:  C++
  Date:      $Date: 2005/10/20 15:24:09 $
  Version:   $Revision: 1.47 $
                                                                                
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
//-----------------------------------------------------------------------------
typedef std::map<DictKey, Dict*> DictSetHT;

//-----------------------------------------------------------------------------
/**
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

   void Print(std::ostream &os = std::cout, std::string const &indent = "" );

   // Probabely useless !
   //EntryNamesList *GetPubDictEntryNames();
   //EntryNamesByCatMap *GetPubDictEntryNamesByCategory();

   Dict *LoadDictFromFile( std::string const &fileName,
                           DictKey const &name );

   Dict *GetDict( DictKey const &DictName );

   /// \brief   Returns the default reference DICOM V3 public dictionary.
   Dict* GetDefaultPubDict() { return GetDict(PUB_DICT_NAME); }

   // \ brief   Returns the virtual references DICOM dictionary.
   // \ warning : not end user intended
   // Dict *GetVirtualDict() { return &VirtualEntries; }

   Dict *GetFirstDict();
   Dict *GetNextDict();

   static std::string BuildDictPath();

protected:
   bool AppendDict(Dict *NewDict, DictKey const &name);

private:
   /// Hash table of all dictionaries contained in this DictSet
   DictSetHT Dicts;
   DictSetHT::iterator ItDictHt;

   /// Directory path to dictionaries
   std::string DictPath;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
