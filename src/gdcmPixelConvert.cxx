/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelConvert.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/08 16:27:20 $
  Version:   $Revision: 1.3 $
                                                                                
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
bool gdcmPixelConvert::UncompressRLE16BitsFromRLE8Bits(
                       int XSize,
                       int YSize,
                       int NumberOfFrames,
                       uint8_t* fixMemUncompressed )
{
   size_t PixelNumber = XSize * YSize;
   size_t fixMemUncompressedSize = XSize * YSize * NumberOfFrames;

   // We assumed Uncompressed contains the decoded RLE pixels but as
   // 8 bits per pixel. In order to convert those pixels to 16 bits
   // per pixel we cannot work in place within Uncompressed and hence
   // we copy Uncompressed in a safe place, say OldUncompressed.

   uint8_t* OldUncompressed = new uint8_t[ fixMemUncompressedSize * 2 ];
   memmove( OldUncompressed, fixMemUncompressed, fixMemUncompressedSize * 2);

   uint8_t* x = fixMemUncompressed;
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

   delete[] OldUncompressed;
      
   /// \todo check that operator new []didn't fail, and sometimes return false
   return true;
}

/**
 * \brief Implementation of the RLE decoding algorithm for uncompressing
 *        a RLE fragment. [refer to PS 3.5-2003, section G.3.2 p 86]
 */
bool gdcmPixelConvert::ReadAndUncompressRLEFragment( uint8_t* decodedZone,
                                               long fragmentSize,
                                               long uncompressedSegmentSize,
                                               FILE* fp )
{
   int8_t count;
   long numberOfOutputBytes = 0;
   long numberOfReadBytes = 0;
                                                                                
   while( numberOfOutputBytes < uncompressedSegmentSize )
   {
      fread( &count, 1, 1, fp );
      numberOfReadBytes += 1;
      if ( count >= 0 )
      // Note: count <= 127 comparison is always true due to limited range
      //       of data type int8_t [since the maximum of an exact width
      //       signed integer of width N is 2^(N-1) - 1, which for int8_t
      //       is 127].
      {
         fread( decodedZone, count + 1, 1, fp);
         numberOfReadBytes += count + 1;
         decodedZone         += count + 1;
         numberOfOutputBytes += count + 1;
      }
      else
      {
         if ( ( count <= -1 ) && ( count >= -127 ) )
         {
            int8_t newByte;
            fread( &newByte, 1, 1, fp);
            numberOfReadBytes += 1;
            for( int i = 0; i < -count + 1; i++ )
            {
               decodedZone[i] = newByte;
            }
            decodedZone         += -count + 1;
            numberOfOutputBytes += -count + 1;
         }
      }
      // if count = 128 output nothing
                                                                                
      if ( numberOfReadBytes > fragmentSize )
      {
         dbg.Verbose(0, "gdcmFile::gdcm_read_RLE_fragment: we read more "
                        "bytes than the segment size.");
         return false;
      }
   }
   return true;
}

/**
 * \brief     Reads from disk the Pixel Data of 'Run Length Encoded'
 *            Dicom encapsulated file and uncompress it.
 * @param     fp already open File Pointer
 * @param     image_buffer destination Address (in caller's memory space)
 *            at which the pixel data should be copied
 * @return    Boolean
 */
bool gdcmPixelConvert::gdcm_read_RLE_file( void* image_buffer,
                                   int XSize,
                                   int YSize,
                                   int ZSize,
                                   int BitsAllocated,
                                   gdcmRLEFramesInfo* RLEInfo,
                                   FILE* fp )
{
   uint8_t* im = (uint8_t*)image_buffer;
   long uncompressedSegmentSize = XSize * YSize;
                                                                                
                                                                                
   // Loop on the frame[s]
   for( gdcmRLEFramesInfo::RLEFrameList::iterator
        it  = RLEInfo->Frames.begin();
        it != RLEInfo->Frames.end();
      ++it )
   {
      // Loop on the fragments
      for( unsigned int k = 1; k <= (*it)->NumberFragments; k++ )
      {
         fseek( fp, (*it)->Offset[k] ,SEEK_SET);
         (void)gdcmPixelConvert::ReadAndUncompressRLEFragment(
                                 (uint8_t*) im, (*it)->Length[k],
                                 uncompressedSegmentSize, fp );
         im += uncompressedSegmentSize;
      }
   }
                                                                                
   if ( BitsAllocated == 16 )
   {
      // Try to deal with RLE 16 Bits
      (void)gdcmPixelConvert::UncompressRLE16BitsFromRLE8Bits(
                                             XSize,
                                             YSize,
                                             ZSize,
                                             (uint8_t*) image_buffer);
   }
                                                                                
   return true;
}

