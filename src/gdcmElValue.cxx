// gdcmElValue.cxx

#include "gdcm.h"

/**
 * \ingroup gdcmElValue
 * \brief   Constructor from a given gdcmDictEntry
 * @param   in Pointer to existing dictionary entry
 */
ElValue::ElValue(gdcmDictEntry* in) {
	ImplicitVr = false;
	entry = in;
}
