// gdcmDictEntry.h

#ifndef GDCMDICTENTRY_H
#define GDCMDICTENTRY_H

#include "gdcmCommon.h"

class GDCM_EXPORT gdcmDictEntry {
private:
   // FIXME : were are the group and element used except from building up
   //         a TagKey. If the answer is nowhere then there is no need
   //         to store the group and element independently.
	guint16 group;       // e.g. 0x0010
	guint16 element;     // e.g. 0x0103
   std::string  vr;     // Value Representation i.e. some clue about the nature
	                     // of the data represented e.g. "FD" short for
	                     // "Floating Point Double"
	// CLEANME: find the official dicom name for this field !
   std::string  fourth; // Fourth field containing some semantics.
                        //(Group Name abbr.)
   std::string  name;   // e.g. "Patient_Name"
	TagKey  key;         // Redundant with (group, element) but we add it
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
                 std::string vr     = "Unknown",
                 std::string fourth = "Unknown",
                 std::string name   = "Unknown");
	              	 
	// fabrique une 'clé' par concaténation du numGroupe et du numElement
	static TagKey TranslateToKey(guint16 group, guint16 element);
	
	guint16      GetGroup(void)  { return group; };
	guint16      GetElement(void){return element;};
   std::string  GetVR(void)     {return vr;     };
	void         SetVR(std::string);
	void         SetKey(std::string k){ key = k; };
	bool         IsVrUnknown(void);
   std::string  GetFourth(void) {return fourth;};
   std::string  GetName(void)   {return name;  };
   std::string  GetKey(void)    {return key;   };
};

#endif
