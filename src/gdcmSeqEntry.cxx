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
gdcmSeqEntry::gdcmSeqEntry(gdcmDictEntry* e) 
             : gdcmDocEntry(e)
{
   delimitor_mode = false;
   seq_term  = NULL;   
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

   std::ostringstream s;
   string vr;
   unsigned short int g, e;
   long lgth;
   size_t o;    
   char greltag[10];  //group element tag
   char st[20]; 


   SetPrintLevel(2);   
//   PrintCommonPart(os); // FIXME : why doesn't it work ?
     
   // First, Print the Dicom Element itself. 
   g  = GetGroup();
   e  = GetElement();   
   o  = GetOffset();
   vr = GetVR();
   sprintf(greltag,"%04x|%04x ",g,e);           
   s << greltag ;
   if (GetPrintLevel()>=2) { 
      s << "lg : ";
      lgth = GetReadLength(); // ReadLength, as opposed to UsableLength
      if (lgth == 0xffffffff) {
         sprintf(st,"x(ffff)");  // I said : "x(ffff)" !
         s.setf(std::ios::left);
         s << std::setw(10-strlen(st)) << " ";  
         s << st << " ";
         s.setf(std::ios::left);
         s << std::setw(8) << "-1";      
      } else {
         sprintf(st,"x(%x)",lgth);
         s.setf(std::ios::left);
         s << std::setw(10-strlen(st)) << " ";  
         s << st << " ";
         s.setf(std::ios::left);
         s << std::setw(8) << lgth; 
      }
      s << " Off.: ";
      sprintf(st,"x(%x)",o); 
      s << std::setw(10-strlen(st)) << " ";
      s << st << " ";
      s << std::setw(8) << o; 
   }

   s << "[" << vr  << "] ";
   if (GetPrintLevel()>=1) {      
      s.setf(std::ios::left);
      s << std::setw(66-GetName().length()) << " ";
   }
    
   s << "[" << GetName()<< "]";

   s << std::endl;
   os << s.str();

 
    // Then, Print each SQ Item   
     for(ListSQItem::iterator cc = items.begin();cc != items.end();++cc)
   {
      //(*cc)->SetPrintLevel(GetPrintLevel()); aurait-ce un sens ?
      (*cc)->Print(os);
   }
   // at end, print the sequence terminator item, if any
   /*
   if (delimitor_mode) {  // TODO : find the trick to print it properly
      s << "   | " ;   
      os << s.str();
      if (seq_term = NULL)
         seq_term->Print(os);      
   }        
  */             
 }

//-----------------------------------------------------------------------------
// Public

 /// \brief   adds the passed ITEM to the ITEM chained List for this SeQuence.      
void gdcmSeqEntry::AddEntry(gdcmSQItem *sqItem) {
   items.push_back(sqItem);
}
//-----------------------------------------------------------------------------
// Protected


//-----------------------------------------------------------------------------
// Private

// end-user intended : the guy *wants* to create his own SeQuence ?!?
gdcmDocEntry *gdcmSeqEntry::NewDocEntryByNumber(guint16 group,
                                                guint16 element) {
// TODO				  
   gdcmDocEntry *a;   
   return a;				  
}

gdcmDocEntry *gdcmSeqEntry::NewDocEntryByName  (std::string Name) {
// TODO	: 			  
   gdcmDocEntry *a;   
   return a;
}	
//-----------------------------------------------------------------------------
