#include <fstream>
#include <stdlib.h>  // For getenv
#include "gdcm.h"
#include "gdcmUtil.h"

#define PUB_DICT_NAME     "DicomV3Dict"
#define PUB_DICT_PATH     "../Dicts/"
#define PUB_DICT_FILENAME "dicomV3.dic"

gdcmDictSet::gdcmDictSet(void) {
	SetDictPath();
	if (! LoadDicomV3Dict())
			  return;
}

void gdcmDictSet::SetDictPath(void) {
	const char* EnvPath = (char*)0;
	EnvPath = getenv("GDCM_DICT_PATH");
	if (EnvPath && (strlen(EnvPath) != 0))
		DictPath = EnvPath;
	else
		DictPath = PUB_DICT_PATH;
}

int gdcmDictSet::LoadDicomV3Dict(void) {
	if (dicts.count(PUB_DICT_NAME))
		return 1;
	return LoadDictFromFile(DictPath + PUB_DICT_FILENAME, PUB_DICT_NAME);
}

int gdcmDictSet::LoadDictFromFile(string FileName, DictKey Name) {
	gdcmDict *NewDict = new gdcmDict(FileName.c_str());
	dicts[Name] = NewDict;
	return 0;
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
	return GetDict(PUB_DICT_NAME);
}
