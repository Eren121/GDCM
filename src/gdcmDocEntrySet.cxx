/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntrySet.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/22 14:11:34 $
  Version:   $Revision: 1.12 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include <errno.h>
#include "gdcmDebug.h"
#include "gdcmCommon.h"
#include "gdcmGlobal.h"
#include "gdcmDocEntrySet.h"
#include "gdcmException.h"
#include "gdcmDocEntry.h"
#include "gdcmSeqEntry.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDocEntrySet
 * \brief   Constructor from a given gdcmDocEntrySet
 */
gdcmDocEntrySet::gdcmDocEntrySet(int depthLevel) {
   SQDepthLevel = depthLevel + 1;
}
/**
 * \brief   Canonical destructor.
 */
gdcmDocEntrySet::~gdcmDocEntrySet(){
}
//-----------------------------------------------------------------------------
// Print
/*
 * \ingroup gdcmDocEntrySet
 * \brief   canonical Printer
 */


//-----------------------------------------------------------------------------
// Public



/**
 * \brief   Build a new Element Value from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Group group   number of the underlying DictEntry
 * @param   Elem  element number of the underlying DictEntry
 */
gdcmValEntry *gdcmDocEntrySet::NewValEntryByNumber(guint16 Group, guint16 Elem) 
{
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDictEntry *DictEntry = GetDictEntryByNumber(Group, Elem);
   if (!DictEntry)
      DictEntry = NewVirtualDictEntry(Group, Elem);

   gdcmValEntry *NewEntry = new gdcmValEntry(DictEntry);
   if (!NewEntry) 
   {
      dbg.Verbose(1, "gdcmDocument::NewValEntryByNumber",
                  "failed to allocate gdcmValEntry");
      return NULL;
   }
   return NewEntry;
}


/**
 * \brief   Build a new Element Value from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Group group   number of the underlying DictEntry
 * @param   Elem  element number of the underlying DictEntry
 */
gdcmBinEntry *gdcmDocEntrySet::NewBinEntryByNumber(guint16 Group, guint16 Elem) 
{
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDictEntry *DictEntry = GetDictEntryByNumber(Group, Elem);
   if (!DictEntry)
      DictEntry = NewVirtualDictEntry(Group, Elem);

   gdcmBinEntry *NewEntry = new gdcmBinEntry(DictEntry);
   if (!NewEntry) 
   {
      dbg.Verbose(1, "gdcmDocument::NewBinEntryByNumber",
                  "failed to allocate gdcmBinEntry");
      return NULL;
   }
   return NewEntry;
}
//-----------------------------------------------------------------------------
// Protected

/**
 * \brief   Gets a Dicom Element inside a SQ Item Entry, by name
 * @return
 */
 gdcmDocEntry *gdcmDocEntrySet::GetDocEntryByName(std::string name) {
   gdcmDict *PubDict=gdcmGlobal::GetDicts()->GetDefaultPubDict();
   gdcmDictEntry *dictEntry = (*PubDict).GetDictEntryByName(name);
   if( dictEntry == NULL)
      return NULL;
   return GetDocEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement());      
}


/**
 * \brief   Get the value of a Dicom Element inside a SQ Item Entry, by name
 * @param   name : name of the searched element.
 * @return
 */ 

std::string gdcmDocEntrySet::GetEntryByName(TagName name)  {
   gdcmDict *PubDict=gdcmGlobal::GetDicts()->GetDefaultPubDict();
   gdcmDictEntry *dictEntry = (*PubDict).GetDictEntryByName(name); 

   if( dictEntry == NULL)
      return GDCM_UNFOUND;
   return GetEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()); 
}


/**
 * \brief   Request a new virtual dict entry to the dict set
 * @param   group     group  number of the underlying DictEntry
 * @param   element  element number of the underlying DictEntry
 * @param   vr     VR of the underlying DictEntry
 * @param   fourth owner group
 * @param   name   english name
 */
gdcmDictEntry *gdcmDocEntrySet::NewVirtualDictEntry(guint16 group, guint16 element,
                                               std::string vr,
                                               std::string fourth,
                                               std::string name)
{
   return gdcmGlobal::GetDicts()->NewVirtualDictEntry(group,element,vr,fourth,name);
}

/// \brief 

// 
// Probabely move, as is, to gdcmDocEntrySet, as a non virtual method
// an remove gdcmDocument::NewDocEntryByNumber
gdcmDocEntry *gdcmDocEntrySet::NewDocEntryByNumber(guint16 group,
                                                   guint16 elem) {
      
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDict *PubDict=gdcmGlobal::GetDicts()->GetDefaultPubDict();
   gdcmDictEntry *DictEntry = (*PubDict).GetDictEntryByNumber(group, elem);
   if (!DictEntry)
      DictEntry = NewVirtualDictEntry(group, elem);

   gdcmDocEntry *NewEntry = new gdcmDocEntry(DictEntry);
   if (!NewEntry) 
   {
      dbg.Verbose(1, "gdcmSQItem::NewDocEntryByNumber",
                  "failed to allocate gdcmDocEntry");
      return (gdcmDocEntry*)0;
   }
   return NewEntry;
}

/// \brief 
gdcmDocEntry *gdcmDocEntrySet::NewDocEntryByName  (std::string Name) {

  gdcmDict *PubDict=gdcmGlobal::GetDicts()->GetDefaultPubDict();
  gdcmDictEntry *NewTag = (*PubDict).GetDictEntryByName(Name);
   if (!NewTag)
      NewTag = NewVirtualDictEntry(0xffff, 0xffff, "LO", "unkn", Name);

   gdcmDocEntry* NewEntry = new gdcmDocEntry(NewTag);
   if (!NewEntry) 
   {
      dbg.Verbose(1, "gdcmSQItem::ObtainDocEntryByName",
                  "failed to allocate gdcmDocEntry");
      return (gdcmDocEntry *)0;
   }
   return NewEntry;
}


/**
 * \brief   Searches both the public and the shadow dictionary (when they
 *          exist) for the presence of the DictEntry with given name.
 *          The public dictionary has precedence on the shadow one.
 * @param   Name name of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 */
gdcmDictEntry *gdcmDocEntrySet::GetDictEntryByName(std::string Name) 
{
   gdcmDictEntry *found = (gdcmDictEntry *)0;
   gdcmDict *PubDict=gdcmGlobal::GetDicts()->GetDefaultPubDict();
   if (!PubDict) 
   {
      dbg.Verbose(0, "gdcmDocument::GetDictEntry",
                     "we SHOULD have a default dictionary");
   }
   else 
     found = PubDict->GetDictEntryByName(Name);  
   return found;
}

/**
 * \brief   Searches both the public and the shadow dictionary (when they
 *          exist) for the presence of the DictEntry with given
 *          group and element. The public dictionary has precedence on the
 *          shadow one.
 * @param   group   group number of the searched DictEntry
 * @param   element element number of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 */
gdcmDictEntry *gdcmDocEntrySet::GetDictEntryByNumber(guint16 group,guint16 element) 
{
   gdcmDictEntry *found = (gdcmDictEntry *)0;
   gdcmDict *PubDict=gdcmGlobal::GetDicts()->GetDefaultPubDict();
   if (!PubDict) 
   {
      dbg.Verbose(0, "gdcmDocument::GetDictEntry",
                     "we SHOULD have a default dictionary");
   }
   else 
     found = PubDict->GetDictEntryByNumber(group, element);  
   return found;
}


//-----------------------------------------------------------------------------
// Private


//-----------------------------------------------------------------------------
