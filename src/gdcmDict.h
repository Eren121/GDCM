////////////////////////////////////////////////////////////////////////////
// A single DICOM dictionary i.e. a container for a collection of dictionary
// entries. There should be a single public dictionary (THE dictionary of
// the actual DICOM v3) but as many shadow dictionaries as imagers 
// combined with all software versions...

#ifndef GDCMDICT_H
#define GDCMDICT_H

#include <map>
#include "gdcmCommon.h"
#include "gdcmDictEntry.h"

typedef map<TagKey,  gdcmDictEntry*> TagKeyHT;
typedef map<TagName, gdcmDictEntry*> TagNameHT;

class GDCM_EXPORT gdcmDict {
	string name;
	string filename;
	TagKeyHT  KeyHt;                // Both accesses with a TagKey or with a
	TagNameHT NameHt;               // TagName are required.
public:
	gdcmDict(const char* FileName);   // Reads Dict from ascii file
	int AddNewEntry (gdcmDictEntry* NewEntry);
	int ReplaceEntry(gdcmDictEntry* NewEntry);
	int RemoveEntry (TagKey key);
	int RemoveEntry (guint16 group, guint16 element);
	gdcmDictEntry * GetTagByNumber(guint16 group, guint16 element);
	gdcmDictEntry * GetTagByName(TagName name);
	void Print(ostream&);
	void PrintByKey(ostream&);
	void PrintByName(ostream&);
	TagKeyHT & GetEntries(void) { return KeyHt; }
};

#endif
