/*
 * proto.h --
 *
 * Part of the Independent JPEG Group's software.
 * See the file Copyright for more details.
 */
/*
 * $Id: proto.h,v 1.1 2003/10/21 12:08:54 jpr Exp $
 */
#ifndef _PROTO
#define _PROTO

#ifdef __STDC__
        # define P(s) s
#else
        # define P(s) ()
#endif

#include "mcu.h"


/* huffd.c */
void  HuffDecoderInit P((DecompressInfo *dcPtr ));
void  DecodeImage P((DecompressInfo *dcPtr, unsigned short **image, int depth));
void  FixHuffTbl (HuffmanTable *htbl);

/* decomp.c */
int   ReadJpegData P((Uchar *buffer , int numBytes));

/* read.c */
void  ReadFileHeader P((DecompressInfo *dcPtr ));
int   ReadScanHeader P((DecompressInfo *dcPtr ));
int   GetJpegChar();
void  UnGetJpegChar(int ch);  

/* util.c */
int   JroundUp P((int a , int b ));
void  DecoderStructInit P((DecompressInfo *dcPtr ));

 /* mcu.c */
void  InitMcuTable P((int numMCU , int blocksInMCU ));
void  PrintMCU P((int blocksInMCU , MCU mcu ));


#undef P
#endif /* _PROTO */

