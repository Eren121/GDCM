// gdcmDictSet.h

#ifndef GDCMDICTSET_H
#define GDCMDICTSET_H

#include <map>
#include <list>
#include "gdcmCommon.h"
#include "gdcmDict.h"

////////////////////////////////////////////////////////////////////////////
// Container for managing a set of loaded dictionaries. Sharing dictionaries
// should avoid :
// * reloading an allready loaded dictionary,
// * having many in memory representations of the same dictionary.

typedef string DictKey;
typedef map<DictKey, gdcmDict*> DictSetHT;

class GDCM_EXPORT gdcmDictSet {
private:
	DictSetHT dicts;
	int AppendDict(gdcmDict* NewDict);
	void LoadDictFromFile(string filename, DictKey);
private:
	static string DictPath;      // Directory path to dictionaries
	static string BuildDictPath(void);
	static gdcmDict* DefaultPubDict;
public:
	static list<string> * GetPubDictTagNames(void);
	static map<string, list<string> >* GetPubDictTagNamesByCategory(void);
	static gdcmDict* LoadDefaultPubDict(void);

	gdcmDictSet(void);
	// TODO Swig int LoadDictFromFile(string filename);
   // QUESTION: the following function might not be thread safe !? Maybe
   //           we need some mutex here, to avoid concurent creation of
   //           the same dictionary !?!?!
	// TODO Swig int LoadDictFromName(string filename);
	// TODO Swig int LoadAllDictFromDirectory(string DirectoryName);
	// TODO Swig string* GetAllDictNames();
	//
	void Print(ostream&);
	gdcmDict* GetDict(DictKey DictName);
	gdcmDict* GetDefaultPubDict(void);
};

#endif
