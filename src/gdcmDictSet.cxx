#include <fstream>
#include "gdcm.h"
#include "gdcmUtil.h"


gdcmDictSet::gdcmDictSet(void) {
	if (! LoadDicomV3Dict())
			  return;
}

int gdcmDictSet::LoadDicomV3Dict(void) {
	if (dicts.count(PUBDICTNAME))
		return 1;
	return LoadDictFromFile(PUBDICTFILENAME, PUBDICTNAME);
}

int gdcmDictSet::LoadDictFromFile(char * FileName, DictKey Name) {
	gdcmDict *NewDict = new gdcmDict(FileName);
	dicts[Name] = NewDict; 
}

void gdcmDictSet::Print(ostream& os) {
	for (DictSetHT::iterator dict = dicts.begin(); dict != dicts.end(); ++dict){
		os << "Printing dictionary " << dict->first << " \n";
		dict->second->Print(os);
	}
}

gdcmDict * gdcmDictSet::GetDict(DictKey DictName) {
	DictSetHT::iterator dict = dicts.find(DictName);
	return dict->second;
}

gdcmDict * gdcmDictSet::GetDefaultPublicDict() {
	return GetDict(PUBDICTNAME);
}
