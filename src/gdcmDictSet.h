// gdcmDictSet.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICTSET_H
#define GDCMDICTSET_H

#include "gdcmDict.h"
#include <map>
#include <list>

typedef std::string DictKey;
typedef std::map<DictKey, gdcmDict*> DictSetHT;

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmDictSet
 * \brief  Container for managing a set of loaded dictionaries.
 * \note   Hopefully, sharing dictionaries should avoid
 * \par    reloading an already loaded dictionary (saving time)
 * \par    having many in memory representations of the same dictionary
 *        (saving memory).
 */
class GDCM_EXPORT gdcmDictSet {
public:
	// TODO Swig int LoadDictFromFile(std::string filename);
   // QUESTION: the following function might not be thread safe !? Maybe
   //           we need some mutex here, to avoid concurent creation of
   //           the same dictionary !?!?!
	// TODO Swig int LoadDictFromName(std::string filename);
	// TODO Swig int LoadAllDictFromDirectory(std::string DirectoryName);
	// TODO Swig std::string* GetAllDictNames();
   gdcmDictSet(void);
   ~gdcmDictSet(void);

   void Print(std::ostream& os);

   std::list<std::string> *GetPubDictEntryNames(void);
   std::map<std::string, std::list<std::string> > *
       GetPubDictEntryNamesByCategory(void);

   void LoadDictFromFile(std::string FileName, DictKey Name);

   gdcmDict *GetDict(DictKey DictName);
   gdcmDict *GetDefaultPubDict(void);

   gdcmDictEntry *NewVirtualDictEntry(guint16 group, guint16 element,
                                      std::string vr     = "Unknown",
                                      std::string fourth = "Unknown",
                                      std::string name   = "Unknown");

   static std::string BuildDictPath(void);

protected:
   bool AppendDict(gdcmDict *NewDict,DictKey Name);

private:
   /// Hash table of all dictionaries contained in this gdcmDictSet
   DictSetHT Dicts;
   /// Directory path to dictionaries
   std::string DictPath;

   std::map<std::string,gdcmDictEntry *> virtualEntry;
};

//-----------------------------------------------------------------------------
#endif
