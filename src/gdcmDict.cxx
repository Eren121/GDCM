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
	TagKey key, vr, fourth, name;
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
		entries[gdcmDictEntry::TranslateToKey(group, element)] = newEntry;
	}
   from.close();
}

void gdcmDict::Print(ostream& os) {
	for (TagHT::iterator tag = entries.begin(); tag != entries.end(); ++tag){
       os << "Tag : ";
       os << "(" << hex << tag->second->GetGroup() << ',';
       os << hex << tag->second->GetElement() << ") = " << dec;
       os << tag->second->GetVR() << ", ";
       os << tag->second->GetFourth() << ", ";
       os << tag->second->GetName() << "."  << endl;
    }
}

// renvoie une ligne de Dictionnaire Dicom à partir de (numGroup, numElement)

gdcmDictEntry * gdcmDict::GetTag(guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! entries.count(key))
		return (gdcmDictEntry*)0; 
	if (entries.count(key) > 1)
		dbg.Verbose(0, "gdcmDict::GetTag", 
		            "multiple entries for this key (FIXME) !");
	return entries.find(key)->second;
}


int gdcmDict::ReplaceEntry(gdcmDictEntry* NewEntry) {

	// au cas ou la NewEntry serait incomplete
	// Question : cela peut-il se produire ?
	//
	TagKey key;
	key = NewEntry->GetKey();
	if (key =="") {
		NewEntry->gdcmDictEntry::SetKey(
				gdcmDictEntry::TranslateToKey(NewEntry->GetGroup(), NewEntry->GetElement())
				);
	}
	
	entries.erase (NewEntry->gdcmDictEntry::GetKey());
	entries[ NewEntry->GetKey()] = NewEntry;
	return (1);
	// Question : Dans quel cas ça peut planter ?
}
 

int gdcmDict::AddNewEntry(gdcmDictEntry* NewEntry) {

	// au cas ou la NewEntry serait incomplete
	// Question : cela peut-il se produire ?
	//
	
	TagKey key;
	key = NewEntry->GetKey();
	if (key =="") {
		NewEntry->SetKey(
				gdcmDictEntry::TranslateToKey(NewEntry->GetGroup(), NewEntry->GetElement())
				);
	}

	if(entries.count(key) >= 1) {
		printf("gdcmDict::AddNewEntry %s deja present\n", key.c_str());
		return(0);
	} else {
		entries[NewEntry->GetKey()] = NewEntry;
		return(1);
	}
	}


int gdcmDict::RemoveEntry(TagKey key) {	
	if(entries.count(key) == 1) {
		entries.erase(key);
		return (1);
	} else {
		printf("gdcmDict::RemoveEntry %s non trouve\n", key.c_str());
		return (0);
	}
}


int gdcmDict::RemoveEntry (guint16 group, guint16 element) {

	return( RemoveEntry(gdcmDictEntry::TranslateToKey(group, element)) );
}

