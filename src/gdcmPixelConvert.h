/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelConvert.h,v $
  Language:  C++
  Date:      $Date: 2004/10/10 16:44:00 $
  Version:   $Revision: 1.5 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/


#ifndef GDCMPIXELCONVERT_H
#define GDCMPIXELCONVERTL_H

#include "gdcmCommon.h"
#include "gdcmRLEFramesInfo.h"
#include "gdcmJPEGFragmentsInfo.h"
#include "gdcmException.h"

/*
 * \brief Utility container for gathering the various forms the pixel data
 *        migth take during the user demanded processes.
 */
class GDCM_EXPORT gdcmPixelConvert {
friend class gdcmFile;
   /// Pixel data represented as RGB after color interpretation
   uint8_t* RGB;
   size_t   RGBSize;          //aka ImageDataSize
   /// Pixel data after decompression and bit/byte rearrangement.
   uint8_t* Uncompressed;
   size_t   UncompressedSize;
public:
   gdcmPixelConvert();
   ~gdcmPixelConvert();

   uint8_t* GetRGB() { return RGB; }
   void     SetRGBSize( size_t size ) { RGBSize = size; }
   size_t   GetRGBSize() { return RGBSize; }
   void     AllocateRGB();

   uint8_t* GetUncompressed() { return Uncompressed; }
   void     SetUncompressedSize( size_t size ) { UncompressedSize = size; }
   size_t   GetUncompressedSize() { return UncompressedSize; }
   void     AllocateUncompressed();

   void Squeeze();
//////////////////////////////////////////////////////////
// In progress
   static bool UncompressRLE16BitsFromRLE8Bits(
                  int XSize,
                  int YSize,
                  int NumberOfFrames,
                  uint8_t* fixMemUncompressed );
   static bool ReadAndUncompressRLEFragment(
                  uint8_t* decodedZone,
                  long fragmentSize,
                  long uncompressedSegmentSize,
                  FILE* fp );
   static bool ReadAndDecompressRLEFile(
                  void* image_buffer,
                  int XSize,
                  int YSize,
                  int ZSize,
                  int BitsAllocated,
                  gdcmRLEFramesInfo* RLEInfo,
                  FILE* fp );
   static void ConvertDecompress12BitsTo16Bits(
                  uint8_t* pixelZone,
                  int sizeX,
                  int sizeY,
                  FILE* filePtr) throw ( gdcmFormatError );
   static void SwapZone(void* im, int swap, int lgr, int nb);
   static void ConvertReorderEndianity(
                  uint8_t* pixelZone,
                  size_t imageDataSize,
                  int numberBitsStored,
                  int numberBitsAllocated,
                  int swapCode,
                  bool signedPixel );
   static bool ReadAndDecompressJPEGFile(
                  uint8_t* destination,
                  int XSize,
                  int YSize,
                  int BitsAllocated,
                  int BitsStored,
                  int SamplesPerPixel,
                  int PixelSize,
                  bool isJPEG2000,
                  bool isJPEGLossless,
                  gdcmJPEGFragmentsInfo* JPEGInfo,
                  FILE* fp );
   static bool gdcmPixelConvert::ConvertReArrangeBits(
                  uint8_t* pixelZone,
                  size_t imageDataSize,
                  int numberBitsStored,
                  int numberBitsAllocated,
                  int highBitPosition ) throw ( gdcmFormatError );


};

//-----------------------------------------------------------------------------
#endif
