// The following crashes on Win32
#include "gdcm.h"

int bug1(int argc, char* argv[])
{  
   gdcmHeader* e1;
	
   if (argc > 1)
      e1 = new gdcmHeader(argv[1]);
   else
      {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/test.acr";
      e1 = new gdcmHeader( filename.c_str() );
   }
   e1->PrintPubDict();

  return 0;
}
