#include "gdcmlib.h"
#define DEBUG 1

void ElValue::SetVR(string ValRep) {
	if (!entry && DEBUG) {
		printf ("ElValue::SetVR: no known entry\n");
		return;
	}
	entry->SetVR(ValRep);
}

void ElValue::SetLgrLue(guint32 lgr) {
	LgrLueElem = lgr;
}
