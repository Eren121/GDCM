#include "gdcmlib.h"

void ElValue::SetVR(string ValRep) { entry->SetVR(ValRep); }

void ElValue::SetLgrLue(guint32 lgr) { LgrLueElem = lgr; }

ElValue::ElValue(gdcmDictEntry* in) { entry = in; }
