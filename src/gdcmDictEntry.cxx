// gdcmDict.cxx

#include "gdcm.h"
#include "gdcmUtil.h"

gdcmDictEntry::gdcmDictEntry(guint16 InGroup, guint16 InElement,
                             string  InVr, string InFourth, string InName) 
{
	group = InGroup;
	element = InElement;
	vr = InVr;
	fourth = InFourth;
	name = InName;
	key = TranslateToKey(group, element);
}

TagKey gdcmDictEntry::TranslateToKey(guint16 group, guint16 element) {
	char trash[10];
	TagKey key;
	// CLEAN ME: better call the iostream<< with the hex manipulator on.
	// This requires some reading of the stdlibC++ sources to make the
	// proper call (or copy).
	sprintf(trash, "%04x|%04x", group , element);
	key = trash;  // Convertion through assignement
	return key;
}

/**
 * \ingroup     gdcmDictEntry
 * \brief       If-and only if-the vr is unset then overwrite it.
 * @param NewVr New vr to be set.
 */
void gdcmDictEntry::SetVR(string NewVr) {
	if ( IsVrUnknown() )
		vr = NewVr;
	else {
		dbg.Error(true, "gdcmDictEntry::SetVR",
		          "Overwriting vr might compromise a dictionary");
	}
}

bool gdcmDictEntry::IsVrUnknown() {
	if ( vr == "Unknown" )
		return true;
	return false;
}
