#include "gdcm.h"

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
