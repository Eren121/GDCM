// gdcmSQItem.cxx
//-----------------------------------------------------------------------------
//
#include "gdcmSQItem.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"


//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmSQItem
 * \brief   Constructor from a given gdcmSQItem
 */
gdcmSQItem::gdcmSQItem( ) 
          : gdcmDocEntrySet() {
}

/**
 * \brief   Canonical destructor.
 */
gdcmSQItem::~gdcmSQItem() 
{
   for(ListDocEntry::iterator cc = docEntries.begin();
       cc != docEntries.end();
       ++cc)
   {
      delete *cc;
   }
}


//-----------------------------------------------------------------------------
// Print
/*
 * \ingroup gdcmSQItem
 * \brief   canonical Printer
 */
 void gdcmSQItem::Print(std::ostream & os) {
   std::ostringstream s;
   s << "   | " ;
   for (ListDocEntry::iterator i = docEntries.begin();  
        i != docEntries.end();
        ++i)
   {
       os << s.str();
      //(*i)->SetPrintLevel(printLevel); //self->GetPrintLevel() ?
      (*i)->SetPrintLevel(2);
      (*i)->Print(os);   
   } 
}

//-----------------------------------------------------------------------------
// Public

bool gdcmSQItem::AddEntry(gdcmDocEntry *entry) {
   std::cout << "    === entree ds gdcmSQItem::AddEntry " 
             << hex << entry->GetGroup()
             << " "
             << entry->GetElement() 
	     << " lgt "
             << entry->GetReadLength()	     
             << std::endl; 
	     
   docEntries.push_back(entry);
   //TODO : check if it worked
   return true;
}   
				  
//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

// end-user intended : the guy *wants* to create his own SeQuence ?!?
gdcmDocEntry *gdcmSQItem::NewDocEntryByNumber(guint16 group,
                                              guint16 element) {
// TODO				  
   gdcmDocEntry *a;   
   return a;				  
}

gdcmDocEntry *gdcmSQItem::NewDocEntryByName  (std::string Name) {
// TODO	: 			  
   gdcmDocEntry *a;   
   return a;				  
}
//-----------------------------------------------------------------------------
