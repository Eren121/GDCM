// gdcmHeaderEntry.cxx
//-----------------------------------------------------------------------------
// TODO
// A 'gdcmHeaderEntry' is actually a 'Dicom Element'.
// WHY such a confusing name???
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
