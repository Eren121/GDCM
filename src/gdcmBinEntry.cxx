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
 
void gdcmBinEntry::Print(std::ostream &os = std::cout) {
   PrintCommonPart(os);
   std::cout << " gdcmBinEntry : Print, so WHAT ?" <<std::endl;
}
//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
   
//-----------------------------------------------------------------------------
