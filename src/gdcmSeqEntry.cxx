// gdcmSeqEntry.cxx
//-----------------------------------------------------------------------------
//
#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmTS.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"

#include <iostream>
#include <iomanip>
//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmSeqEntry
 * \brief   Constructor from a given gdcmSeqEntry
 */
gdcmSeqEntry::gdcmSeqEntry(gdcmDictEntry* e, int depth) 
             : gdcmDocEntry(e)
{
   delimitor_mode = false;
   seq_term  = NULL;
   SQDepthLevel = depth;
}

/**
 * \ingroup gdcmSeqEntry
 * \brief   Canonical destructor.
 */
gdcmSeqEntry::~gdcmSeqEntry() {
   for(ListSQItem::iterator cc = items.begin();cc != items.end();++cc)
   {
      delete *cc;
   }
   if (!seq_term)
      delete seq_term;
}

//-----------------------------------------------------------------------------
// Print
/*
 * \ingroup gdcmSeqEntry
 * \brief   canonical Printer
 */
void gdcmSeqEntry::Print(std::ostream &os){

   std::ostringstream s,s2;
   std::string vr;
   unsigned short int g, e;
   long lgth;
   size_t o;    
   char greltag[10];  //group element tag
   char st[20]; 

   // First, Print the Dicom Element itself.
   SetPrintLevel(2);   
   PrintCommonPart(os);
   s << std::endl;
   os << s.str();   
 
    // Then, Print each SQ Item   
     for(ListSQItem::iterator cc = items.begin();cc != items.end();++cc)
   {	
	
	std::cout << "SQItemNumber " << (*cc)->GetSQItemNumber() << std::endl;
			
      (*cc)->Print(os);   
   }
   // at end, print the sequence terminator item, if any

   if (delimitor_mode) {
      s2 << "   | " ;  // FIXME : cout the right number of | ! 
      os << s2.str();
      if (seq_term != NULL) {
         seq_term->Print(os);
      } 
      else 
         std::cout
             << "      -------------- should have a sequence terminator item"
             << std::endl;      
   }                    
 }

//-----------------------------------------------------------------------------
// Public

 /// \brief   adds the passed ITEM to the ITEM chained List for this SeQuence.      
void gdcmSeqEntry::AddEntry(gdcmSQItem *sqItem, int itemNumber) {
   sqItem->SetSQItemNumber(itemNumber);
   items.push_back(sqItem);
}

/// \brief Sets the depth level of a Sequence Entry embedded in a SeQuence 
void gdcmSeqEntry::SetDepthLevel(int depth) {
   SQDepthLevel = depth;
}

//-----------------------------------------------------------------------------
// Protected


//-----------------------------------------------------------------------------
// Private

// end-user intended : the guy *wants* to create his own SeQuence ?!?

/// \brief to be written, if really usefull
gdcmDocEntry *gdcmSeqEntry::NewDocEntryByNumber(guint16 group,
                                                guint16 element) {
// TODO
   std::cout << "TODO : gdcmSeqEntry::NewDocEntryByNumber " << std::endl;
   gdcmDocEntry *a;   
   return a;
}

/// \brief to be written, if really usefull
gdcmDocEntry *gdcmSeqEntry::NewDocEntryByName  (std::string Name) {
// TODO
   std::cout << "TODO : gdcmSeqEntry::NewDocEntryByName " << std::endl;
   gdcmDocEntry *a;   
   return a;   
}

/// \brief to be written, if really usefull
gdcmDocEntry *gdcmSeqEntry::GetDocEntryByNumber(guint16 group,
                                                guint16 element) {
// TODO
   std::cout << "TODO : gdcmSeqEntry::GetDocEntryByNumber " << std::endl;
   gdcmDocEntry *a;   
   return a;
}
//-----------------------------------------------------------------------------
