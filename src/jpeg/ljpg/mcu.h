/*
 * mcu.h --
 *
 * Part of the Independent JPEG Group's software.
 * See the file Copyright for more details.
 *
 */
/*
 * $Id: mcu.h,v 1.1 2003/10/21 12:08:54 jpr Exp $
 */
#ifndef _MCU
#define _MCU

/*
 * An MCU (minimum coding unit) is an array of samples.
 */
typedef short ComponentType; /* the type of image components */
typedef ComponentType *MCU;  /* MCU - array of samples */

extern MCU *mcuTable; /* the global mcu table that buffers the source image */
extern int numMCU;    /* number of MCUs in mcuTable */
extern MCU *mcuROW1,*mcuROW2; /* pt to two rows of MCU in encoding & decoding */

/*
 *--------------------------------------------------------------
 *
 * MakeMCU --
 *
 *      MakeMCU returns an MCU for input parsing.
 *
 * Results:
 *      A new MCU
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */
#define MakeMCU(dcPtr)  (mcuTable[numMCU++])

#endif /* _MCU */
