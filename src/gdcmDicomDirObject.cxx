/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirObject.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/06 16:05:06 $
  Version:   $Revision: 1.13 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDicomDirObject.h"
#include "gdcmGlobal.h"
#include "gdcmDebug.h"
#include "gdcmValEntry.h"
#include "gdcmDictSet.h"

namespace gdcm 
{

//-----------------------------------------------------------------------------
/**
 * \brief  Constructor 
 *          
 * @param depth Sequence depth level
 */
  
DicomDirObject::DicomDirObject(int depth) 
          : SQItem (depth)
{
}


/**
 * \brief   Canonical destructor.
 */
DicomDirObject::~DicomDirObject()
{
}


//-----------------------------------------------------------------------------
// Public


/**
 * \brief   Builds a hash table (multimap) containing 
 *          pointers to all Header Entries (i.e Dicom Element)
 *          related to this 'object'
 * @return
 */ 
TagDocEntryHT DicomDirObject::GetEntry()
{
   TagDocEntryHT HT;
   DocEntries = GetDocEntries();   
   for(ListDocEntry::iterator i = DocEntries.begin(); 
                              i != DocEntries.end(); ++i)
   {
      HT[(*i)->GetKey()] = *i;
   }
   return HT;
}

//-----------------------------------------------------------------------------
// Protected
/**
 * \brief   add the 'Object' related Dicom Elements to the listEntries
 *          of a partially created DICOMDIR
 * @param elemList Element List to add at the right place
 */
void DicomDirObject::FillObject(ListDicomDirMetaElem const & elemList)
{
  // FillObject rempli le SQItem qui sera accroche au bon endroit

   ListDicomDirMetaElem::const_iterator it;
   uint16_t tmpGr,tmpEl;
   DictEntry *dictEntry;
   ValEntry *entry;
      
   // for all the Elements found in they own part of the DicomDir dict.     
   for(it = elemList.begin(); it != elemList.end(); ++it)
   {
      tmpGr = it->Group;
      tmpEl = it->Elem;
      dictEntry = Global::GetDicts()->GetDefaultPubDict()->GetDictEntryByNumber(tmpGr,tmpEl);
      entry = new ValEntry(dictEntry);
      entry->SetOffset(0); // just to avoid further missprinting
      entry->SetValue(it->Value);

      AddEntry(entry);
   }   
}   
} // end namespace gdcm

