// gdcmElValue.cxx
//-----------------------------------------------------------------------------
// TODO
// A 'gdcmElValue' is actually a 'Dicom Element'.
// WHY such a confusing name???
//
#include "gdcmElValue.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmElValue
 * \brief   Constructor from a given gdcmDictEntry
 * @param   in Pointer to existing dictionary entry
 */
gdcmElValue::gdcmElValue(gdcmDictEntry* in) {
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
