// gdcmObject.cxx
//-----------------------------------------------------------------------------
#include "gdcmObject.h"
#include "gdcmUtil.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
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
      //for(ListTag::iterator i=beginObj;i!=endObj;++i) // JPR
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
   //for(ListTag::iterator i=beginObj;i!=endObj;++i) // JPR
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
 * \ingroup gdcmParser
 * \brief   Sets Entry (Dicom Element) value of an element,
 *          specified by it's tag (Group, Number) 
 *          and the length, too ...       
 * @param   val string value to set
 * @param   group Group of the searched tag.
 * @param   element Element of the searched tag.
 * @return  true if element was found, else false
 */
 bool gdcmObject::SetEntryByNumber(std::string val,guint16 group, 
 						   guint16 element) {
						   
   //for(ListTag::iterator i=beginObj;i!=endObj;++i) // JPR
   for(ListTag::iterator i=beginObj;;++i) {  
      if ( (*i)->GetGroup()==group && (*i)->GetElement()==element) {
         (*i)->SetValue(val);
	 (*i)->SetLength(val.length()+1);
         return true;
      }
      if (i == endObj) break;      
   }
   return false;   						    
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
   //for(ListTag::iterator i=beginObj;i!=endObj;++i) // JPR
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
   //for(ListTag::iterator i=beginObj;i!=endObj;++i) // JPR
   for(ListTag::iterator i=beginObj;;++i) {
      list.push_back(*i);
      if (i == endObj) break;      
   }
   return(list);
}


//-----------------------------------------------------------------------------
// Protected
/*
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
      entry->SetOffset(0); // just to avoid missprinting //JPR
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
      plistEntries->insert(debInsertion ,entry);       // en tete de liste des Patients
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
