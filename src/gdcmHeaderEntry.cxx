// gdcmHeaderEntry.cxx
//-----------------------------------------------------------------------------
//
#include "gdcmHeaderEntry.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmHeaderEntry
 * \brief   Constructor from a given gdcmDictEntry
 * @param   in Pointer to existing dictionary entry
 */
gdcmHeaderEntry::gdcmHeaderEntry(gdcmDictEntry* in) {
	ImplicitVr = false;
	entry = in;
}

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
