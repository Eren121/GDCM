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

   gdcmDocEntrySet(int depth = 0); 
   ~gdcmDocEntrySet(void);

/// \brief adds any type of entry to the entry set (pure vitual)
   virtual bool AddEntry(gdcmDocEntry *Entry) = 0; // pure virtual

//   bool CheckEntryVR(gdcmDocEntry *Entry, std::string vr);
	 
/// \brief prints any type of entry to the entry set (pure vitual) 	
   virtual void Print (std::ostream & os = std::cout) = 0;// pure virtual

   /// \brief Gets the depth level of a Dicom Header Entry embedded in a SeQuence
   inline int GetDepthLevel(void) 
      {return(SQDepthLevel);}
		
   /// \brief Sets the depth level of a Dicom Header Entry embedded in a SeQuence
   inline void SetDepthLevel(int depth) 
      {SQDepthLevel = depth;}
         
protected:

//   void gdcmDocEntrySet::FindDocEntryLength (gdcmDocEntry *Entry);
   
   // DocEntry related utilities
    
 /// \brief   Build a new Element Value from all the low level arguments. 
 ///         Check for existence of dictionary entry, and build
 ///          a default one when absent (pure virtual)
   virtual gdcmDocEntry *NewDocEntryByNumber(guint16 group, 
                                             guint16 element)=0; // pure virtual

 /// \brief   Build a new Element Value from all the low level arguments. 
 ///         Check for existence of dictionary entry, and build
 ///          a default one when absent (pure virtual)
    virtual gdcmDocEntry *NewDocEntryByName  (std::string Name)=0;// pure virtual  

   /// Gives the depth level of the element set inside SeQuences   
   int SQDepthLevel;
                 				          
private:
    
};


//-----------------------------------------------------------------------------
#endif

