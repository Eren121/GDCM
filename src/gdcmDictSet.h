// gdcmDictSet.h

#ifndef GDCMDICTSET_H
#define GDCMDICTSET_H

#include <map>
#include <list>
#include "gdcmDict.h"

typedef string DictKey;
typedef map<DictKey, gdcmDict*> DictSetHT;

/*
 * \defgroup gdcmDictSet
 * \brief  Container for managing a set of loaded dictionaries.
 * \note   Hopefully, sharing dictionaries should avoid
 * \par    reloading an allready loaded dictionary (saving time)
 * \par    having many in memory representations of the same dictionary
 *        (saving memory).
 */
class GDCM_EXPORT gdcmDictSet {
private:
   /// Hash table of all dictionaries contained in this gdcmDictSet
	DictSetHT Dicts;
   /// Directory path to dictionaries
	string DictPath;
	int AppendDict(gdcmDict* NewDict);
	void LoadDictFromFile(string filename, DictKey);
	string BuildDictPath(void);
public:
	list<string> * GetPubDictTagNames(void);
	map<string, list<string> >* GetPubDictTagNamesByCategory(void);

	// TODO Swig int LoadDictFromFile(string filename);
   // QUESTION: the following function might not be thread safe !? Maybe
   //           we need some mutex here, to avoid concurent creation of
   //           the same dictionary !?!?!
	// TODO Swig int LoadDictFromName(string filename);
	// TODO Swig int LoadAllDictFromDirectory(string DirectoryName);
	// TODO Swig string* GetAllDictNames();
	gdcmDictSet(void);
	~gdcmDictSet(void);
	void Print(ostream&);
	gdcmDict* GetDict(DictKey DictName);
	gdcmDict* GetDefaultPubDict(void);
};

#endif
