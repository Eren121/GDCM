// gdcmDict.cxx
//-----------------------------------------------------------------------------
#include "gdcmDict.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"

#include <fstream>
#include <iostream>
#include <iomanip>

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Construtor
 * @param   FileName from which to build the dictionary.
 */
gdcmDict::gdcmDict(std::string & FileName) {
   guint16 group, element;
   char buff[1024];
   TagName vr;
   TagName fourth;
   TagName name;

   std::ifstream from(FileName.c_str());
   dbg.Error(!from, "gdcmDict::gdcmDict: can't open dictionary",
                    FileName.c_str());

   while (!from.eof()) {
      from >> std::hex;
      from >> group;          /// MEMORY LEAK in std::istream::operator>>
      from >> element;
      from >> vr;
      from >> fourth;
      getline(from, name);    /// MEMORY LEAK in std::getline<>

      gdcmDictEntry * newEntry = new gdcmDictEntry(group, element,
                                                   vr, fourth, name);
      AddNewEntry(newEntry);
   }
   from.close();

   filename=FileName;
}

/**
 * \brief  Destructor 
 */
gdcmDict::~gdcmDict() {
   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag) {
      gdcmDictEntry* EntryToDelete = tag->second;
      if ( EntryToDelete )
         delete EntryToDelete;
   }
   // Since AddNewEntry adds symetrical in both KeyHt and NameHT we can
   // assume all the pointed gdcmDictEntries are already cleaned-up when
   // we cleaned KeyHt.
   KeyHt.clear();
   NameHt.clear();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by tag i.e. the couple (group, element).
 * @param   os The output stream to be written to.
 */
void gdcmDict::Print(std::ostream &os) {
   os<<"Dict file name : "<<filename<<std::endl;
   PrintByKey(os);
}

/**
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by tag i.e. the couple (group, element).
 * @param   os The output stream to be written to.
 */
void gdcmDict::PrintByKey(std::ostream &os) {
   std::ostringstream s;

   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag){
      s << "Entry : ";
      s << "(" << std::hex << std::setw(4) << tag->second->GetGroup() << ',';
      s << std::hex << std::setw(4) << tag->second->GetElement() << ") = " << std::dec;
      s << tag->second->GetVR() << ", ";
      s << tag->second->GetFourth() << ", ";
      s << tag->second->GetName() << "."  << std::endl;
   }
   os << s.str();
}

/**
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by the name of the dictionary entries.
 * \warning AVOID USING IT : the name IS NOT an identifier; 
 *                           unpredictable result
 * @param   os The output stream to be written to.
 */
void gdcmDict::PrintByName(std::ostream& os) {
   std::ostringstream s;

   for (TagNameHT::iterator tag = NameHt.begin(); tag != NameHt.end(); ++tag){
      s << "Entry : ";
      s << tag->second->GetName() << ",";
      s << tag->second->GetVR() << ", ";
      s << tag->second->GetFourth() << ", ";
      s << "(" << std::hex << std::setw(4) << tag->second->GetGroup() << ',';
      s << std::hex << std::setw(4) << tag->second->GetElement() << ") = ";
      s << std::dec << std::endl;
   }
   os << s.str();
}

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup gdcmDict
 * \brief  adds a new Dicom Dictionary Entry 
 * @param   NewEntry entry to add 
 * @return  false if Dicom Element already exists
 */
bool gdcmDict::AddNewEntry(gdcmDictEntry *NewEntry) 
{
   TagKey key;
   key = NewEntry->GetKey();

   if(KeyHt.count(key) == 1)
   {
      dbg.Verbose(1, "gdcmDict::AddNewEntry already present", key.c_str());
      return(false);
   } 
   else 
   {
      KeyHt[NewEntry->GetKey()] = NewEntry;
      NameHt[NewEntry->GetName()] = NewEntry;  /// MEMORY LEAK in
                                               /// std::map<>::operator[]
      return(true);
   }
}

/**
 * \ingroup gdcmDict
 * \brief  replaces an already existing Dicom Element by a new one
 * @param   NewEntry new entry (overwrites any previous one with same tag)
 * @return  false if Dicom Element doesn't exist
 */
bool gdcmDict::ReplaceEntry(gdcmDictEntry *NewEntry) {
   if ( RemoveEntry(NewEntry->gdcmDictEntry::GetKey()) ) {
       KeyHt[NewEntry->GetKey()] = NewEntry;
       NameHt[NewEntry->GetName()] = NewEntry;
       return (true);
   } 
   return (false);
}

/**
 * \ingroup gdcmDict
 * \brief  removes an already existing Dicom Dictionary Entry,
 *         identified by its Tag
 * @param   key (group|element)
 * @return  false if Dicom Dictionary Entry doesn't exist
 */
bool gdcmDict::RemoveEntry(TagKey key) 
{
   if(KeyHt.count(key) == 1) 
   {
      gdcmDictEntry* EntryToDelete = KeyHt.find(key)->second;

      if ( EntryToDelete )
      {
         NameHt.erase(EntryToDelete->GetName());
         delete EntryToDelete;
      }

      KeyHt.erase(key);
      return (true);
   } 
   else 
   {
      dbg.Verbose(1, "gdcmDict::RemoveEntry unfound entry", key.c_str());
      return (false);
  }
}

/**
 * \ingroup gdcmDict
 * \brief  removes an already existing Dicom Dictionary Entry, 
 *          identified by its group,element number
 * @param   group   Dicom group number of the Dicom Element
 * @param   element Dicom element number of the Dicom Element
 * @return  false if Dicom Dictionary Entry doesn't exist
 */
bool gdcmDict::RemoveEntry (guint16 group, guint16 element) {
	return( RemoveEntry(gdcmDictEntry::TranslateToKey(group, element)) );
}

/**
 * \ingroup gdcmDict
 * \brief   Get the dictionnary entry identified by it's name.
 * @param   name element of the ElVal to modify
 * \warning : NEVER use it !
 *            the 'name' IS NOT an identifier within the Dicom Dicom Dictionary
 *            the name MAY CHANGE between two versions !
 * @return  the corresponding dictionnary entry when existing, NULL otherwise
 */
gdcmDictEntry *gdcmDict::GetDictEntryByName(TagName name) {
   if ( ! NameHt.count(name))
      return NULL; 
   return NameHt.find(name)->second;
}

/**
 * \ingroup gdcmDict
 * \brief   Get the dictionnary entry identified by a given tag (group,element)
 * @param   group   group of the entry to be found
 * @param   element element of the entry to be found
 * @return  the corresponding dictionnary entry when existing, NULL otherwise
 */
gdcmDictEntry *gdcmDict::GetDictEntryByNumber(guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! KeyHt.count(key))
      return NULL; 
   return KeyHt.find(key)->second;
}

/** 
 * \ingroup gdcmDict
 * \brief   Consider all the entries of the public dicom dictionnary. 
 *          Build all list of all the tag names of all those entries.
 * \sa      gdcmDictSet::GetPubDictTagNamesByCategory
 * @return  A list of all entries of the public dicom dictionnary.
 */
std::list<std::string> *gdcmDict::GetDictEntryNames(void) 
{
   std::list<std::string> *Result = new std::list<std::string>;
   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag)
   {
      Result->push_back( tag->second->GetName() );
   }
   return Result;
}

/** 
 * \ingroup gdcmDict
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
 *
 * @return  An hashtable: whose keys are the names of the groups and whose
 *          corresponding values are lists of all the dictionnary entries
 *          among that group.
 */
std::map<std::string, std::list<std::string> > *gdcmDict::GetDictEntryNamesByCategory(void) 
{
   std::map<std::string, std::list<std::string> > *Result = new std::map<std::string, std::list<std::string> >;

   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag)
   {
      (*Result)[tag->second->GetFourth()].push_back(tag->second->GetName());
   }
   return Result;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
