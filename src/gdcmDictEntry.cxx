// gdcmDictEntry.cxx

//This is needed when compiling in debug mode
#ifdef _MSC_VER
// 'identifier' : class 'type' needs to have dll-interface to be used by
// clients of class 'type2'
#pragma warning ( disable : 4251 )
// 'identifier' : identifier was truncated to 'number' characters in the
// debug information
#pragma warning ( disable : 4786 )
#endif //_MSC_VER

#include <stdio.h>    // FIXME For sprintf
#include "gdcmDictEntry.h"
#include "gdcmUtil.h"


gdcmDictEntry::gdcmDictEntry(guint16 InGroup, guint16 InElement,
									  std::string  InVr, std::string InFourth,
									  std::string  InName) {
	group 		= InGroup;
	element 	= InElement;
	vr 		= InVr;
	fourth 		= InFourth;
	name 		= InName;
	key 		= TranslateToKey(group, element);
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
void gdcmDictEntry::SetVR(std::string NewVr) {
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
