/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmObject.cxx,v $
  Language:  C++
  Date:      $Date: 2004/07/17 22:47:01 $
  Version:   $Revision: 1.22 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmObject.h"
#include "gdcmGlobal.h"
#include "gdcmDebug.h"
#include "gdcmValEntry.h"

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmObject
 * \brief  Constructor 
 *          
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmDocEntries)
 * @param depth Seaquence depth level
 */
  
gdcmObject::gdcmObject(TagDocEntryHT *ptagHT, int depth) 
          : gdcmSQItem (depth) {
   this->ptagHT = ptagHT;
}


/**
 * \ingroup gdcmObject
 * \brief   Canonical destructor.
 */
gdcmObject::~gdcmObject(void) {
}



//-----------------------------------------------------------------------------
// Public


/**
 * \ingroup gdcmObject
 * \brief   Builds a hash table (multimap) containing 
 *          pointers to all Header Entries (i.e Dicom Element)
 *          related to this 'object'
 * @return
 */ 
TagDocEntryHT gdcmObject::GetEntry(void) {
   TagDocEntryHT HT;
   docEntries=GetDocEntries();   
   for(ListDocEntry::iterator i=docEntries.begin();i!=docEntries.end();++i) {
      HT[(*i)->GetKey()]=*i;
   }
   return(HT);
}

//-----------------------------------------------------------------------------
// Protected
/**
 * \brief   add the 'Object' related Dicom Elements to the listEntries
 *          of a partially created DICOMDIR
 */
void gdcmObject::FillObject(std::list<gdcmElement> elemList) {

  // FillObject rempli le SQItem qui sera accroche au bon endroit

   std::list<gdcmElement>::iterator it;
   uint16_t tmpGr,tmpEl;
   gdcmDictEntry *dictEntry;
   gdcmValEntry *entry;
   
   //gdcmSQItem *s = new gdcmSQItem;
   
   // for all the Elements found in they own part of the DicomDir dict.     
   for(it=elemList.begin();it!=elemList.end();++it)
   {
      tmpGr=it->group;
      tmpEl=it->elem;
      dictEntry=gdcmGlobal::GetDicts()->GetDefaultPubDict()->GetDictEntryByNumber(tmpGr,tmpEl);
      entry=new gdcmValEntry(dictEntry);
      entry->SetOffset(0); // just to avoid further missprinting
      entry->SetValue(it->value);

      // dealing with value length ...
  
      if(dictEntry->GetGroup()==0xfffe) 
      {
         entry->SetLength(entry->GetValue().length());
      }
      else if( (dictEntry->GetVR()=="UL") || (dictEntry->GetVR()=="SL") ) 
      {
         entry->SetLength(4);
      } 
      else if( (dictEntry->GetVR()=="US") || (dictEntry->GetVR()=="SS") ) 
      {
         entry->SetLength(2); 
      } 
      else if(dictEntry->GetVR()=="SQ") 
      {
         entry->SetLength(0xffffffff);
      }
      else
      {
         entry->SetLength(entry->GetValue().length()); 
      }                                
      AddDocEntry(entry);
   }   
}   
