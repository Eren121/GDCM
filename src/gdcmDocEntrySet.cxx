/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntrySet.cxx,v $
  Language:  C++
  Date:      $Date: 2004/09/14 16:47:08 $
  Version:   $Revision: 1.21 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

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
gdcmDocEntrySet::gdcmDocEntrySet(int depthLevel)
{
   SQDepthLevel = depthLevel + 1;   //magic +1 !
}
/**
 * \brief   Canonical destructor.
 */
gdcmDocEntrySet::~gdcmDocEntrySet()
{
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
 * \brief   Build a new Val Entry from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   group group   number of the underlying DictEntry
 * @param   elem  element number of the underlying DictEntry
 */
gdcmValEntry *gdcmDocEntrySet::NewValEntryByNumber(uint16_t group,
                                                   uint16_t elem) 
{
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDictEntry *dictEntry = GetDictEntryByNumber(group, elem);
   if (!dictEntry)
   {
      dictEntry = NewVirtualDictEntry(group, elem);
   }

   gdcmValEntry *newEntry = new gdcmValEntry(dictEntry);
   if (!newEntry) 
   {
      dbg.Verbose(1, "gdcmDocument::NewValEntryByNumber",
                  "failed to allocate gdcmValEntry");
      return 0;
   }
   return newEntry;
}


/**
 * \brief   Build a new Bin Entry from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   group group   number of the underlying DictEntry
 * @param   elem  element number of the underlying DictEntry
 */
gdcmBinEntry *gdcmDocEntrySet::NewBinEntryByNumber(uint16_t group,
                                                   uint16_t elem) 
{
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDictEntry *dictEntry = GetDictEntryByNumber(group, elem);
   if (!dictEntry)
   {
      dictEntry = NewVirtualDictEntry(group, elem);
   }

   gdcmBinEntry *newEntry = new gdcmBinEntry(dictEntry);
   if (!newEntry) 
   {
      dbg.Verbose(1, "gdcmDocument::NewBinEntryByNumber",
                  "failed to allocate gdcmBinEntry");
      return 0;
   }
   return newEntry;
}

/**
 * \brief   Build a new Seq Entry from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Group group   number of the underlying DictEntry
 * @param   Elem  element number of the underlying DictEntry
 */
gdcmSeqEntry *gdcmDocEntrySet::NewSeqEntryByNumber(uint16_t Group,
                                                   uint16_t Elem) 
{
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDictEntry *DictEntry = GetDictEntryByNumber(Group, Elem);
   if (!DictEntry)
      DictEntry = NewVirtualDictEntry(Group, Elem);

   gdcmSeqEntry *NewEntry = new gdcmSeqEntry(DictEntry, 1); // FIXME : 1
   if (!NewEntry) 
   {
      dbg.Verbose(1, "gdcmDocument::NewSeqEntryByNumber",
                  "failed to allocate gdcmSeqEntry");
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
 gdcmDocEntry *gdcmDocEntrySet::GetDocEntryByName(std::string const & name)
 {
   gdcmDict *pubDict = gdcmGlobal::GetDicts()->GetDefaultPubDict();
   gdcmDictEntry *dictEntry = pubDict->GetDictEntryByName(name);
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

std::string gdcmDocEntrySet::GetEntryByName(TagName const & name)
{
   gdcmDict *pubDict = gdcmGlobal::GetDicts()->GetDefaultPubDict();
   gdcmDictEntry *dictEntry = pubDict->GetDictEntryByName(name); 

   if( !dictEntry )
   {
      return GDCM_UNFOUND;
   }

   return GetEntryByNumber(dictEntry->GetGroup(), dictEntry->GetElement()); 
}


/**
 * \brief   Request a new virtual dict entry to the dict set
 * @param   group     group  number of the underlying DictEntry
 * @param   element  element number of the underlying DictEntry
 * @param   vr     VR of the underlying DictEntry
 * @param   fourth owner group
 * @param   name   english name
 */
gdcmDictEntry* gdcmDocEntrySet::NewVirtualDictEntry(uint16_t group,
                                                    uint16_t element,
                                                    std::string const & vr,
                                                    std::string const & fourth,
                                                    std::string const & name)
{
   return gdcmGlobal::GetDicts()->NewVirtualDictEntry(group,element,vr,fourth,name);
}

/** \brief 
 * Creates a new DocEntry (without any 'value' ...)
 * @param   group     group  number of the underlying DictEntry
 * @param   elem  elem number of the underlying DictEntry 
 */
gdcmDocEntry* gdcmDocEntrySet::NewDocEntryByNumber(uint16_t group,
                                                   uint16_t elem)
{
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDict *pubDict = gdcmGlobal::GetDicts()->GetDefaultPubDict();
   gdcmDictEntry *dictEntry = pubDict->GetDictEntryByNumber(group, elem);
   if (!dictEntry)
   {
      dictEntry = NewVirtualDictEntry(group, elem);
   }

   gdcmDocEntry *newEntry = new gdcmDocEntry(dictEntry);
   if (!newEntry) 
   {
      dbg.Verbose(1, "gdcmSQItem::NewDocEntryByNumber",
                  "failed to allocate gdcmDocEntry");
      return 0;
   }
   return newEntry;
}


/** \brief 
 * Creates a new DocEntry (without any 'value' ...)
 * @param   group     group  number of the underlying DictEntry
 * @param   elem  elem number of the underlying DictEntry 
 * @param   VR   V(alue) R(epresentation) of the Entry -if private Entry- 

 */
gdcmDocEntry* gdcmDocEntrySet::NewDocEntryByNumber(uint16_t group,
                                                   uint16_t elem,
                                                   std::string const &VR)
{
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDict *pubDict = gdcmGlobal::GetDicts()->GetDefaultPubDict();
   gdcmDictEntry *dictEntry = pubDict->GetDictEntryByNumber(group, elem);
   if (!dictEntry)
   {
      dictEntry = NewVirtualDictEntry(group, elem, VR);
   }

   gdcmDocEntry *newEntry = new gdcmDocEntry(dictEntry);
   if (!newEntry) 
   {
      dbg.Verbose(1, "gdcmSQItem::NewDocEntryByNumber",
                  "failed to allocate gdcmDocEntry");
      return 0;
   }
   return newEntry;
}
/* \brief
 * Probabely move, as is, to gdcmDocEntrySet, as a non virtual method
 * an remove gdcmDocument::NewDocEntryByName
 */
gdcmDocEntry *gdcmDocEntrySet::NewDocEntryByName  (std::string const & name)
{
  gdcmDict *pubDict = gdcmGlobal::GetDicts()->GetDefaultPubDict();
  gdcmDictEntry *newTag = pubDict->GetDictEntryByName(name);
   if (!newTag)
   {
      newTag = NewVirtualDictEntry(0xffff, 0xffff, "LO", "unkn", name);
   }

   gdcmDocEntry* newEntry = new gdcmDocEntry(newTag);
   if (!newEntry) 
   {
      dbg.Verbose(1, "gdcmSQItem::ObtainDocEntryByName",
                  "failed to allocate gdcmDocEntry");
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
gdcmDictEntry *gdcmDocEntrySet::GetDictEntryByName(std::string const & name) 
{
   gdcmDictEntry *found = 0;
   gdcmDict *pubDict = gdcmGlobal::GetDicts()->GetDefaultPubDict();
   if (!pubDict) 
   {
      dbg.Verbose(0, "gdcmDocument::GetDictEntry",
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
 * @param   element element number of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 */
gdcmDictEntry *gdcmDocEntrySet::GetDictEntryByNumber(uint16_t group,
                                                     uint16_t element) 
{
   gdcmDictEntry *found = 0;
   gdcmDict *pubDict = gdcmGlobal::GetDicts()->GetDefaultPubDict();
   if (!pubDict) 
   {
      dbg.Verbose(0, "gdcmDocument::GetDictEntry",
                     "we SHOULD have a default dictionary");
   }
   else
   {
      found = pubDict->GetDictEntryByNumber(group, element);  
   }
   return found;
}


//-----------------------------------------------------------------------------
// Private


//-----------------------------------------------------------------------------
