// The following crashes on Win32
#include "gdcm.h"

int main(int argc, char* argv[])
{  
	gdcmHeader* e1;
	
	if (argc > 1)
		e1 = new gdcmHeader(argv[1]);
	else
		e1 = new gdcmHeader("test.acr");
	e1->PrintPubDict();

  return 0;
}



