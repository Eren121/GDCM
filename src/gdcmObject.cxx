// gdcmObject.cxx
//-----------------------------------------------------------------------------
#include "gdcmObject.h"
#include "gdcmUtil.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
gdcmObject::gdcmObject(ListTag::iterator begin,ListTag::iterator end) 
{
   beginObj=begin;
   endObj=end;

   if(beginObj==endObj)
      dbg.Verbose(0, "gdcmObject::gdcmObject empty list");
}

gdcmObject::~gdcmObject(void) 
{
}

//-----------------------------------------------------------------------------
// Print
void gdcmObject::Print(std::ostream &os)
{
   if(printLevel>=0)
   {
      for(ListTag::iterator i=beginObj;i!=endObj;++i)
      {
         (*i)->SetPrintLevel(printLevel);
         (*i)->Print(os);
      }
   }
}

//-----------------------------------------------------------------------------
// Public
std::string gdcmObject::GetEntryByNumber(guint16 group, guint16 element) 
{
   for(ListTag::iterator i=beginObj;i!=endObj;++i)
   {
      if ( (*i)->GetGroup()==group && (*i)->GetElement()==element)
         return (*i)->GetValue();
   }
   
   return GDCM_UNFOUND;
}


std::string gdcmObject::GetEntryByName(TagName name) 
{
   gdcmDict *PubDict=gdcmGlobal::GetDicts()->GetDefaultPubDict();
   gdcmDictEntry *dictEntry = (*PubDict).GetDictEntryByName(name); 

   if( dictEntry == NULL)
      return GDCM_UNFOUND;
   return GetEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()); 
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
