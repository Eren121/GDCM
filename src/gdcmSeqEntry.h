// gdcmSeqEntry.h

#ifndef GDCMSQDOCENTRY_H
#define GDCMSQDOCENTRY_H

#include <list>

#include "gdcmSQItem.h"
#include "gdcmDocEntry.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmSQItem *> ListSQItem;
//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmSeqEntry : public gdcmDocEntry 
{
public:
   gdcmSeqEntry(gdcmDictEntry* e);
   ~gdcmSeqEntry(void);
   
   virtual void Print(std::ostream &os = std::cout); 

 /// \brief   returns the SQITEM chained List for this SeQuence.
   inline ListSQItem &GetSQItems() 
      {return items;};

    void AddEntry(gdcmSQItem *it); 

 /// \brief   creates a new SQITEM for this SeQuence.
    gdcmSQItem * NewItem(void);
    
   gdcmDocEntry *NewDocEntryByNumber(guint16 group, guint16 element);    
   gdcmDocEntry *NewDocEntryByName  (std::string Name); 
     
protected:

private:

// Variables

/// \brief chained list of SQ Items
   ListSQItem items;

};


//-----------------------------------------------------------------------------
#endif

