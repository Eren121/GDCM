// gdcmDictEntry.h

#ifndef GDCMDICTENTRY_H
#define GDCMDICTENTRY_H

#include "gdcmCommon.h"

class GDCM_EXPORT gdcmDictEntry {
private:
	guint16 group;    // e.g. 0x0010  // FIXME : s'en sert-on qq part
	guint16 element;  // e.g. 0x0103  // si ce n'est pour fabriquer la TagKey ?
	string  vr;       // Value Representation i.e. some clue about the nature
	                  // of the data represented e.g. "FD" short for
	                  // "Floating Point Double"
	// CLEANME: find the official dicom name for this field !
	string  fourth;   // Fourth field containing some semantics. (Group Name abbr.)
	string  name;     // e.g. "Patient_Name"
	TagKey  key;      // Redundant with (group, element) but we add it
	                  // on efficiency purposes.
	// DCMTK has many fields for handling a DictEntry (see below). What are the
	// relevant ones for gdcmlib ?
	//      struct DBI_SimpleEntry {
	//         Uint16 upperGroup;
	//         Uint16 upperElement;
	//         DcmEVR evr;
	//         const char* tagName;
	//         int vmMin;
	//         int vmMax;
	//         const char* standardVersion;
	//         DcmDictRangeRestriction groupRestriction;
	//         DcmDictRangeRestriction elementRestriction;
	//       };
public:
	gdcmDictEntry(guint16 group, 
	              guint16 element,
	              string vr     = "Unknown",
	              string fourth = "Unknown",
	              string name   = "Unknown");
	              	 
	// fabrique une 'clé' par concaténation du numGroupe et du numElement
	static TagKey TranslateToKey(guint16 group, guint16 element);
	
	guint16 GetGroup(void)  { return group; };
	guint16 GetElement(void){return element;};
	string  GetVR(void)     {return vr;     };
	void    SetVR(string);
	void    SetKey(string k){ key = k;     }
	bool    IsVrUnknown(void);
	string  GetFourth(void) {return fourth;};
	string  GetName(void)   {return name;  };
	string  GetKey(void)    {return key;   };
};

#endif
