// gdcmSQItem.h
//-----------------------------------------------------------------------------
#ifndef GDCMSQITEM_H
#define GDCMSQITEM_H

#include <list>

#include "gdcmDocEntry.h"
#include "gdcmDocEntrySet.h"
#include "gdcmDocument.h"
//-----------------------------------------------------------------------------
typedef std::list<gdcmDocEntry *> ListDocEntry;
//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmSQItem : public gdcmDocEntrySet
{
public:
   gdcmSQItem(int);
   ~gdcmSQItem(void);

   virtual void Print(std::ostream &os = std::cout); 

 /// \brief   returns the DocEntry chained List for this SQ Item.
   inline ListDocEntry &GetDocEntries() 
      {return docEntries;};   
   
 /// \brief   adds the passed DocEntry to the DocEntry chained List for this SQ Item.      
   inline void AddDocEntry(gdcmDocEntry *e) 
      {docEntries.push_back(e);};         

   virtual bool AddEntry(gdcmDocEntry *Entry); // add to the List
  
   gdcmDocEntry *GetDocEntryByNumber(guint16 group, 
                                     guint16 element);
   gdcmDocEntry *GetDocEntryByName  (std::string Name);
   
   bool SetEntryByNumber(std::string val,guint16 group,
                                         guint16 element);                   
    
   std::string GetEntryByNumber(guint16 group, guint16 element);
   std::string GetEntryByName(TagName name);
      
protected:

   // DocEntry related utilities 
         
   virtual gdcmDocEntry *NewDocEntryByNumber(guint16 group,
                                             guint16 element); // TODO
   virtual gdcmDocEntry *NewDocEntryByName  (std::string Name); //TODO 

// Variables

/// \brief chained list of (Elementary) Doc Entries
  ListDocEntry docEntries;
  
/// \brief SQ Item ordinal number 
  int SQItemNumber;


///\brief pointer to the HTable of the gdcmDocument,
///       (because we don't know it within any gdcmObject nor any gdcmSQItem) 
  TagDocEntryHT *ptagHT;
       
private:


};


//-----------------------------------------------------------------------------
#endif

