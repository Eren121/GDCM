// gdcmDict.h

#ifndef GDCMDICT_H
#define GDCMDICT_H

#include "gdcmCommon.h"
#include "gdcmDictEntry.h"
#include <map>

typedef std::map<TagKey,  gdcmDictEntry*> TagKeyHT;
typedef std::map<TagName, gdcmDictEntry*> TagNameHT;

/*
 * \defgroup gdcmDict
 * \brief    gdcmDict acts a memory representation of a dicom dictionary i.e.
 *           it is a container for a collection of dictionary entries. The
 *           dictionary is loaded from in an ascii file.
 *           There should be a single public dictionary (THE dictionary of
 *           the actual DICOM v3) but as many shadow dictionaries as imagers 
 *           combined with all software versions...
 * \see gdcmDictSet
 */
class GDCM_EXPORT gdcmDict {
   std::string name;
   std::string filename;
   /// Access through TagKey (see alternate access with NameHt)
	TagKeyHT  KeyHt;
   /// Access through TagName (see alternate access with KeyHt)
	TagNameHT NameHt;
public:
	gdcmDict(std::string & FileName);
	~gdcmDict();
	bool AddNewEntry (gdcmDictEntry* NewEntry);
	bool ReplaceEntry(gdcmDictEntry* NewEntry);
	bool RemoveEntry (TagKey key);
	bool RemoveEntry (guint16 group, guint16 element);
	gdcmDictEntry * GetTagByNumber(guint16 group, guint16 element);
	gdcmDictEntry * GetTagByName(TagName name);
	void Print(std::ostream&);
	void PrintByKey(std::ostream&);
	void PrintByName(std::ostream&);	

 /**
 * \ingroup gdcmDict
 * \brief   returns a ref to the Dicom Dictionary H table (map)
 * return the Dicom Dictionary H table
 */
 inline TagKeyHT &      gdcmDict::GetEntries(void)  {
    return KeyHt; 
 }
 
};

#endif
