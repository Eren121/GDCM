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

   virtual bool AddEntry(gdcmDocEntry *Entry) = 0; // pure virtual
   bool CheckEntryVR(gdcmDocEntry *Entry, std::string vr); 
   virtual void Print (std::ostream & os = std::cout) = 0;// pure virtual
       				          
protected:

   void gdcmDocEntrySet::FindDocEntryLength (gdcmDocEntry *Entry);
   
   // DocEntry related utilities
    
   virtual gdcmDocEntry *NewDocEntryByNumber(guint16 group, 
                                             guint16 element)=0; // pure virtual
   virtual gdcmDocEntry *NewDocEntryByName  (std::string Name)=0; // pure virtual  
  

         
private:
    
};


//-----------------------------------------------------------------------------
#endif

