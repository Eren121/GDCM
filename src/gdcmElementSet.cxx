// gdcmElementSet.cxx
//-----------------------------------------------------------------------------
//
#include "gdcmElementSet.h"
#include "gdcmDebug.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmElementSet
 * \brief   Constructor from a given gdcmElementSet
 */
gdcmElementSet::gdcmElementSet() {
   //TagDocEntryHT tagHT est un champ de gdcmElementSet.
   // inutile de faire new ?
      
}

/**
 * \ingroup gdcmElementSet
 * \brief   Canonical destructor.
 */
gdcmElementSet::~gdcmElementSet() 
{
  gdcmDocEntry* EntryToDelete;  
  for(TagDocEntryHT::iterator cc = tagHT.begin();cc != tagHT.end();++cc)
   {
      EntryToDelete = cc->second;
      if ( EntryToDelete )
         delete EntryToDelete;  // TODO : a verifier
   }
   tagHT.clear();
}


//-----------------------------------------------------------------------------
// Public

bool gdcmElementSet::AddEntry( gdcmDocEntry *NewEntry) {
   TagKey key;
   key = NewEntry->GetKey();

   if(tagHT.count(key) == 1)
   {
      dbg.Verbose(1, "gdcmElementSet::AddEntry key already present: ", key.c_str());
      return(false);
   } 
   else 
   {
      tagHT[NewEntry->GetKey()] = NewEntry;
      return(true);
   }   
}

// end-user intended : the guy *wants* to create his own SeQuence ?!?
gdcmDocEntry *gdcmElementSet::NewDocEntryByNumber(guint16 group,
                                                  guint16 element) {
// TODO				  
   gdcmDocEntry *a;   
   return a;				  
}

gdcmDocEntry *gdcmElementSet::NewDocEntryByName  (std::string Name) {
// TODO	: 			  
   gdcmDocEntry *a;   
   return a;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
