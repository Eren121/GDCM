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

/// Build a memory representation of a dicom dictionary by parsing
/// an ascii file
class GDCM_EXPORT gdcmDict {
	string name;
	string filename;
   /// Access through TagKey (see alternate access with NameHt)
	TagKeyHT  KeyHt;
   /// Access through TagName (see alternate access with KeyHt)
	TagNameHT NameHt;
public:
	gdcmDict(string & FileName);
	~gdcmDict();
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
