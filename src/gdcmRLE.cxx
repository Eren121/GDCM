#include <stdio.h>
#include "gdcmFile.h"
#include "jpeg/libijg8/cdjpeg.h"		/* Common decls for cjpeg/djpeg applications */
#include "jpeg/libijg8/jversion.h"	/* for version message */
#include <ctype.h>		/* to declare isprint() */
/* Create the add-on message string table. */

#define JMESSAGE(code,string)	string ,

static const char * const cdjpeg_message_table[] = {
#include "jpeg/libijg8/cderror.h"
  NULL
};
 
 /* -------------------------------------------------------------------- */
 //
 // RLE LossLess Files
 //
 /* -------------------------------------------------------------------- */

int
gdcmFile::gdcm_read_RLE_file (void * image_buffer) {


  











  
  
   printf("Sorry RLE LossLess File not yet taken into account\n");
   return 0;
}
