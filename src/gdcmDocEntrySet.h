// gdcmDocEntrySet.h
//-----------------------------------------------------------------------------
#ifndef GDCMDOCENTRYSET_H
#define GDCMDOCENTRYSET_H

#include "gdcmException.h"
#include "gdcmDocEntry.h"
 
//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmDocEntrySet
{

public:

   gdcmDocEntrySet(void); 
   ~gdcmDocEntrySet(void);

   void FindDocEntryLength (gdcmDocEntry *Entry, FileType filetype, FILE* fp);
   virtual bool AddEntry(gdcmDocEntry *Entry) =0; // pure virtual
   bool CheckEntryVR(gdcmDocEntry *Entry, std::string vr);
   gdcmDictEntry *NewVirtualDictEntry(guint16 group, 
                                      guint16 element,
                                      std::string vr     = "unkn",
                                      std::string fourth = "unkn",
                                      std::string name   = "unkn"); 
   				          
protected:

   void gdcmDocEntrySet::FindDocEntryLength (gdcmDocEntry *Entry);
      
private:
   
   // DocEntry related utilities
    
   virtual gdcmDocEntry *NewDocEntryByNumber(guint16 group, 
                                             guint16 element)=0; // pure virtual
   virtual gdcmDocEntry *NewDocEntryByName  (std::string Name)=0; // pure virtual  
  
     
};


//-----------------------------------------------------------------------------
#endif

