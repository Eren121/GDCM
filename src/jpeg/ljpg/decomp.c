/*
 * decomp.c --
 *
 * This is the routine that is called to decompress a frame 
 * image data. It is based on the program originally named ljpgtopnm.c.
 * Major portions taken from the Independent JPEG Group' software, and
 * from the Cornell lossless JPEG code
 */
/*
 * $Id: decomp.c,v 1.1 2003/10/21 12:08:53 jpr Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include "io.h"
#include "jpeg.h"
#include "mcu.h"
#include "proto.h"

DecompressInfo  dcInfo;
StreamIN        JpegInFile;     

/*
 *--------------------------------------------------------------
 *
 * ReadJpegData --
 *
 *        This is an interface routine to the JPEG library.  The
 *        JPEG library calls this routine to "get more data"
 *
 * Results:
 *        Number of bytes actually returned.
 *
 * Side effects:
 *        None.
 *
 *--------------------------------------------------------------
 */
static void efree(void **ptr)
{
        if((*ptr) != 0)
                free((*ptr));
        *ptr = 0;
}


int ReadJpegData (Uchar *buffer, int numBytes)
{
    unsigned long size = sizeof(unsigned char);

    fread(buffer,size,numBytes, JpegInFile);

    return numBytes;  
}


short JPEGLosslessDecodeImage (StreamIN inFile, unsigned short *image16, int depth, int length)
{ 
    /* Initialization */
    JpegInFile = inFile;
    MEMSET (&dcInfo, 0, sizeof (dcInfo));
    inputBufferOffset = 0;
     
    /* Allocate input buffer */
    inputBuffer = (unsigned char*)malloc((size_t)length+5);
    if (inputBuffer == NULL)
                return -1;

        /* Read input buffer */
    ReadJpegData (inputBuffer, length);
    inputBuffer [length] = EOF;
    
        /* Read JPEG File header */
    ReadFileHeader (&dcInfo);
    if (dcInfo.error) { efree ((void **)&inputBuffer); return -1; }

    /* Read the scan header */
    if (!ReadScanHeader (&dcInfo)) { efree ((void **)&inputBuffer); return -1; }
   
    /* 
     * Decode the image bits stream. Clean up everything when
     * finished decoding.
     */
    DecoderStructInit (&dcInfo);

    if (dcInfo.error) { efree ((void **)&inputBuffer); return -1; }

    HuffDecoderInit (&dcInfo);
    
    if (dcInfo.error) { efree ((void **)&inputBuffer); return -1; }

    DecodeImage (&dcInfo, (unsigned short **) &image16, depth);
    
    /* Free input buffer */
    efree ((void **)&inputBuffer);
    
    return 0;
}
