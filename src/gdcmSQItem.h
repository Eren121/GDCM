// gdcmSQItem.h
//-----------------------------------------------------------------------------
#ifndef GDCMSQITEM_H
#define GDCMSQITEM_H

#include <list>

#include "gdcmDocEntry.h"
#include "gdcmDocEntrySet.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmDocEntry *> ListDocEntry;
//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmSQItem : public gdcmDocEntrySet
{
public:
   gdcmSQItem(void);
   ~gdcmSQItem(void);

   virtual void Print(std::ostream &os = std::cout); 

 /// \brief   returns the DocEntry chained List for this SQ Item.
   inline ListDocEntry &GetDocEntries() 
      {return docEntries;};   
   
 /// \brief   adds the passed DocEntry to the DocEntry chained List for this SQ Item.      
   inline void AddDocEntry(gdcmDocEntry *e) 
      {docEntries.push_back(e);};         
            
protected:

private:


// Variables

/// \brief chained list of (Elementary) Doc Entries
  ListDocEntry docEntries;
};


//-----------------------------------------------------------------------------
#endif

