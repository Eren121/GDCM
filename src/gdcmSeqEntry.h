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
   gdcmSeqEntry(gdcmDictEntry* e, int depth);
   ~gdcmSeqEntry(void);
   
   virtual void Print(std::ostream &os = std::cout); 

   /// \brief   returns the SQITEM chained List for this SeQuence.
   inline ListSQItem &GetSQItems() {return items;};
      
   /// \brief Sets the delimitor mode
   inline void SetDelimitorMode(bool dm) { delimitor_mode = dm;}

   /// \brief Sets the Sequence Delimitation Item
   inline void SetSequenceDelimitationItem(gdcmDocEntry * e) { seq_term = e;}
       
    void AddEntry(gdcmSQItem *it, int itemNumber); 

   /// \brief creates a new SQITEM for this SeQuence.
   gdcmSQItem * NewItem(void);
    
   gdcmDocEntry *NewDocEntryByNumber(guint16 group, guint16 element);    
   gdcmDocEntry *NewDocEntryByName  (std::string Name); 
   gdcmDocEntry *GetDocEntryByNumber(guint16 group, guint16 element);

   void SetDepthLevel(int depth);
         
protected:

private:
// Variables

   /// \brief If this Sequence is in delimitor mode (length =0xffffffff) or not
   bool delimitor_mode;
   
   /// \brief Chained list of SQ Items
   ListSQItem items;
   
   /// \brief sequence terminator item 
   gdcmDocEntry *seq_term;
};

//-----------------------------------------------------------------------------
#endif

