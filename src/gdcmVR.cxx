// gdcmVR.cxx
#include <fstream>

#include "gdcmVR.h"
#include "gdcmUtil.h"

#ifndef PUB_DICT_PATH
#  define PUB_DICT_PATH     "../Dicts/"
#endif
#define DICT_VR "dicomVR.dic"

gdcmVR::gdcmVR(void) {
   std::string filename=gdcmDictSet::BuildDictPath() + std::string(DICT_VR);
   std::ifstream from(filename.c_str());
   dbg.Error(!from, "gdcmVR::gdcmVR: can't open dictionary",filename.c_str());

   char buff[1024];
   std::string key;
   std::string name;

   while (!from.eof()) {
      eatwhite(from);
      from.getline(buff, 1024, ' ');
      key = buff;
      eatwhite(from);
      from.getline(buff, 1024, ';');
      name = buff;

      eatwhite(from);
      from.getline(buff, 1024, '\n');

      if(key!="")
      {
         vr[key]=name;
      }
   }
   from.close();
}

gdcmVR::~gdcmVR() {
   vr.clear();
}

int gdcmVR::Count(VRKey key) {
   return vr.count(key);
}
