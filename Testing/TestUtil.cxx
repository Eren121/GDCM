// This test should test everything in Util, since I didn't know any other 
// way to test this class.

#include "gdcm.h"

int TestUtil(int , char * [])
{
   std::cout << gdcm::Util::CreateUniqueUID("") << std::endl;
   
   const char ref[] = "MONOCHROME1";
   std::string a = "MONOCHROME1";
   a += '\0';
   std::string b = "MONOCHROME1 ";
   std::string c = gdcm::Util::DicomString("MONOCHROME1");
   std::string d = "MONOCHROME1";

   if( !gdcm::Util::DicomStringEqual(a,ref) ) return 1;
   if( !gdcm::Util::DicomStringEqual(b,ref) ) return 1;
   if( !gdcm::Util::DicomStringEqual(c,ref) ) return 1;
   if(  gdcm::Util::DicomStringEqual(d,ref) ) return 1;

   return 0;
}
