//This is needed when compiling in debug mode
#ifdef _MSC_VER
// 'identifier' : class 'type' needs to have dll-interface to be used by
// clients of class 'type2'
#pragma warning ( disable : 4251 )
// 'identifier' : identifier was truncated to 'number' characters in the
// debug information
#pragma warning ( disable : 4786 )
#endif //_MSC_VER
 
 
 /* -------------------------------------------------------------------- */
 //
 // JPEG 2000 Files
 //
 /* -------------------------------------------------------------------- */

#include <stdio.h>
#include "gdcmFile.h"

int
gdcmFile::gdcm_read_JPEG2000_file (void * image_buffer) {


   printf("Sorry JPEG 2000 File not yet taken into account\n");
   return 0;
}


