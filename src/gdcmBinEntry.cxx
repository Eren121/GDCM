// gdcmBinEntry.cxx
//-----------------------------------------------------------------------------
//
#include "gdcmBinEntry.h"


//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \brief   Constructor from a given gdcmBinEntry
 */
gdcmBinEntry::gdcmBinEntry(gdcmDictEntry* e) : gdcmValEntry(e) {

}

/**
 * \brief   Canonical destructor.
 */
gdcmBinEntry::~gdcmBinEntry(){
   free (voidArea);
}


//-----------------------------------------------------------------------------
// Print
/*
 * \ingroup gdcmDocEntry
 * \brief   canonical Printer
 */
 

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
   
//-----------------------------------------------------------------------------
