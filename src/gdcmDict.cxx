// gdcmDict.cxx

#include <fstream>
#include "gdcmDict.h"
#include "gdcmUtil.h"
using namespace std;

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
 * \brief   
 * @param   
 * @return  
 */
 gdcmDict::~gdcmDict() {
   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag) {
      gdcmDictEntry* EntryToDelete = tag->second;
      if ( EntryToDelete )
         delete EntryToDelete;
   }
   KeyHt.clear();
   // Since AddNewEntry adds symetrical in both KeyHt and NameHT we can
   // assume all the pointed gdcmDictEntries are allready cleaned-up when
   // we cleaned KeyHt.
   NameHt.clear();
}

/**
 * \ingroup gdcmDict
 * \brief   
 * @param   
 * @return  
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
   for (TagKeyHT::iterator tag = KeyHt.begin(); tag != KeyHt.end(); ++tag){
      os << "Tag : ";
      os << "(" << hex << tag->second->GetGroup() << ',';
      os << hex << tag->second->GetElement() << ") = " << dec;
      os << tag->second->GetVR() << ", ";
      os << tag->second->GetFourth() << ", ";
      os << tag->second->GetName() << "."  << endl;
   }
}

/**
 * \ingroup gdcmDict
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by the name of the dictionary entries.
 * @param   os The output stream to be written to.
 */
void gdcmDict::PrintByName(std::ostream& os) {
   for (TagNameHT::iterator tag = NameHt.begin(); tag != NameHt.end(); ++tag){
      os << "Tag : ";
      os << tag->second->GetName() << ",";
      os << tag->second->GetVR() << ", ";
      os << tag->second->GetFourth() << ", ";
      os << "(" << std::hex << tag->second->GetGroup() << ',';
      os << std::hex << tag->second->GetElement() << ") = " << dec << std::endl;
   }
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
 * @return  the corresponding dictionnary entry when existing, NULL otherwise
 */
gdcmDictEntry * gdcmDict::GetTagByName(TagName name) {
   if ( ! NameHt.count(name))
      return (gdcmDictEntry*)0; 
   return NameHt.find(name)->second;
}

/**
 * \ingroup gdcmDict
 * \brief   
 * @param   
 * @return  
 */
int gdcmDict::ReplaceEntry(gdcmDictEntry* NewEntry) {
   if ( RemoveEntry(NewEntry->gdcmDictEntry::GetKey()) ) {
       KeyHt[ NewEntry->GetKey()] = NewEntry;
       return (1);
   } 
   return (0);
}

/**
 * \ingroup gdcmDict
 * \brief   
 * @param   
 * @return  
 */
 int gdcmDict::AddNewEntry(gdcmDictEntry* NewEntry) {
   TagKey key;
   key = NewEntry->GetKey();
	
   if(KeyHt.count(key) == 1) {
      dbg.Verbose(1, "gdcmDict::AddNewEntry allready present", key.c_str());
      return(0);
   } else {
      KeyHt[NewEntry->GetKey()] = NewEntry;
      return(1);
   }
}

/**
 * \ingroup gdcmDict
 * \brief   
 * @param   
 * @return  
 */
int gdcmDict::RemoveEntry(TagKey key) {
   if(KeyHt.count(key) == 1) {
      gdcmDictEntry* EntryToDelete = KeyHt.find(key)->second;
      if ( EntryToDelete )
         delete EntryToDelete;
      KeyHt.erase(key);
      return (1);
   } else {
      dbg.Verbose(1, "gdcmDict::RemoveEntry unfound entry", key.c_str());
      return (0);
  }
}

/**
 * \ingroup gdcmDict
 * \brief   
 * @param   
 * @return  
 */
int gdcmDict::RemoveEntry (guint16 group, guint16 element) {
	return( RemoveEntry(gdcmDictEntry::TranslateToKey(group, element)) );
}

