/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelConvert.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/08 08:56:48 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

//////////////////   TEMPORARY NOT
// look for "fixMem" and convert that to a member of this class
// Removing the prefix fixMem and dealing with allocations should do the trick
#define str2num(str, typeNum) *((typeNum *)(str))

#include "gdcmDebug.h"
#include "gdcmPixelConvert.h"


//-----------------------------------------------------------------------------
// Constructor / Destructor
gdcmPixelConvert::gdcmPixelConvert() 
{
   RGB = 0;
   RGBSize = 0;
   Uncompressed = 0;
   UncompressedSize = 0;
}

void gdcmPixelConvert::Squeeze() 
{
   if ( RGB ) {
      delete [] RGB;
   } 
   if ( Uncompressed ) {
      delete [] Uncompressed;
   }
}

gdcmPixelConvert::~gdcmPixelConvert() 
{
   Squeeze();
}

void gdcmPixelConvert::AllocateRGB()
{
  if ( RGB ) {
     delete [] RGB;
  }
  RGB = new uint8_t[RGBSize];
}

void gdcmPixelConvert::AllocateUncompressed()
{
  if ( Uncompressed ) {
     delete [] Uncompressed;
  }
  Uncompressed = new uint8_t[ UncompressedSize ];
}

/**
 * \brief Read from file a 12 bits per pixel image and uncompress it
 *        into a 16 bits per pixel image.
 */
bool gdcmPixelConvert::ReadAndUncompress12Bits( FILE* filePointer,
                                                size_t uncompressedSize,
                                                size_t PixelNumber )
{
   SetUncompressedSize( uncompressedSize );
   AllocateUncompressed();

   uint16_t* pdestination = (uint16_t*)Uncompressed;
                                                                                
   for(int p = 0; p < PixelNumber; p += 2 )
   {
      // 2 pixels 12bit =     [0xABCDEF]
      // 2 pixels 16bit = [0x0ABD] + [0x0FCE]
      uint8_t b0, b1, b2;
      size_t ItemRead;
      ItemRead = fread( &b0, 1, 1, filePointer);
      if ( ItemRead != 1 )
      {
         return false;
      }
      ItemRead = fread( &b1, 1, 1, filePointer);
      if ( ItemRead != 1 )
      {
         return false;
      }
      ItemRead = fread( &b2, 1, 1, filePointer);
      if ( ItemRead != 1 )
      {
         return false;
      }
                                                                                
      //Two steps are necessary to please VC++
      *pdestination++ =  ((b0 >> 4) << 8) + ((b0 & 0x0f) << 4) + (b1 & 0x0f);
      //                     A                     B                 D
      *pdestination++ =  ((b2 & 0x0f) << 8) + ((b1 >> 4) << 4) + (b2 >> 4);
      //                     F                     C                 E
                                                                                
      /// \todo JPR Troubles expected on Big-Endian processors ?
   }
   return true;
}

/**
 * \brief Read from file an uncompressed image.
 */
bool gdcmPixelConvert::ReadUncompressed( FILE* filePointer,
                                         size_t uncompressedSize,
                                         size_t expectedSize )
{
   if ( expectedSize > uncompressedSize )
   {
      dbg.Verbose(0, "gdcmPixelConvert::ReadUncompressed: expectedSize"
                     "is bigger than it should");
      return false;
   }
   SetUncompressedSize( uncompressedSize );
   AllocateUncompressed();
   size_t ItemRead = fread( (void*)Uncompressed, expectedSize, 1, filePointer);
   if ( ItemRead != 1 )
   {
      return false;
   }
   return true;
}

/**
 * \brief  Convert a Gray plane and ( Lut R, Lut G, Lut B ) into an
 *         RGB plane.
 * @return True on success.
 */
bool gdcmPixelConvert::ConvertGrayAndLutToRGB( uint8_t *lutRGBA )

{
   /// We assume Uncompressed contains the decompressed gray plane
   /// and build the RGB image.
   SetRGBSize( UncompressedSize );
   AllocateRGB();

//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//COPY HERE THE CODE OF GetImageDataIntoVector
      
   /// \todo check that operator new []didn't fail, and sometimes return false
   return true;
}

/**
 * \brief     Try to deal with RLE 16 Bits. 
 *            We assume the RLE has allready been parsed and loaded in
 *            Uncompressed (through \ref ReadAndUncompressRLE8Bits ).
 *            We here need to make 16 Bits Pixels from Low Byte and
 *            High Byte 'Planes'...(for what it may mean)
 * @return    Boolean
 */
uint8_t* gdcmPixelConvert::UncompressRLE16BitsFromRLE8Bits(
                       int XSize,
                       int YSize,
                       int NumberOfFrames,
                       uint8_t* fixMemUncompressed )
{
   size_t PixelNumber = XSize * YSize;
   size_t fixMemUncompressedSize = XSize * YSize * NumberOfFrames;

   // We assumed Uncompressed contains the decoded RLE pixels but as
   // 8 bits per pixel. In order to convert those pixels to 16 bits
   // per pixel we cannot work in place within Uncompressed.
   // Here is how we handle things:
   // - First  stage: copy Uncompressed in a safe place, say OldUncompressed
   // - Second stage: reallocate Uncompressed with the needed space
   // - Third  stage: expand from OldUncompressed to Uncompressed
   // - Fourth stage: clean up OldUncompressed

   /// First stage:
   uint8_t* OldUncompressed = new uint8_t[ fixMemUncompressedSize * 2 ];
   memmove( OldUncompressed, fixMemUncompressed, fixMemUncompressedSize * 2);

   /// Second stage:
   //fixMem SetUncompressedSize( 2 * UncompressedSize );
   //fixMem AllocateUncompressed();
   uint8_t* fixMemNewUncompressed = new uint8_t[fixMemUncompressedSize * 2];

   /// Third stage:
   uint8_t* x = fixMemNewUncompressed;
   uint8_t* a = OldUncompressed;
   uint8_t* b = a + PixelNumber;

   for ( int i = 0; i < NumberOfFrames; i++ )
   {
      for ( int j = 0; j < PixelNumber; j++ )
      {
         *(x++) = *(a++);
         *(x++) = *(b++);
      }
   }

   // Fourth stage:
   delete[] OldUncompressed;
      
   /// \todo check that operator new []didn't fail, and sometimes return false
   return fixMemNewUncompressed;
}
