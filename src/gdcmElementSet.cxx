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
gdcmElementSet::gdcmElementSet(int depthLevel) 
              : gdcmDocEntrySet(depthLevel) {
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


//-----------------------------------------------------------------------------
// Print
/**
  * \brief   Prints the Header Entries (Dicom Elements)
  *          from the H Table
  * @return
  */ 
void gdcmElementSet::Print(std::ostream & os) {
   for (TagDocEntryHT::iterator i = tagHT.begin(); i != tagHT.end(); ++i)  
   {
      //(*i)->second->SetPrintLevel(printLevel);
      (i->second)->Print(os);   
   } 
}


//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private


/**
 * \brief   add a new Dicom Element pointer to the H Table
 * @param   NewEntry entry to add
 */
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

