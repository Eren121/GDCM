#include "gdcm.h"

gdcmFile::gdcmFile(string & filename)
	:gdcmHeader(filename.c_str())
{
	
}
