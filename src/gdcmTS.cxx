// gdcmTS.cxx

#include <fstream>

#include "gdcmTS.h"
#include "gdcmUtil.h"

#ifndef PUB_DICT_PATH
#  define PUB_DICT_PATH     "../Dicts/"
#endif
#define DICT_TS "dicomTS.dic"

gdcmTS::gdcmTS(void) {
   std::string filename=gdcmDictSet::BuildDictPath() + std::string(DICT_TS);
   std::ifstream from(filename.c_str());
   dbg.Error(!from, "gdcmTS::gdcmTS: can't open dictionary",filename.c_str());

   char buff[1024];
   std::string key;
   std::string name;

   while (!from.eof()) {
      eatwhite(from);
      from.getline(buff, 1024, ' ');
      key = buff;
      eatwhite(from);
      from.getline(buff, 1024, '\n');
      name = buff;

      if(key!="")
      {
         ts[key]=name;
      }
   }
   from.close();
}

gdcmTS::~gdcmTS() {
   ts.clear();
}

int gdcmTS::Count(TSKey key) {
   return ts.count(key);
}

std::string gdcmTS::GetValue(TSKey key) {
   if (ts.count(key) == 0) 
      return (GDCM_UNFOUND);
   return ts[key];
}
