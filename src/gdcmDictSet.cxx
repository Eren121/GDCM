// gdcmDictEntry
//-----------------------------------------------------------------------------
#include "gdcmDictSet.h"
#include "gdcmUtil.h"
#include <fstream>
#include <stdlib.h>  // For getenv

#define PUB_DICT_NAME     "DicomV3Dict"
#ifndef PUB_DICT_PATH
#  define PUB_DICT_PATH     "../Dicts/"
#endif
#define PUB_DICT_FILENAME "dicomV3.dic"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/** 
 * \ingroup gdcmDictSet
 * \brief   The Dictionnary Set obtained with this constructor simply
 *          contains the Default Public dictionnary.
 */
gdcmDictSet::gdcmDictSet(void) 
{
   DictPath = BuildDictPath();
   std::string PubDictFile = DictPath + PUB_DICT_FILENAME;
   Dicts[PUB_DICT_NAME] = new gdcmDict(PubDictFile);
}

/**
 * \ingroup gdcmDictSet
 * \brief  Destructor 
 */
gdcmDictSet::~gdcmDictSet() 
{
   // Remove dictionnaries
   for (DictSetHT::iterator tag = Dicts.begin(); tag != Dicts.end(); ++tag) 
   {
      gdcmDict *EntryToDelete = tag->second;
      if ( EntryToDelete )
         delete EntryToDelete;
      tag->second=NULL;
   }
   Dicts.clear();

   // Remove virtual dictionnary entries
   std::map<std::string,gdcmDictEntry *>::iterator it;
   for(it=virtualEntry.begin(); it!=virtualEntry.end(); ++it)
   {
      gdcmDictEntry *Entry = it->second;
      if ( Entry )
         delete Entry;
      it->second=NULL;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDictSet
 * \brief   Print, in an informal fashion, the list of all the dictionaries
 *          contained is this gdcmDictSet, along with their respective content.
 * @param   os Output stream used for printing.
 */
void gdcmDictSet::Print(std::ostream& os) 
{
   for (DictSetHT::iterator dict = Dicts.begin(); dict != Dicts.end(); ++dict)
   {
      os << "Printing dictionary " << dict->first << std::endl;
      dict->second->Print(os);
   }
}

//-----------------------------------------------------------------------------
// Public
/** 
 * \ingroup gdcmDictSet
 * \brief   Consider all the entries of the public dicom dictionnary. 
 *          Build all list of all the tag names of all those entries.
 * \sa      gdcmDictSet::GetPubDictTagNamesByCategory
 * @return  A list of all entries of the public dicom dictionnary.
 */
std::list<std::string> *gdcmDictSet::GetPubDictEntryNames(void) 
{
   return(GetDefaultPubDict()->GetDictEntryNames());
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
 *           than asking us to perform a line by line check of the dictionnary
 *           at the beginning of each year to -try to- guess the changes)
 *           Therefore : please NEVER use that fourth field :-(
 * *
 * @return  An hashtable: whose keys are the names of the groups and whose
 *          corresponding values are lists of all the dictionnary entries
 *          among that group.
 */
std::map<std::string, std::list<std::string> > *gdcmDictSet::GetPubDictEntryNamesByCategory(void) 
{
   return(GetDefaultPubDict()->GetDictEntryNamesByCategory());
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
void gdcmDictSet::LoadDictFromFile(std::string FileName, DictKey Name) 
{
   gdcmDict *NewDict = new gdcmDict(FileName);
   AppendDict(NewDict,Name);
}

/**
 * \ingroup gdcmDictSet
 * \brief   Retrieve the specified dictionary (when existing) from this
 *          gdcmDictSet.
 * @param   DictName The symbolic name of the searched dictionary.
 * \result  The retrieved dictionary.
 */
gdcmDict *gdcmDictSet::GetDict(DictKey DictName) 
{
   DictSetHT::iterator dict = Dicts.find(DictName);
   if(dict!=Dicts.end())
      return dict->second;
   return NULL;
}

/**
 * \ingroup gdcmDictSet
 * \brief   Retrieve the default reference DICOM V3 public dictionary.
 * \result  The retrieved default dictionary.
 */
gdcmDict *gdcmDictSet::GetDefaultPubDict() 
{
   return GetDict(PUB_DICT_NAME);
}

/**
 * \ingroup gdcmDictSet
 * \brief   Create a gdcmDictEntry which will be reference 
 *          in no dictionnary
 * @return  virtual entry
 */
gdcmDictEntry *gdcmDictSet::NewVirtualDictEntry(guint16 group, guint16 element,
                                                std::string vr,std::string fourth,
                                                std::string name)
{
   gdcmDictEntry *entry;
   std::string tag=gdcmDictEntry::TranslateToKey(group,element)+vr;
   std::map<std::string,gdcmDictEntry *>::iterator it;
   
   it=virtualEntry.find(tag);
   if(it!=virtualEntry.end())
   {
      entry=it->second;
   }
   else
   {
      entry=new gdcmDictEntry(group,element,vr,fourth,name);
      virtualEntry[tag]=entry;
   }
   return(entry);
}

/**
 * \ingroup gdcmDictSet
 * \brief   Obtain from the GDCM_DICT_PATH environnement variable the
 *          path to directory containing the dictionnaries. When
 *          the environnement variable is absent the path is defaulted
 *          to "../Dicts/".
 * @return  path to directory containing the dictionnaries
 */
std::string gdcmDictSet::BuildDictPath(void) 
{
   std::string ResultPath;
   const char *EnvPath = (char*)0;
   EnvPath = getenv("GDCM_DICT_PATH");
   if (EnvPath && (strlen(EnvPath) != 0)) 
   {
      ResultPath = EnvPath;
      if (ResultPath[ResultPath.length() -1] != '/' )
         ResultPath += '/';
      dbg.Verbose(1, "gdcmDictSet::BuildDictPath:",
                     "Dictionary path set from environnement");
   } 
   else
      ResultPath = PUB_DICT_PATH;
   return ResultPath;
}

//-----------------------------------------------------------------------------
// Protected
bool gdcmDictSet::AppendDict(gdcmDict *NewDict,DictKey Name)
{
   Dicts[Name] = NewDict;
   return(true);
}

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------


