#include "gdcm.h"

void   ElValue::SetVR(string ValRep) { entry->SetVR(ValRep); }
string ElValue::GetVR(void)   { return entry->GetVR(); }

ElValue::ElValue(gdcmDictEntry* in) { entry = in; }
