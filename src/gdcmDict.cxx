// gdcmDict.cxx

#include <fstream>
#include "gdcm.h"
#include "gdcmUtil.h"

gdcmDict::gdcmDict(const char* FileName) {
	std::ifstream from(FileName);
	dbg.Error(!from, "gdcmDict::gdcmDict: can't open dictionary", FileName);
	guint16 group, element;
	// CLEANME : use defines for all those constants
	char buff[1024];
	TagKey key;
	TagName vr;
	TagName fourth;
	TagName name;
	while (!from.eof()) {
		from >> hex >> group >> element;
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
		NameHt[name] = newEntry;
		KeyHt[gdcmDictEntry::TranslateToKey(group, element)] = newEntry;
	}
	from.close();
}

void gdcmDict::Print(ostream& os) {
	PrintByKey(os);
}

/**
 * \ingroup gdcmHeader
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by tag i.e. the couple (group, element).
 * @param   os The output stream to be written to.
 */
void gdcmDict::PrintByKey(ostream& os) {
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
 * \ingroup gdcmHeader
 * \brief   Print all the dictionary entries contained in this dictionary.
 *          Entries will be sorted by the name of the dictionary entries.
 * @param   os The output stream to be written to.
 */
void gdcmDict::PrintByName(ostream& os) {
	for (TagNameHT::iterator tag = NameHt.begin(); tag != NameHt.end(); ++tag){
		os << "Tag : ";
		os << tag->second->GetName() << ",";
		os << tag->second->GetVR() << ", ";
		os << tag->second->GetFourth() << ", ";
		os << "(" << hex << tag->second->GetGroup() << ',';
		os << hex << tag->second->GetElement() << ") = " << dec << endl;
	}
}

/**
 * \ingroup gdcmHeader
 * \brief   Get the dictionnary entry identified by a given tag (group,element)
 * @param   group   group of the entry to be found
 * @param   element element of the entry to be found
 * @return  the corresponding dictionnary entry when existing, NULL otherwise
 */
gdcmDictEntry * gdcmDict::GetTagByKey(guint16 group, guint16 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! KeyHt.count(key))
		return (gdcmDictEntry*)0; 
	if (KeyHt.count(key) > 1)
		dbg.Verbose(0, "gdcmDict::GetTagByName", 
		            "multiple entries for this key (FIXME) !");
	return KeyHt.find(key)->second;
}

/**
 * \ingroup gdcmHeader
 * \brief   Get the dictionnary entry identified by it's name.
 * @param   name element of the ElVal to modify
 * @return  the corresponding dictionnary entry when existing, NULL otherwise
 */
gdcmDictEntry * gdcmDict::GetTagByName(TagName name) {
	if ( ! NameHt.count(name))
		return (gdcmDictEntry*)0; 
	if (NameHt.count(name) > 1)
		dbg.Verbose(0, "gdcmDict::GetTagByName", 
		            "multiple entries for this key (FIXME) !");
	return NameHt.find(name)->second;
}


int gdcmDict::ReplaceEntry(gdcmDictEntry* NewEntry) {
	//JPRCLEAN
	// au cas ou la NewEntry serait incomplete
	// Question : cela peut-il se produire ?
	//
	// --> NON : voir constructeur
	//TagKey key;
	//key = NewEntry->GetKey();
	//if (key =="") {
	//	NewEntry->gdcmDictEntry::SetKey(
	//			gdcmDictEntry::TranslateToKey(NewEntry->GetGroup(), NewEntry->GetElement())
	//			);
	//}
	
	KeyHt.erase (NewEntry->gdcmDictEntry::GetKey());
	KeyHt[ NewEntry->GetKey()] = NewEntry;
	return (1);
	// Question(jpr): Dans quel cas ça peut planter ?
	// Reponse(frog): dans les mauvais cas...
}
 

int gdcmDict::AddNewEntry(gdcmDictEntry* NewEntry) {

	TagKey key;
	key = NewEntry->GetKey();
	
	if(KeyHt.count(key) >= 1) {
		printf("gdcmDict::AddNewEntry %s deja present\n", key.c_str());
		return(0);
	} else {
		KeyHt[NewEntry->GetKey()] = NewEntry;
		return(1);
	}
	}


int gdcmDict::RemoveEntry(TagKey key) {	
	if(KeyHt.count(key) == 1) {
		KeyHt.erase(key);
		return (1);
	} else {
		printf("gdcmDict::RemoveEntry %s non trouve\n", key.c_str());
		return (0);
	}
}


int gdcmDict::RemoveEntry (guint16 group, guint16 element) {

	return( RemoveEntry(gdcmDictEntry::TranslateToKey(group, element)) );
}

