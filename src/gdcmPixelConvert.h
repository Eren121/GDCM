/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelConvert.h,v $
  Language:  C++
  Date:      $Date: 2004/10/15 10:43:28 $
  Version:   $Revision: 1.9 $
                                                                                
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
   bool HasLUT;

public:
   PixelConvert();
   ~PixelConvert();

   //// Setter accessors:
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
   void SetHasLUT ( bool hasLUT ) { HasLUT = hasLUT; }
 
   void     SetRGBSize( size_t size ) { RGBSize = size; }
   void     SetDecompressedSize( size_t size ) { DecompressedSize = size; }

   //// Getter accessors:
   uint8_t* GetRGB() { return RGB; }
   size_t   GetRGBSize() { return RGBSize; }
   uint8_t* GetDecompressed() { return Decompressed; }
   size_t   GetDecompressedSize() { return DecompressedSize; }

   //// Predicates:
   bool IsDecompressedRGB();

//////////////////////////////////////////////////////////
private:
   // Use the fp:
   bool ReadAndDecompressRLEFragment(
                  uint8_t* decodedZone,
                  long fragmentSize,
                  long uncompressedSegmentSize,
                  FILE* fp );
   void ReadAndDecompress12BitsTo16Bits( FILE* fp ) throw ( FormatError );
   bool ReadAndDecompressRLEFile( FILE* fp );
   bool ReadAndDecompressJPEGFile( FILE* fp );

   // In place (within Decompressed and with no fp access) decompression
   // or convertion:
   bool DecompressRLE16BitsFromRLE8Bits( int NumberOfFrames );
   void ConvertSwapZone();
   void ConvertReorderEndianity();
   bool ConvertReArrangeBits() throw ( FormatError );
   void ConvertRGBPlanesToRGBPixels();
   void ConvertYcBcRPlanesToRGBPixels();
   void ConvertHandleColor();

   void ComputeDecompressedImageDataSize();
   void AllocateRGB();
   void AllocateDecompressed();
public:
// In progress
   bool ReadAndDecompressPixelData( FILE* fp );
   void Squeeze();
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
