// gdcmObject.cxx
//-----------------------------------------------------------------------------
#include "gdcmObject.h"
#include "gdcmUtil.h"


/*
gdcmObject::gdcmObject() {

}


gdcmObject::~gdcmObject() {

}
*/

std::string gdcmObject::GetEntryByNumber(guint16 group, guint16 element) {
  guint16 gr, el;
  ListTag::iterator deb , fin;  
  deb = beginObj;	
  fin = endObj; 
  
  ListTag::iterator i=deb; 
  
  if (deb == fin) cout << "Big Trouble : Empty List!" <<endl;
  while ( i!= fin) {
     gr = (*(*i)).GetGroup();   
     el = (*(*i)).GetElement();
     if ( gr==group && el==element) 
        return (*(*i)).GetValue();;
     ++i;        
  }        
   return GDCM_UNFOUND;
}


std::string gdcmObject::GetEntryByName(TagName name) {
   gdcmDict *PubDict=gdcmGlobal::GetDicts()->GetDefaultPubDict();
   gdcmDictEntry *dictEntry = (*PubDict).GetDictEntryByName(name); 
   if( dictEntry == NULL)
      return GDCM_UNFOUND;
   return GetEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()); 
}

