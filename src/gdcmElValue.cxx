// gdcmElValue.cxx

#include "gdcmElValue.h"


/**
 * \ingroup gdcmElValue
 * \brief   Constructor from a given gdcmDictEntry
 * @param   in Pointer to existing dictionary entry
 */

gdcmElValue::gdcmElValue(gdcmDictEntry* in) {
	ImplicitVr = false;
	entry = in;
}
