// gdcmSeqEntry.cxx
//-----------------------------------------------------------------------------
//
#include "gdcmSeqEntry.h"
#include "gdcmTS.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"


//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmSeqEntry
 * \brief   Constructor from a given gdcmSeqEntry
 * @param   in Pointer to existing dictionary entry
 */
gdcmSeqEntry::gdcmSeqEntry() : gdcmDocEntry( ) {


}


/**
 * \ingroup gdcmSeqEntry
 * \brief   Canonical destructor.
 */
gdcmSeqEntry::~gdcmSeqEntry() 
{
   for(ListSQItem::iterator cc = items.begin();cc != items.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/*
 * \ingroup gdcmSeqEntry
 * \brief   canonical Printer
 */
 

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
