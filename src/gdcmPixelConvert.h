/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelConvert.h,v $
  Language:  C++
  Date:      $Date: 2004/09/29 17:33:17 $
  Version:   $Revision: 1.1 $
                                                                                
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
bool ReadAndUncompress12Bits( FILE* filePointer,
                              size_t uncompressedSize,
                              size_t PixelNumber );
bool ReadUncompressed( FILE* filePointer,
                       size_t uncompressedSize,
                       size_t expectedSize );
bool ConvertGrayAndLutToRGB( uint8_t *lutRGBA );
bool ReadAndUncompressRLE8Bits(FILE* fp, size_t uncompressedSize );
bool UncompressRLE16BitsFromRLE8Bits( size_t PixelNumber, int NumberOfFrames );





};

//-----------------------------------------------------------------------------
#endif
