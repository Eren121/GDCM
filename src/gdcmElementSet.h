// gdcmElementSet.h

#ifndef GDCMELEMENTSET_H
#define GDCMELEMENTSET_H

#include <map>
#include <iostream>
#include "gdcmCommon.h"
#include "gdcmDocEntrySet.h"

typedef std::map<TagKey, gdcmDocEntry *> TagDocEntryHT;

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmElementSet : public gdcmDocEntrySet
{
public:
   gdcmElementSet(int);
   ~gdcmElementSet(void);
   virtual bool AddEntry(gdcmDocEntry *Entry);
   virtual bool RemoveEntry(gdcmDocEntry *EntryToRemove);

   virtual void Print(std::ostream &os = std::cout); 
    
protected:
// Variables
   /// Hash Table (map), to provide fast access
   TagDocEntryHT tagHT; 
     
private:
   
};

//-----------------------------------------------------------------------------
#endif

