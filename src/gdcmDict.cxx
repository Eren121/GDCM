// gdcmDict.cxx

#include "gdcmDict.h"
#include "gdcmUtil.h"
#include <fstream>
#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif

/**
 * \ingroup gdcmDict
 * \brief   Construtor
 * @param   FileName from which to build the dictionary.
 */
gdcmDict::gdcmDict(std::string & FileName) {
   std::ifstream from(FileName.c_str());
   dbg.Error(!from, "gdcmDict::gdcmDict: can't open dictionary",
                    FileName.c_str());
   guint16 group, element;
	// CLEANME : use defines for all those constants
   char buff[1024];
   TagKey key;
   TagName vr;
   TagName fourth;
   TagName name;

   while (!from.eof()) {
      from >> std::hex >> group >> element;
      eatwhite(from);
      from.getline(buff, 256, ' ');
      vr = buff;
      eatwhite(from);
      from.getline(buff, 256, ' ');
      fourth = buff;
      from.getline(buff, 256, '\n');
      name = buff;
      gdcmDictEntry * newEntry = new gdcmDictEntry(group, element,
		                                   vr, fourth, name);
      // FIXME: use AddNewEntry
      NameHt[name] = newEntry;
      KeyHt[gdcmDictEntry::TranslateToKey(group, element)] = newEntry;
   }
   from.close();
}

/**
 * \ingroup gdcmDict
 * \brief  Destructor 
 */
gdcmDict::~gdcmDict() {
   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag) {
      gdcmDictEntry* EntryToDelete = tag->second;
      if ( EntryToDelete )
         delete EntryToDelete;
   }
   KeyHt.clear();
   // Since AddNewEntry adds symetrical in both KeyHt and NameHT we can
   // assume all the pointed gdcmDictEntries are already cleaned-up when
   // we cleaned KeyHt.
   NameHt.clear();
}

/**
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by tag i.e. the couple (group, element).
 * @param   os The output stream to be written to.
 */
void gdcmDict::Print(std::ostream& os) {
   PrintByKey(os);
}

/**
 * \ingroup gdcmDict
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by tag i.e. the couple (group, element).
 * @param   os The output stream to be written to.
 */
void gdcmDict::PrintByKey(std::ostream& os) {
   std::ostringstream s;

   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag){
      s << "Tag : ";
      s << "(" << std::hex << tag->second->GetGroup() << ',';
      s << std::hex << tag->second->GetElement() << ") = " << std::dec;
      s << tag->second->GetVR() << ", ";
      s << tag->second->GetFourth() << ", ";
      s << tag->second->GetName() << "."  << std::endl;
   }
   os << s.str();
}

/**
 * \ingroup gdcmDict
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by the name of the dictionary entries.
 * \warning AVOID USING IT : the name IS NOT an identifier
 *                           unpredictable result
 * @param   os The output stream to be written to.
 */
void gdcmDict::PrintByName(std::ostream& os) {
   std::ostringstream s;

   for (TagNameHT::iterator tag = NameHt.begin(); tag != NameHt.end(); ++tag){
      s << "Tag : ";
      s << tag->second->GetName() << ",";
      s << tag->second->GetVR() << ", ";
      s << tag->second->GetFourth() << ", ";
      s << "(" << std::hex << tag->second->GetGroup() << ',';
      s << std::hex << tag->second->GetElement() << ") = " << std::dec << std::endl;
   }
   os << s.str();
}

/**
 * \ingroup gdcmDict
 * \brief   Get the dictionnary entry identified by a given tag (group,element)
 * @param   group   group of the entry to be found
 * @param   element element of the entry to be found
 * @return  the corresponding dictionnary entry when existing, NULL otherwise
 */
gdcmDictEntry * gdcmDict::GetTagByNumber(guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! KeyHt.count(key))
      return (gdcmDictEntry*)0; 
   return KeyHt.find(key)->second;
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
gdcmDictEntry * gdcmDict::GetTagByName(TagName name) {
   if ( ! NameHt.count(name))
      return (gdcmDictEntry*)0; 
   return NameHt.find(name)->second;
}

/**
 * \ingroup gdcmDict
 * \brief  replaces an already existing Dicom Element by a new one
 * @param   NewEntry
 * @return  false if Dicom Element doesn't exist
 */
bool gdcmDict::ReplaceEntry(gdcmDictEntry* NewEntry) {
   if ( RemoveEntry(NewEntry->gdcmDictEntry::GetKey()) ) {
       KeyHt[ NewEntry->GetKey()] = NewEntry;
       return (true);
   } 
   return (false);
}

/**
 * \ingroup gdcmDict
 * \brief  adds a new Dicom Dictionary Entry 
 * @param   NewEntry 
 * @return  false if Dicom Element already existed
 */
 bool gdcmDict::AddNewEntry(gdcmDictEntry* NewEntry) {
   TagKey key;
   key = NewEntry->GetKey();
	
   if(KeyHt.count(key) == 1) {
      dbg.Verbose(1, "gdcmDict::AddNewEntry already present", key.c_str());
      return(false);
   } else {
      KeyHt[NewEntry->GetKey()] = NewEntry;
      return(true);
   }
}

/**
 * \ingroup gdcmDict
 * \brief  removes an already existing Dicom Dictionary Entry,
 *         identified by its Tag
 * @param   key (group|element)
 * @return  false if Dicom Dictionary Entry doesn't exist
 */
bool gdcmDict::RemoveEntry(TagKey key) {
   if(KeyHt.count(key) == 1) {
      gdcmDictEntry* EntryToDelete = KeyHt.find(key)->second;
      if ( EntryToDelete )
         delete EntryToDelete;
      KeyHt.erase(key);
      return (true);
   } else {
      dbg.Verbose(1, "gdcmDict::RemoveEntry unfound entry", key.c_str());
      return (false);
  }
}

/**
 * \ingroup gdcmDict
 * \brief  removes an already existing Dicom Dictionary Entry, 
 *          identified by its group,element
 number
 * @param   group   Dicom group number of the Dicom Element
 * @param   element Dicom element number of the Dicom Element
 * @return  false if Dicom Dictionary Entry doesn't exist
 */
bool gdcmDict::RemoveEntry (guint16 group, guint16 element) {
	return( RemoveEntry(gdcmDictEntry::TranslateToKey(group, element)) );
}

