// gdcmDictEntry

#ifdef _MSC_VER
//'identifier' : decorated name length exceeded, name was truncated
//#pragma warning ( disable : 4503 )
// 'identifier' : class 'type' needs to have dll-interface to be used by
// clients of class 'type2'
#pragma warning ( disable : 4251 )
// 'identifier' : identifier was truncated to 'number' characters in the
// debug information
#pragma warning ( disable : 4786 )
#endif //_MSC_VER

#include <fstream>
#include <stdlib.h>  // For getenv
#include "gdcmUtil.h"
#include "gdcmDictSet.h"

#define PUB_DICT_NAME     "DicomV3Dict"
#ifndef PUB_DICT_PATH
#  define PUB_DICT_PATH     "../Dicts/"
#endif
#define PUB_DICT_FILENAME "dicomV3.dic"

/** 
 * \ingroup gdcmDictSet
 * \brief   Consider all the entries of the public dicom dictionnary. 
 *          Build all list of all the tag names of all those entries.
 * \sa      gdcmDictSet::GetPubDictTagNamesByCategory
 * @return  A list of all entries of the public dicom dictionnary.
 */
std::list<std::string> * gdcmDictSet::GetPubDictTagNames(void) {
   std::list<std::string> * Result = new std::list<std::string>;
   TagKeyHT entries = GetDefaultPubDict()->GetEntries();
   
   for (TagKeyHT::iterator tag = entries.begin(); tag != entries.end(); ++tag){
      Result->push_back( tag->second->GetName() );
   }
   return Result;
}

/** 
 * \ingroup gdcmDictSet
 * \brief   Consider all the entries of the public dicom dictionnary.
 *          Build an hashtable whose keys are the names of the groups
 *          (fourth field in each line of dictionary) and whose corresponding
 *          values are lists of all the dictionnary entries among that
 *          group. Note that apparently the Dicom standard doesn't explicitely
 *          define a name (as a string) for each group.
 *          A typical usage of this method would be to enable a dynamic
 *          configuration of a Dicom file browser: the admin/user can
 *          select in the interface which Dicom tags should be displayed.
 * \warning Dicom *doesn't* define any name for any 'categorie'
 *          (the dictionnary fourth field was formerly NIH defined
 *           - and no longer he is-
 *           and will be removed when Dicom provides us a text file
 *           with the 'official' Dictionnary, that would be more friendly
 *           than asking us to perform a line by line check od thhe dictionnary
 *           at the beginning of each year to -try to- guess the changes)
 *           Therefore : please NEVER use that fourth field :-(
 *
 * @return  An hashtable: whose keys are the names of the groups and whose
 *          corresponding values are lists of all the dictionnary entries
 *          among that group.
 */
std::map<std::string, std::list<std::string> > * gdcmDictSet::GetPubDictTagNamesByCategory(void) {
   std::map<std::string, std::list<std::string> > * Result = new std::map<std::string, std::list<std::string> >;
   TagKeyHT entries = GetDefaultPubDict()->GetEntries();

   for (TagKeyHT::iterator tag = entries.begin(); tag != entries.end(); ++tag){
      (*Result)[tag->second->GetFourth()].push_back(tag->second->GetName());
   }
   return Result;
}

/**
 * \ingroup gdcmDictSet
 * \brief   Obtain from the GDCM_DICT_PATH environnement variable the
 *          path to directory containing the dictionnaries. When
 *          the environnement variable is absent the path is defaulted
 *          to "../Dicts/".
 */
std::string gdcmDictSet::BuildDictPath(void) {
   std::string ResultPath;
   const char* EnvPath = (char*)0;
   EnvPath = getenv("GDCM_DICT_PATH");
   if (EnvPath && (strlen(EnvPath) != 0)) {
      ResultPath = EnvPath;
      if (ResultPath[ResultPath.length() -1] != '/' )
         ResultPath += '/';
      dbg.Verbose(1, "gdcmDictSet::BuildDictPath:",
                     "Dictionary path set from environnement");
   } else
      ResultPath = PUB_DICT_PATH;
   return ResultPath;
}

/** 
 * \ingroup gdcmDictSet
 * \brief   The Dictionnary Set obtained with this constructor simply
 *          contains the Default Public dictionnary.
 */
gdcmDictSet::gdcmDictSet(void) {
   DictPath = BuildDictPath();
   std::string PubDictFile = DictPath + PUB_DICT_FILENAME;
   Dicts[PUB_DICT_NAME] = new gdcmDict(PubDictFile);
}

gdcmDictSet::~gdcmDictSet() {
   for (DictSetHT::iterator tag = Dicts.begin(); tag != Dicts.end(); ++tag) {
      gdcmDict* EntryToDelete = tag->second;
      if ( EntryToDelete )
         delete EntryToDelete;
   }
   Dicts.clear();
}

/**
 * \ingroup gdcmDictSet
 * \brief   Loads a dictionary from a specified file, and add it
 *          to already the existing ones contained in this gdcmDictSet.
 * @param   FileName Absolute or relative filename containing the
 *          dictionary to load.
 * @param   Name Symbolic name that be used as identifier of the newly 
 *          created dictionary.
 */
void gdcmDictSet::LoadDictFromFile(std::string FileName, DictKey Name) {
   gdcmDict *NewDict = new gdcmDict(FileName);
   Dicts[Name] = NewDict;
}

/**
 * \ingroup gdcmDictSet
 * \brief   Print, in an informal fashion, the list of all the dictionaries
 *          contained is this gdcmDictSet, along with their respective content.
 * @param   os Output stream used for printing.
 */
void gdcmDictSet::Print(std::ostream& os) {
   for (DictSetHT::iterator dict = Dicts.begin(); dict != Dicts.end(); ++dict){
      os << "Printing dictionary " << dict->first << std::endl;
      dict->second->Print(os);
   }
}

/**
 * \ingroup gdcmDictSet
 * \brief   Retrieve the specified dictionary (when existing) from this
 *          gdcmDictSet.
 * @param   DictName The synbolic name of the searched dictionary.
 * \result  The retrieved dictionary.
 */
gdcmDict * gdcmDictSet::GetDict(DictKey DictName) {
   DictSetHT::iterator dict = Dicts.find(DictName);
   return dict->second;
}

/**
 * \ingroup gdcmDictSet
 * \brief   Retrieve the default reference DICOM V3 public dictionary.
 * \result  The retrieved default dictionary.
 */
gdcmDict * gdcmDictSet::GetDefaultPubDict() {
   return GetDict(PUB_DICT_NAME);
}
