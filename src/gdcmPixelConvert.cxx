/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelConvert.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/10 16:44:00 $
  Version:   $Revision: 1.6 $
                                                                                
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
//
// grep PIXELCONVERT everywhere and clean up !


#define str2num(str, typeNum) *((typeNum *)(str))

#include "gdcmDebug.h"
#include "gdcmPixelConvert.h"

// External JPEG decompression

// for JPEGLosslessDecodeImage
#include "jpeg/ljpg/jpegless.h"

// For JPEG 2000, body in file gdcmJpeg2000.cxx
bool gdcm_read_JPEG2000_file (FILE* fp, void* image_buffer);

// For JPEG 8 Bits, body in file gdcmJpeg8.cxx
bool gdcm_read_JPEG_file     (FILE* fp, void* image_buffer);

// For JPEG 12 Bits, body in file gdcmJpeg12.cxx
bool gdcm_read_JPEG_file12   (FILE* fp, void* image_buffer);



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
void gdcmPixelConvert::ConvertDecompress12BitsTo16Bits(
                  uint8_t* pixelZone,
                  int sizeX,
                  int sizeY,
                  FILE* filePtr)
               throw ( gdcmFormatError )
{
   int nbPixels = sizeX * sizeY;
   uint16_t* destination = (uint16_t*)pixelZone;
                                                                                
   for( int p = 0; p < nbPixels; p += 2 )
   {
      uint8_t b0, b1, b2;
      size_t ItemRead;
                                                                                
      ItemRead = fread( &b0, 1, 1, filePtr);
      if ( ItemRead != 1 )
      {
         throw gdcmFormatError( "gdcmFile::ConvertDecompress12BitsTo16Bits()",
                                "Unfound first block" );
      }
                                                                                
      ItemRead = fread( &b1, 1, 1, filePtr);
      if ( ItemRead != 1 )
      {
         throw gdcmFormatError( "gdcmFile::ConvertDecompress12BitsTo16Bits()",
                                "Unfound second block" );
      }
                                                                                
      ItemRead = fread( &b2, 1, 1, filePtr);
      if ( ItemRead != 1 )
      {
         throw gdcmFormatError( "gdcmFile::ConvertDecompress12BitsTo16Bits()",
                                "Unfound second block" );
      }
                                                                                
      // Two steps are necessary to please VC++
      //
      // 2 pixels 12bit =     [0xABCDEF]
      // 2 pixels 16bit = [0x0ABD] + [0x0FCE]
      //                     A                     B                 D
      *destination++ =  ((b0 >> 4) << 8) + ((b0 & 0x0f) << 4) + (b1 & 0x0f);
      //                     F                     C                 E
      *destination++ =  ((b2 & 0x0f) << 8) + ((b1 >> 4) << 4) + (b2 >> 4);
                                                                                
      /// \todo JPR Troubles expected on Big-Endian processors ?
   }
}

/**
 * \brief     Try to deal with RLE 16 Bits. 
 *            We assume the RLE has allready been parsed and loaded in
 *            Uncompressed (through \ref ReadAndDecompressJPEGFile ).
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
      for ( unsigned int j = 0; j < PixelNumber; j++ )
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
bool gdcmPixelConvert::ReadAndDecompressRLEFile( void* image_buffer,
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
         fseek( fp, (*it)->Offset[k] ,SEEK_SET );
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

/**
 * \brief   Swap the bytes, according to swap code.
 * \warning not end user intended
 * @param   im area to deal with
 * @param   swap swap code
 * @param   lgr Area Length
 * @param   nb Pixels Bit number
 */
void gdcmPixelConvert::SwapZone(void* im, int swap, int lgr, int nb)
{
   int i;
                                                                                
   if( nb == 16 )
   {
      uint16_t* im16 = (uint16_t*)im;
      switch( swap )
      {
         case 0:
         case 12:
         case 1234:
            break;
         case 21:
         case 3412:
         case 2143:
         case 4321:
            for(i=0; i < lgr/2; i++)
            {
               im16[i]= (im16[i] >> 8) | (im16[i] << 8 );
            }
            break;
         default:
            std::cout << "SWAP value (16 bits) not allowed :i" << swap <<
            std::endl;
      }
   }
   else if( nb == 32 )
   {
      uint32_t s32;
      uint16_t fort, faible;
      uint32_t* im32 = (uint32_t*)im;
      switch ( swap )
      {
         case 0:
         case 1234:
            break;
         case 4321:
            for(i = 0; i < lgr/4; i++)
            {
               faible  = im32[i] & 0x0000ffff;  // 4321
               fort    = im32[i] >> 16;
               fort    = ( fort >> 8   ) | ( fort << 8 );
               faible  = ( faible >> 8 ) | ( faible << 8);
               s32     = faible;
               im32[i] = ( s32 << 16 ) | fort;
            }
            break;
         case 2143:
            for(i = 0; i < lgr/4; i++)
            {
               faible  = im32[i] & 0x0000ffff;   // 2143
               fort    = im32[i] >> 16;
               fort    = ( fort >> 8 ) | ( fort << 8 );
               faible  = ( faible >> 8) | ( faible << 8);
               s32     = fort;
               im32[i] = ( s32 << 16 ) | faible;
            }
            break;
         case 3412:
            for(i = 0; i < lgr/4; i++)
            {
               faible  = im32[i] & 0x0000ffff; // 3412
               fort    = im32[i] >> 16;
               s32     = faible;
               im32[i] = ( s32 << 16 ) | fort;
            }
            break;
         default:
            std::cout << "SWAP value (32 bits) not allowed : " << swap <<
            std::endl;
      }
   }
}



/**
 * \brief Deal with endianity i.e. re-arange bytes inside the integer
 */
void gdcmPixelConvert::ConvertReorderEndianity( uint8_t* pixelZone,
                                        size_t imageDataSize,
                                        int numberBitsStored,
                                        int numberBitsAllocated,
                                        int swapCode,
                                        bool signedPixel)
{
   if ( numberBitsAllocated != 8 )
   {
      SwapZone( pixelZone, swapCode, imageDataSize, numberBitsAllocated );
   }
                                                                                
   // Special kludge in order to deal with xmedcon broken images:
   if (  ( numberBitsAllocated == 16 )
      && ( numberBitsStored < numberBitsAllocated )
      && ( ! signedPixel ) )
   {
      int l = (int)(imageDataSize / (numberBitsAllocated/8));
      uint16_t *deb = (uint16_t *)pixelZone;
      for(int i = 0; i<l; i++)
      {
         if( *deb == 0xffff )
         {
           *deb = 0;
         }
         deb++;
      }
   }
}

/**
 * \brief     Reads from disk the Pixel Data of JPEG Dicom encapsulated
 &            file and uncompress it.
 * @param     fp already open File Pointer
 * @param     destination Where decompressed fragments should end up
 * @return    Boolean
 */
bool gdcmPixelConvert::ReadAndDecompressJPEGFile( uint8_t* destination,
                                   int XSize,
                                   int YSize,
                                   int BitsAllocated,
                                   int BitsStored,
                                   int SamplesPerPixel,
                                   int PixelSize,
                                   bool isJPEG2000,
                                   bool isJPEGLossless,
                                   gdcmJPEGFragmentsInfo* JPEGInfo,
                                   FILE* fp )
{
   // Loop on the fragment[s]
   for( gdcmJPEGFragmentsInfo::JPEGFragmentsList::iterator
        it  = JPEGInfo->Fragments.begin();
        it != JPEGInfo->Fragments.end();
      ++it )
   {
      fseek( fp, (*it)->Offset, SEEK_SET );
                                                                                
      if ( isJPEG2000 )
      {
         if ( ! gdcm_read_JPEG2000_file( fp, destination ) )
         {
            return false;
         }
      }
      else if ( isJPEGLossless )
      {
         // JPEG LossLess : call to xmedcom Lossless JPEG
         JPEGLosslessDecodeImage( fp,
                                  (uint16_t*)destination,
                                  PixelSize * 8 * SamplesPerPixel,
                                  (*it)->Length );
      }
      else if ( BitsStored == 8)
      {
         // JPEG Lossy : call to IJG 6b
         if ( ! gdcm_read_JPEG_file ( fp, destination ) )
         {
            return false;
         }
      }
      else if ( BitsStored == 12)
      {
         // Reading Fragment pixels
         if ( ! gdcm_read_JPEG_file12 ( fp, destination ) )
         {
            return false;
         }
      }
      else
      {
         // other JPEG lossy not supported
         dbg.Error(" gdcmFile::ReadPixelData : unknown jpeg lossy "
                   " compression ");
         return false;
      }
                                                                                
      // Advance to next free location in destination 
      // for next fragment decompression (if any)
      int length = XSize * YSize * SamplesPerPixel;
      int numberBytes = BitsAllocated / 8;
                                                                                
      destination = (uint8_t*)destination + length * numberBytes;
                                                                                
   }
   return true;
}

/**
 * \brief  Re-arrange the bits within the bytes.
 * @param  fp already open File Pointer
 * @param  destination Where decompressed fragments should end up
 * @return Boolean
 */
bool gdcmPixelConvert::ConvertReArrangeBits(
                          uint8_t* pixelZone,
                          size_t imageDataSize,
                          int numberBitsStored,
                          int numberBitsAllocated,
                          int highBitPosition )
     throw ( gdcmFormatError )
{
   if ( numberBitsStored != numberBitsAllocated )
   {
      int l = (int)(imageDataSize / (numberBitsAllocated/8));
      if ( numberBitsAllocated == 16 )
      {
         uint16_t mask = 0xffff;
         mask = mask >> ( numberBitsAllocated - numberBitsStored );
         uint16_t* deb = (uint16_t*)pixelZone;
         for(int i = 0; i<l; i++)
         {
            *deb = (*deb >> (numberBitsStored - highBitPosition - 1)) & mask;
            deb++;
         }
      }
      else if ( numberBitsAllocated == 32 )
      {
         uint32_t mask = 0xffffffff;
         mask = mask >> ( numberBitsAllocated - numberBitsStored );
         uint32_t* deb = (uint32_t*)pixelZone;
         for(int i = 0; i<l; i++)
         {
            *deb = (*deb >> (numberBitsStored - highBitPosition - 1)) & mask;
            deb++;
         }
      }
      else
      {
         dbg.Verbose(0, "gdcmPixelConvert::ConvertReArrangeBits: weird image");
         throw gdcmFormatError( "gdcmFile::ConvertReArrangeBits()",
                                "weird image !?" );
      }
   }
}

