// gdcmJpeg2000.cxx
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "gdcmFile.h"

//-----------------------------------------------------------------------------
 /**
 * \ingroup gdcmFile
 * \brief   routine for JPEG decompression 
 * @param fp pointer to an already open file descriptor 
 *                      JPEG2000 encoded image
 * @param image_buffer to receive uncompressed pixels
 * @return 1 on success, 0 on error
 * @warning : not yet made
 */

bool gdcmFile::gdcm_read_JPEG2000_file (FILE *fp,void * image_buffer) {
   (void)fp;                  //FIXME
   (void)image_buffer;        //FIXME
   printf("Sorry JPEG 2000 File not yet taken into account\n");
   return false;
}

//-----------------------------------------------------------------------------

