// gdcmElValue.cxx

#include "gdcm.h"

	// void    ElValue::SetVR(string v)	 { entry->SetVR(v);       };

ElValue::ElValue(gdcmDictEntry* in) {
	ImplicitVr = false;
	entry = in;
}


