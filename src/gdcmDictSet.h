// gdcmDictSet.h

#ifndef GDCMDICTSET_H
#define GDCMDICTSET_H

#include <map>
#include <list>
#include "gdcmDict.h"

typedef std::string DictKey;
typedef std::map<DictKey, gdcmDict*> DictSetHT;

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
   std::string DictPath;
	int AppendDict(gdcmDict* NewDict);
	void LoadDictFromFile(std::string filename, DictKey);
   std::string BuildDictPath(void);
public:
   std::list<std::string> * GetPubDictTagNames(void);
   std::map<std::string, std::list<std::string> >*
       GetPubDictTagNamesByCategory(void);

	// TODO Swig int LoadDictFromFile(std::string filename);
   // QUESTION: the following function might not be thread safe !? Maybe
   //           we need some mutex here, to avoid concurent creation of
   //           the same dictionary !?!?!
	// TODO Swig int LoadDictFromName(std::string filename);
	// TODO Swig int LoadAllDictFromDirectory(std::string DirectoryName);
	// TODO Swig std::string* GetAllDictNames();
	gdcmDictSet(void);
	~gdcmDictSet(void);
	void Print(ostream&);
	gdcmDict* GetDict(DictKey DictName);
	gdcmDict* GetDefaultPubDict(void);
};

#endif
