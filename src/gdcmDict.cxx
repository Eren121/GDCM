#include <fstream>
#include "gdcmlib.h"
#include "gdcmUtil.h"

gdcmDict::gdcmDict(char * FileName) {
	std::ifstream from(FileName);
	dbg.Error(!from, "gdcmDictSet::gdcmDictSet:",
	          "can't open dictionary");
	guint16 group, element;
	// CLEANME : use defines for all those constants
	char buff[1024];
	char trash[10];
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

gdcmDictEntry * gdcmDict::GetTag(guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	TagHT::iterator found = entries.find(key);
	return found->second;
}
