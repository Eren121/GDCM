// gdcmElementSet.h

#ifndef GDCMELEMENTSET_H
#define GDCMELEMENTSET_H

#include "gdcmDocEntrySet.h"
#include "gdcmDocEntry.h"

#include <map>
typedef std::string TagKey;
typedef std::map<TagKey, gdcmDocEntry *> TagDocEntryHT;


//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmElementSet : public gdcmDocEntrySet 
{
public:
   gdcmElementSet(void);
   ~gdcmElementSet(void);
   virtual bool AddEntry(gdcmDocEntry *Entry); // add to the H Table

  
protected:

private:

   // DocEntry related utilities    
   virtual gdcmDocEntry *NewDocEntryByNumber(guint16 group,
                                             guint16 element);
   virtual gdcmDocEntry *NewDocEntryByName  (std::string Name);  


// Variables

   /// Hash Table (map), to provide fast access
   TagDocEntryHT tagHT;   
   
};

//-----------------------------------------------------------------------------
#endif

