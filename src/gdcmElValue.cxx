// gdcmElValue.cxx

#include "gdcm.h"

void   ElValue::SetVR(string ValRep) { entry->SetVR(ValRep); }
string ElValue::GetVR(void)          { return entry->GetVR(); }

ElValue::ElValue(gdcmDictEntry* in) {
	ImplicitVr = false;
	entry = in;
}
