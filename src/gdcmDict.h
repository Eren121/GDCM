// gdcmDict.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICT_H
#define GDCMDICT_H

#include "gdcmCommon.h"
#include "gdcmDictEntry.h"

#include <iostream>
#include <list>
#include <map>

//-----------------------------------------------------------------------------
typedef std::map<TagKey,  gdcmDictEntry*> TagKeyHT;
typedef std::map<TagName, gdcmDictEntry*> TagNameHT;

//-----------------------------------------------------------------------------
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
public:
   gdcmDict(std::string & FileName);
	~gdcmDict();

// Print
	void Print(std::ostream &os = std::cout);
	void PrintByKey(std::ostream &os = std::cout);
	void PrintByName(std::ostream &os = std::cout);	

// Entries
   bool AddNewEntry (gdcmDictEntry *NewEntry);
	bool ReplaceEntry(gdcmDictEntry *NewEntry);
	bool RemoveEntry (TagKey key);
	bool RemoveEntry (guint16 group, guint16 element);

// Tag
	gdcmDictEntry *GetDictEntryByName(TagName name);
	gdcmDictEntry *GetDictEntryByNumber(guint16 group, guint16 element);

   std::list<std::string> *GetDictEntryNames(void);
   std::map<std::string, std::list<std::string> > *
        GetDictEntryNamesByCategory(void);

   /**
    * \ingroup gdcmDict
    * \brief   returns a ref to the Dicom Dictionary H table (map)
    * return the Dicom Dictionary H table
    */
   inline TagKeyHT & GetEntriesByKey(void)  { return KeyHt; }

   /**
    * \ingroup gdcmDict
    * \brief   returns a ref to the Dicom Dictionary H table (map)
    * return the Dicom Dictionary H table
    */
   inline TagNameHT & GetEntriesByName(void)  { return NameHt; }
 
private:
   /// ASCII file holding the Dictionnary
   std::string filename;
   /// Access through TagKey (see alternate access with NameHt)
	TagKeyHT  KeyHt;
   /// Access through TagName (see alternate access with KeyHt)
	TagNameHT NameHt;
};

//-----------------------------------------------------------------------------
#endif
