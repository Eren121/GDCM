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
