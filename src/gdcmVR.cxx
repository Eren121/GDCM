// gdcmVR.cxx
//-----------------------------------------------------------------------------
#include <fstream>

#include <iostream>
#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif

#include "gdcmVR.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"

#ifndef PUB_DICT_PATH
#  define PUB_DICT_PATH     "../Dicts/"
#endif
#define DICT_VR "dicomVR.dic"

//-----------------------------------------------------------------------------
// Constructor / Destructor
gdcmVR::gdcmVR(void) 
{
   std::string filename=gdcmDictSet::BuildDictPath() + std::string(DICT_VR);
   std::ifstream from(filename.c_str());
   dbg.Error(!from, "gdcmVR::gdcmVR: can't open dictionary",filename.c_str());

   char buff[1024];
   std::string key;
   std::string name;

   while (!from.eof()) 
   {
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

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmVR
 * \brief   Print all 
 * @param   os The output stream to be written to.
 */
void gdcmVR::Print(std::ostream &os) 
{
   std::ostringstream s;

   for (VRHT::iterator it = vr.begin(); it != vr.end(); ++it)
   {
      s << "VR : "<<it->first<<" = "<<it->second<<std::endl;
   }
   os << s.str();
}

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup gdcmVR
 * \brief   Get the count for an element
 * @param   key key to count
 */
int gdcmVR::Count(VRKey key) 
{
   return vr.count(key);
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
