// gdcmObject.cxx
//-----------------------------------------------------------------------------
#include "gdcmObject.h"
#include "gdcmUtil.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmObject
 * \brief   
 * @param   begin iterator of begin for the object
 * @param   end   iterator of end for the object
 */
gdcmObject::gdcmObject(ListTag::iterator begin,ListTag::iterator end) 
{
   beginObj=begin;
   endObj=end;

   if(beginObj==endObj)
      dbg.Verbose(0, "gdcmObject::gdcmObject empty list");
}

/**
 * \ingroup gdcmObject
 * \brief   Canonical destructor.
 */
gdcmObject::~gdcmObject(void) 
{
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmObject
 * \brief   Prints the Object
 * @return
 */ 
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
/**
 * \ingroup gdcmObject
 * \brief   Get an entry by number
 * @return
 */ 
std::string gdcmObject::GetEntryByNumber(guint16 group, guint16 element) 
{
   for(ListTag::iterator i=beginObj;i!=endObj;++i)
   {
      if ( (*i)->GetGroup()==group && (*i)->GetElement()==element)
         return (*i)->GetValue();
   }
   
   return GDCM_UNFOUND;
}

/**
 * \ingroup gdcmObject
 * \brief   Get an entry by name
 * @param   name name of the searched element.
 * @return
 */ 
std::string gdcmObject::GetEntryByName(TagName name) 
{
   gdcmDict *PubDict=gdcmGlobal::GetDicts()->GetDefaultPubDict();
   gdcmDictEntry *dictEntry = (*PubDict).GetDictEntryByName(name); 

   if( dictEntry == NULL)
      return GDCM_UNFOUND;
   return GetEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()); 
}

/**
 * \ingroup gdcmObject
 * \brief   Get all entries in a hash table
 * @return
 */ 
TagHeaderEntryHT gdcmObject::GetEntry(void)
{
   TagHeaderEntryHT HT;

   for(ListTag::iterator it=beginObj;it!=endObj;++it)
   {
      HT.insert( PairHT( (*it)->GetKey(),(*it)) );
   }

   return(HT);
}

/**
 * \ingroup gdcmObject
 * \brief   Get all entries in a list
 * @return
 */ 
ListTag gdcmObject::GetListEntry(void)
{
   ListTag list;

   for(ListTag::iterator it=beginObj;it!=endObj;++it)
   {
      list.push_back(*it);
   }

   return(list);
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
