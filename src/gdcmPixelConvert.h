/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelConvert.h,v $
  Language:  C++
  Date:      $Date: 2004/10/13 14:15:30 $
  Version:   $Revision: 1.8 $
                                                                                
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

namespace gdcm
{
                                                                                
/*
 * \brief Utility container for gathering the various forms the pixel data
 *        migth take during the user demanded processes.
 */
class GDCM_EXPORT PixelConvert {
friend class File;
   /// Pixel data represented as RGB after color interpretation
   uint8_t* RGB;
   size_t   RGBSize;          //aka ImageDataSize
   /// Pixel data after decompression and bit/byte rearrangement.
   uint8_t* Decompressed;
   size_t   DecompressedSize;

   // Set by the accessors:
   size_t PixelOffset;
   size_t PixelDataLength;
   int XSize;
   int YSize;
   int ZSize;
   int BitsAllocated;
   int BitsStored;
   int HighBitPosition;
   int SamplesPerPixel;
   int PixelSize;
   bool PixelSign;
   int SwapCode;

   bool IsUncompressed;
   bool IsJPEG2000;
   bool IsJPEGLossless;
   bool IsRLELossless;

   RLEFramesInfo* RLEInfo;
   JPEGFragmentsInfo* JPEGInfo;

   // For handling color stage
   int PlanarConfiguration;
   bool IsMonochrome;
   bool IsPaletteColor;
   bool IsYBRFull;

private:
   bool ReadAndUncompressRLEFragment(
                  uint8_t* decodedZone,
                  long fragmentSize,
                  long uncompressedSegmentSize,
                  FILE* fp );
public:
   PixelConvert();
   ~PixelConvert();

   void SetXSize( int xSize ) { XSize = xSize; }
   void SetYSize( int ySize ) { YSize = ySize; }
   void SetZSize( int zSize ) { ZSize = zSize; }
   void SetBitsAllocated( int bitsAllocated ) { BitsAllocated = bitsAllocated; }
   void SetBitsStored( int bitsStored ) { BitsStored = bitsStored; }
   void SetHighBitPosition( int highBitPosition )
           { HighBitPosition = highBitPosition; }
   void SetSamplesPerPixel( int samplesPerPixel )
           { SamplesPerPixel = samplesPerPixel; }
   void SetPixelSize( int pixelSize ) { PixelSize = pixelSize; }
   void SetPixelSign( int pixelSign ) { PixelSign = pixelSign; }
   void SetSwapCode( int swapCode ) { SwapCode = swapCode; }
   void SetIsUncompressed( bool isUncompressed )
           { IsUncompressed = isUncompressed; }
   void SetIsJPEG2000( bool isJPEG2000 ) { IsJPEG2000 = isJPEG2000; }
   void SetIsJPEGLossless( bool isJPEGLossless )
           { IsJPEGLossless = isJPEGLossless; }
   void SetIsRLELossless( bool isRLELossless )
           { IsRLELossless = isRLELossless; }
   void SetPixelOffset( size_t pixelOffset ) { PixelOffset = pixelOffset; }
   void SetPixelDataLength( size_t pixelDataLength )
           { PixelDataLength = pixelDataLength; }
   void SetRLEInfo( RLEFramesInfo* inRLEFramesInfo )
           { RLEInfo = inRLEFramesInfo; }
   void SetJPEGInfo( JPEGFragmentsInfo* inJPEGFragmentsInfo )
           { JPEGInfo = inJPEGFragmentsInfo; }

   void SetPlanarConfiguration( size_t planarConfiguration )
           { PlanarConfiguration = planarConfiguration; }
   void SetIsMonochrome( bool isMonochrome ) { IsMonochrome = isMonochrome; }
   void SetIsPaletteColor( bool isPaletteColor )
           { IsPaletteColor = isPaletteColor; }
   void SetIsYBRFull( bool isYBRFull ) { IsYBRFull = isYBRFull; }

   uint8_t* GetRGB() { return RGB; }
   void     SetRGBSize( size_t size ) { RGBSize = size; }
   size_t   GetRGBSize() { return RGBSize; }
   void     AllocateRGB();

   uint8_t* GetDecompressed() { return Decompressed; }
   void     SetDecompressedSize( size_t size ) { DecompressedSize = size; }
   size_t   GetDecompressedSize() { return DecompressedSize; }
   void     AllocateDecompressed();

//////////////////////////////////////////////////////////
// In progress
private:
   bool UncompressRLE16BitsFromRLE8Bits(
                  int NumberOfFrames,
                  uint8_t* fixMemUncompressed );
   void ComputeDecompressedImageDataSize();
   void ReadAndDecompress12BitsTo16Bits(
                  uint8_t* pixelZone,
                  FILE* filePtr) throw ( FormatError );
   bool ReadAndDecompressRLEFile( void* image_buffer, FILE* fp );
   bool ReadAndDecompressJPEGFile( uint8_t* destination, FILE* fp );
   void SwapZone( uint8_t* im );
   void ReorderEndianity( uint8_t* pixelZone );
   bool ReArrangeBits( uint8_t* pixelZone ) throw ( FormatError );
   void ConvertRGBPlanesToRGBPixels( uint8_t* destination );
   void ConvertYcBcRPlanesToRGBPixels( uint8_t* destination );
public:
   bool ReadAndDecompressPixelData( void* destination, FILE* fp );
   bool HandleColor( uint8_t* destination );
   void Squeeze();
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
