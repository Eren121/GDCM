/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntrySet.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/06 16:29:22 $
  Version:   $Revision: 1.31 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmDocEntrySet.h"

#include "gdcmDebug.h"
#include "gdcmCommon.h"
#include "gdcmDictSet.h"
#include "gdcmGlobal.h"
#include "gdcmException.h"
#include "gdcmDocEntry.h"
#include "gdcmSeqEntry.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"

#include <assert.h>

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Public
/**
 * \brief  Gets a Dicom Element inside a SQ Item Entry, by name
 * @param  name of the element to be found.
 * @return
 */
DocEntry* DocEntrySet::GetDocEntryByName( TagName const & name )
{
   Dict *pubDict = Global::GetDicts()->GetDefaultPubDict();
   DictEntry *dictEntry = pubDict->GetDictEntryByName(name);
   if( !dictEntry )
   {
      return 0;
   }

   return GetDocEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement());      
}


/**
 * \brief   Get the value of a Dicom Element inside a SQ Item Entry, by name
 * @param   name : name of the searched element.
 * @return
 */ 

std::string DocEntrySet::GetEntryByName(TagName const & name)
{
   Dict *pubDict = Global::GetDicts()->GetDefaultPubDict();
   DictEntry *dictEntry = pubDict->GetDictEntryByName(name); 

   if( !dictEntry )
   {
      return GDCM_UNFOUND;
   }

   return GetEntryByNumber(dictEntry->GetGroup(), dictEntry->GetElement()); 
}

/**
 * \brief   Request a new virtual dict entry to the dict set
 * @param   group     group  number of the underlying DictEntry
 * @param   elem  element number of the underlying DictEntry
 * @param   vr     VR of the underlying DictEntry
 * @param   fourth owner group
 * @param   name   english name
 */
DictEntry* DocEntrySet::NewVirtualDictEntry( uint16_t group,uint16_t elem,
                                             TagName const & vr,
                                             TagName const & fourth,
                                             TagName const & name )
{
   return Global::GetDicts()->NewVirtualDictEntry(group,elem,vr,fourth,name);
}

//-----------------------------------------------------------------------------
// Protected
/**
 * \brief   Build a new Val Entry from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   group group   number of the new Entry
 * @param   elem  element number of the new Entry
 * @param   vr     VR of the new Entry 
 */
ValEntry *DocEntrySet::NewValEntryByNumber(uint16_t group,uint16_t elem,
                                           TagName const & vr) 
{
   DictEntry *dictEntry = GetDictEntryByNumber(group, elem, vr);
   assert(dictEntry);

   ValEntry *newEntry = new ValEntry(dictEntry);
   if (!newEntry) 
   {
      dbg.Verbose(1, "Document::NewValEntryByNumber",
                  "failed to allocate ValEntry");
      return 0;
   }
   return newEntry;
}


/**
 * \brief   Build a new Bin Entry from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   group group   number of the new Entry
 * @param   elem  element number of the new Entry
 * @param   vr     VR of the new Entry 
 */
BinEntry *DocEntrySet::NewBinEntryByNumber(uint16_t group,uint16_t elem,
                                           TagName const & vr) 
{
   DictEntry *dictEntry = GetDictEntryByNumber(group, elem, vr);
   assert(dictEntry);

   BinEntry *newEntry = new BinEntry(dictEntry);
   if (!newEntry) 
   {
      dbg.Verbose(1, "Document::NewBinEntryByNumber",
                  "failed to allocate BinEntry");
      return 0;
   }
   return newEntry;
}

/**
 * \brief   Build a new Seq Entry from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   group group   number of the new Entry
 * @param   elem  element number of the new Entry
 */
SeqEntry* DocEntrySet::NewSeqEntryByNumber(uint16_t group,uint16_t elem) 
{
   DictEntry *dictEntry = GetDictEntryByNumber(group, elem, "SQ");
   assert(dictEntry);

   SeqEntry *newEntry = new SeqEntry( dictEntry );
   if (!newEntry)
   {
      dbg.Verbose(1, "Document::NewSeqEntryByNumber",
                  "failed to allocate SeqEntry");
      return 0;
   }
   return newEntry;
}

/** \brief 
 * Creates a new DocEntry (without any 'value' ...)
 * @param   group     group  number of the underlying DictEntry
 * @param   elem  elem number of the underlying DictEntry 
 * @param   vr    V(alue) R(epresentation) of the Entry -if private Entry- 
 */
DocEntry* DocEntrySet::NewDocEntryByNumber(uint16_t group, uint16_t elem,
                                           TagName const & vr)
{
   DictEntry *dictEntry = GetDictEntryByNumber(group, elem, vr);
   assert(dictEntry);

   // Create the DocEntry
   DocEntry *newEntry = new DocEntry(dictEntry);
   if (!newEntry) 
   {
      dbg.Verbose(1, "SQItem::NewDocEntryByNumber",
                  "failed to allocate DocEntry");
      return 0;
   }
   return newEntry;
}

/**
 * \brief   Searches both the public and the shadow dictionary (when they
 *          exist) for the presence of the DictEntry with given name.
 *          The public dictionary has precedence on the shadow one.
 * @param   name Name of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 */
DictEntry *DocEntrySet::GetDictEntryByName(TagName const & name) 
{
   DictEntry *found = 0;
   Dict *pubDict = Global::GetDicts()->GetDefaultPubDict();
   if (!pubDict) 
   {
      dbg.Verbose(0, "Document::GetDictEntry",
                     "we SHOULD have a default dictionary");
   }
   else
   {
      found = pubDict->GetDictEntryByName(name);  
   }
   return found;
}

/**
 * \brief   Searches both the public and the shadow dictionary (when they
 *          exist) for the presence of the DictEntry with given
 *          group and element. The public dictionary has precedence on the
 *          shadow one.
 * @param   group   group number of the searched DictEntry
 * @param   elem element number of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 */
DictEntry *DocEntrySet::GetDictEntryByNumber(uint16_t group,uint16_t elem) 
{
   DictEntry *found = 0;
   Dict *pubDict = Global::GetDicts()->GetDefaultPubDict();
   if (!pubDict) 
   {
      dbg.Verbose(0, "Document::GetDictEntry",
                     "we SHOULD have a default dictionary");
   }
   else
   {
      found = pubDict->GetDictEntryByNumber(group, elem);  
   }
   return found;
}

DictEntry *DocEntrySet::GetDictEntryByNumber(uint16_t group, uint16_t elem,
                                             TagName const & vr)
{
   DictEntry *dictEntry = GetDictEntryByNumber(group,elem);
   DictEntry *goodEntry = dictEntry;
   std::string goodVR=vr;

   if (elem==0x0000)
      goodVR="UL";

   if (goodEntry)
      if (goodEntry->GetVR() != goodVR && goodVR!=GDCM_UNKNOWN)
         goodEntry=NULL;

   // Create a new virtual DictEntry if necessary
   if (!goodEntry)
   {
      if (dictEntry)
         goodEntry = NewVirtualDictEntry(group, elem, goodVR,"FIXME",dictEntry->GetName());
      else
         goodEntry = NewVirtualDictEntry(group, elem, goodVR);
   }

   return goodEntry;
}

//-----------------------------------------------------------------------------
// Private

} // end namespace gdcm

//-----------------------------------------------------------------------------
