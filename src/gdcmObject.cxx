// gdcmObject.cxx
//-----------------------------------------------------------------------------
#include "gdcmObject.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmObject
 * \brief  Constructor 
 * @param  begin  iterator on the first Header Entry (i.e Dicom Element)
 *                related to this 'Object'
 * @param  end  iterator on the last Header Entry 
 *              (i.e Dicom Element) related to this 'Object'             
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 * @param plistEntries pointer to the chained List (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 */
gdcmObject::gdcmObject(ListTag::iterator begin, ListTag::iterator end,
              TagHeaderEntryHT *ptagHT, ListTag *plistEntries) {
   beginObj = begin;
   endObj   = end;
   this->ptagHT = ptagHT;
   this->plistEntries = plistEntries;
   if(begin==end)
      dbg.Verbose(0, "gdcmObject::gdcmObject empty list");
}

/**
 * \ingroup gdcmObject
 * \brief   Canonical destructor.
 */
gdcmObject::~gdcmObject(void) {
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmObject
 * \brief   Prints the Object
 * @return
 */ 
void gdcmObject::Print(std::ostream &os) {
   if(printLevel>=0) {
      ListTag::iterator i;
      for(i=beginObj;;++i) {
         (*i)->SetPrintLevel(printLevel);
         (*i)->Print(os);
	 if (i == endObj) break;
      }
   }
}

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup gdcmObject
 * \brief   Get the value of an Header Entries (i.e Dicom Element) by number
 * @return
 */ 
std::string gdcmObject::GetEntryByNumber(guint16 group, guint16 element) {
   for(ListTag::iterator i=beginObj;;++i) {
      if ( (*i)->GetGroup()==group && (*i)->GetElement()==element)
         return (*i)->GetValue();
      if (i == endObj) break;  
   }   
   return GDCM_UNFOUND;
}

/**
 * \ingroup gdcmObject
 * \brief   Get the value of an Header Entries (i.e Dicom Element) by name
 * @param   name : name of the searched element.
 * @return
 */ 
std::string gdcmObject::GetEntryByName(TagName name)  {
   gdcmDict *PubDict=gdcmGlobal::GetDicts()->GetDefaultPubDict();
   gdcmDictEntry *dictEntry = (*PubDict).GetDictEntryByName(name); 

   if( dictEntry == NULL)
      return GDCM_UNFOUND;
   return GetEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()); 
}

/**
 * \ingroup gdcmObject
 * \brief   Set the 'boundaries' gdcmObject (gdcmDicomDirPatient,
 *          gdcmDicomDirStudy, gdcmDicomDirSerie, gdcmDicomDirImage)
 *          comming for the parsing of a DICOMDIR file
 * \warning NOT en user intended function
 * @param  flag = 0 when META to be dealt with
 */ 
void gdcmObject::ResetBoundaries(int flag) {

   if (flag) { // it's NOT a META
     // upwards to fffe,e000   
       for( i=j=debut();
            ((*i)->GetGroup() != 0xfffe)  && ((*i)->GetElement() != 0x0000);
	    --i,j--) {	    
       }
      beginObj=j;
   }
         
  // downwards to fffe,e000       
   for( i=j=fin();
        ((*i)->GetGroup() != 0xfffe)  && ((*i)->GetElement() != 0x0000);
	--i,j--) {	    
   }
   j--;
   endObj=j;    
}

/**
 * \ingroup gdcmParser
 * \brief   Sets Entry (Dicom Element) value of an element,
 *          specified by it's tag (Group, Number) 
 *          and the length, too ...
 *          If the Element is not found, it's just created !
 * \warning we suppose, right now, the element belongs to a Public Group
 *          (NOT a shadow one)       
 * @param   val string value to set
 * @param   group Group of the searched tag.
 * @param   element Element of the searched tag.
 * @return  true if element was found or created successfully
 */
 bool gdcmObject::SetEntryByNumber(std::string val,guint16 group, 
						   guint16 element) {

   for(ListTag::iterator i=beginObj;;++i) { 
      if ( (*i)->GetGroup() == 0xfffe && (*i)->GetElement() == 0xe000 ) 
         continue;
      if ( group   < (*i)->GetGroup() || 
           (group == (*i)->GetGroup() && element < (*i)->GetElement()) ){
	 // instead of ReplaceOrCreateByNumber 
	 // that is a method of gdcmParser :-( 
         gdcmHeaderEntry *Entry;
         TagKey key = gdcmDictEntry::TranslateToKey(group, element);
         if ( ! ptagHT->count(key)) {
	   // we assume a Public Dictionnary *is* loaded
           gdcmDict *PubDict         = gdcmGlobal::GetDicts()->GetDefaultPubDict();
           // if the invoked (group,elem) doesn't exist inside the Dictionary
	   // we create a VirtualDictEntry
           gdcmDictEntry *DictEntry  = PubDict->GetDictEntryByNumber(group, element);
	   if (DictEntry == NULL) {
	      DictEntry=gdcmGlobal::GetDicts()->NewVirtualDictEntry(group,element,"UN","??","??");
	   } 
           // we assume the constructor didn't fail
           Entry = new gdcmHeaderEntry(DictEntry);
	   // ----
	   // TODO
	   // ----
	   // better we don't assume too much !
	   // in the next release, gdcmObject will be used 
	   // to describe any Header Entry ...
         } else {
            Entry = ptagHT->find(key)->second;
         }
         Entry->SetValue(val); 
         Entry->SetLength(val.length());
         plistEntries->insert(i,Entry); 
	 return true;
      }	   
      if (group == (*i)->GetGroup() && element == (*i)->GetElement() ) {
         (*i)->SetValue(val);
         (*i)->SetLength(val.length()); 
         return true;    
      }   
   }						    
}
/**
 * \ingroup gdcmObject
 * \brief   Builds a hash table (multimap) containing 
 *          pointers to all Header Entries (i.e Dicom Element)
 *          related to this 'object'
 * @return
 */ 
TagHeaderEntryHT gdcmObject::GetEntry(void) {
   TagHeaderEntryHT HT;
   for(ListTag::iterator i=beginObj;;++i) {
      HT.insert( PairHT( (*i)->GetKey(),(*i)) );
      if (i == endObj) break;      
   }
   return(HT);
}

/**
 * \ingroup gdcmObject
 * \brief   Builds a Chained List containing 
 *          pointers to all Header Entries (i.e Dicom Element)
 *          related to this 'object'
 * @return
 */ 
ListTag gdcmObject::GetListEntry(void) {
   ListTag list;
   for(ListTag::iterator i=beginObj;;++i) {
      list.push_back(*i);
      if (i == endObj) break;      
   }
   return(list);
}


//-----------------------------------------------------------------------------
// Protected
/**
 * \ingroup gdcmObject
 * \brief   add the 'Object' related Dicom Elements to the listEntries
 *          of a partially created DICOMDIR
 */
void gdcmObject::FillObject(std::list<gdcmElement> elemList) {
   std::list<gdcmElement>::iterator it;
   guint16 tmpGr,tmpEl;
   gdcmDictEntry *dictEntry;
   gdcmHeaderEntry *entry;
      
   debInsertion = this->fin(); 
   ++debInsertion;
   finInsertion=debInsertion;
   
   for(it=elemList.begin();it!=elemList.end();++it)
   {
      tmpGr=it->group;
      tmpEl=it->elem;
      dictEntry=gdcmGlobal::GetDicts()->GetDefaultPubDict()->GetDictEntryByNumber(tmpGr,tmpEl);
      entry=new gdcmHeaderEntry(dictEntry);
      entry->SetOffset(0); // just to avoid further missprinting
      entry->SetValue(it->value);

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
      ptagHT->insert( PairHT(entry->GetKey(),entry) ); // add in the (multimap) H Table
      plistEntries->insert(debInsertion ,entry);       // add at the begining of the Patient list
      ++finInsertion;				           
   }
     
   i=fin();
   i++;
   j=debInsertion;
   j--;
}   
//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
