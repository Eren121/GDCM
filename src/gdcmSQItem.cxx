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
gdcmSQItem::gdcmSQItem() : gdcmDocEntry( ) {


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
 

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
