/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelConvert.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/13 04:05:04 $
  Version:   $Revision: 1.10 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

//////////////////   TEMPORARY NOTE
// look for "fixMem" and convert that to a member of this class
// Removing the prefix fixMem and dealing with allocations should do the trick
//
// grep PIXELCONVERT everywhere and clean up !

#include "gdcmDebug.h"
#include "gdcmPixelConvert.h"

// External JPEG decompression

// for JPEGLosslessDecodeImage
#include "jpeg/ljpg/jpegless.h"

namespace gdcm
{
                                                                                
#define str2num(str, typeNum) *((typeNum *)(str))

// For JPEG 2000, body in file gdcmJpeg2000.cxx
bool gdcm_read_JPEG2000_file (FILE* fp, void* image_buffer);

// For JPEG 8 Bits, body in file gdcmJpeg8.cxx
bool gdcm_read_JPEG_file     (FILE* fp, void* image_buffer);

// For JPEG 12 Bits, body in file gdcmJpeg12.cxx
bool gdcm_read_JPEG_file12   (FILE* fp, void* image_buffer);



//-----------------------------------------------------------------------------
// Constructor / Destructor
PixelConvert::PixelConvert() 
{
   RGB = 0;
   RGBSize = 0;
   Decompressed = 0;
   DecompressedSize = 0;
}

void PixelConvert::Squeeze() 
{
   if ( RGB ) {
      delete [] RGB;
   } 
   if ( Decompressed ) {
      delete [] Decompressed;
   }
}

PixelConvert::~PixelConvert() 
{
   Squeeze();
}

void PixelConvert::AllocateRGB()
{
  if ( RGB ) {
     delete [] RGB;
  }
  RGB = new uint8_t[RGBSize];
}

void PixelConvert::AllocateDecompressed()
{
  if ( Decompressed ) {
     delete [] Decompressed;
  }
  Decompressed = new uint8_t[ DecompressedSize ];
}

/**
 * \brief Read from file a 12 bits per pixel image and uncompress it
 *        into a 16 bits per pixel image.
 */
void PixelConvert::Decompress12BitsTo16Bits(
                  uint8_t* pixelZone,
                  FILE* filePtr)
               throw ( FormatError )
{
   int nbPixels = XSize * YSize;
   uint16_t* destination = (uint16_t*)pixelZone;
                                                                                
   for( int p = 0; p < nbPixels; p += 2 )
   {
      uint8_t b0, b1, b2;
      size_t ItemRead;
                                                                                
      ItemRead = fread( &b0, 1, 1, filePtr);
      if ( ItemRead != 1 )
      {
         throw FormatError( "File::Decompress12BitsTo16Bits()",
                                "Unfound first block" );
      }
                                                                                
      ItemRead = fread( &b1, 1, 1, filePtr);
      if ( ItemRead != 1 )
      {
         throw FormatError( "File::Decompress12BitsTo16Bits()",
                                "Unfound second block" );
      }
                                                                                
      ItemRead = fread( &b2, 1, 1, filePtr);
      if ( ItemRead != 1 )
      {
         throw FormatError( "File::Decompress12BitsTo16Bits()",
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
bool PixelConvert::UncompressRLE16BitsFromRLE8Bits(
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
bool PixelConvert::ReadAndUncompressRLEFragment( uint8_t* decodedZone,
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
         dbg.Verbose(0, "File::gdcm_read_RLE_fragment: we read more "
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
bool PixelConvert::ReadAndDecompressRLEFile(
                          void* image_buffer,
                          FILE* fp )
{
   uint8_t* im = (uint8_t*)image_buffer;
   long uncompressedSegmentSize = XSize * YSize;
                                                                                
                                                                                
   // Loop on the frame[s]
   for( RLEFramesInfo::RLEFrameList::iterator
        it  = RLEInfo->Frames.begin();
        it != RLEInfo->Frames.end();
      ++it )
   {
      // Loop on the fragments
      for( int k = 1; k <= (*it)->NumberFragments; k++ )
      {
         fseek( fp, (*it)->Offset[k] ,SEEK_SET );
         (void)PixelConvert::ReadAndUncompressRLEFragment(
                                 (uint8_t*) im, (*it)->Length[k],
                                 uncompressedSegmentSize, fp );
         im += uncompressedSegmentSize;
      }
   }
                                                                                
   if ( BitsAllocated == 16 )
   {
      // Try to deal with RLE 16 Bits
      (void)UncompressRLE16BitsFromRLE8Bits( ZSize,
                                             (uint8_t*) image_buffer);
   }
                                                                                
   return true;
}

/**
 * \brief   Swap the bytes, according to swap code.
 * \warning not end user intended
 * @param   im area to deal with
 */
void PixelConvert::SwapZone( uint8_t* im )
{
   unsigned int i;
                                                                                
   if( BitsAllocated == 16 )
   {
      uint16_t* im16 = (uint16_t*)im;
      switch( SwapCode )
      {
         case 0:
         case 12:
         case 1234:
            break;
         case 21:
         case 3412:
         case 2143:
         case 4321:
            for( i = 0; i < DecompressedSize / 2; i++ )
            {
               im16[i]= (im16[i] >> 8) | (im16[i] << 8 );
            }
            break;
         default:
            dbg.Verbose( 0, "PixelConvert::SwapZone: SwapCode value "
                            "(16 bits) not allowed." );
      }
   }
   else if( BitsAllocated == 32 )
   {
      uint32_t s32;
      uint16_t fort, faible;
      uint32_t* im32 = (uint32_t*)im;
      switch ( SwapCode )
      {
         case 0:
         case 1234:
            break;
         case 4321:
            for( i = 0; i < DecompressedSize / 4; i++ )
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
            for( i = 0; i < DecompressedSize / 4; i++ )
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
            for( i = 0; i < DecompressedSize / 4; i++ )
            {
               faible  = im32[i] & 0x0000ffff; // 3412
               fort    = im32[i] >> 16;
               s32     = faible;
               im32[i] = ( s32 << 16 ) | fort;
            }
            break;
         default:
            dbg.Verbose( 0, "PixelConvert::SwapZone: SwapCode value "
                            "(32 bits) not allowed." );
      }
   }
}

/**
 * \brief Deal with endianity i.e. re-arange bytes inside the integer
 */
void PixelConvert::ReorderEndianity( uint8_t* pixelZone )
{
   if ( BitsAllocated != 8 )
   {
      SwapZone( pixelZone );
   }
                                                                                
   // Special kludge in order to deal with xmedcon broken images:
   if (  ( BitsAllocated == 16 )
       && ( BitsStored < BitsAllocated )
       && ( ! PixelSign ) )
   {
      int l = (int)( DecompressedSize / ( BitsAllocated / 8 ) );
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
bool PixelConvert::ReadAndDecompressJPEGFile(
                          uint8_t* destination,
                          FILE* fp )
{
   // Loop on the fragment[s]
   for( JPEGFragmentsInfo::JPEGFragmentsList::iterator
        it  = JPEGInfo->Fragments.begin();
        it != JPEGInfo->Fragments.end();
      ++it )
   {
      fseek( fp, (*it)->Offset, SEEK_SET );
                                                                                
      if ( IsJPEG2000 )
      {
         if ( ! gdcm_read_JPEG2000_file( fp, destination ) )
         {
            return false;
         }
      }
      else if ( IsJPEGLossless )
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
         dbg.Error(" File::ReadAndDecompressJPEGFile: unknown jpeg lossy "
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
bool PixelConvert::ReArrangeBits( uint8_t* pixelZone )
     throw ( FormatError )
{
   if ( BitsStored != BitsAllocated )
   {
      int l = (int)( DecompressedSize / ( BitsAllocated / 8 ) );
      if ( BitsAllocated == 16 )
      {
         uint16_t mask = 0xffff;
         mask = mask >> ( BitsAllocated - BitsStored );
         uint16_t* deb = (uint16_t*)pixelZone;
         for(int i = 0; i<l; i++)
         {
            *deb = (*deb >> (BitsStored - HighBitPosition - 1)) & mask;
            deb++;
         }
      }
      else if ( BitsAllocated == 32 )
      {
         uint32_t mask = 0xffffffff;
         mask = mask >> ( BitsAllocated - BitsStored );
         uint32_t* deb = (uint32_t*)pixelZone;
         for(int i = 0; i<l; i++)
         {
            *deb = (*deb >> (BitsStored - HighBitPosition - 1)) & mask;
            deb++;
         }
      }
      else
      {
         dbg.Verbose(0, "PixelConvert::ReArrangeBits: weird image");
         throw FormatError( "File::ReArrangeBits()",
                                "weird image !?" );
      }
   }
   return true; //???
}

/**
 * \brief   Convert (Y plane, cB plane, cR plane) to RGB pixels
 * \warning Works on all the frames at a time
 */
void PixelConvert::ConvertYcBcRPlanesToRGBPixels(
                           uint8_t* destination,
                           size_t imageDataSize )
{
   uint8_t* oldPixelZone = new uint8_t[ imageDataSize ];
   memmove( oldPixelZone, destination, imageDataSize );
                                                                                
   // to see the tricks about YBR_FULL, YBR_FULL_422,
   // YBR_PARTIAL_422, YBR_ICT, YBR_RCT have a look at :
   // ftp://medical.nema.org/medical/dicom/final/sup61_ft.pdf
   // and be *very* affraid
   //
   int l        = XSize * YSize;
   int nbFrames = ZSize;
                                                                                
   uint8_t* a = oldPixelZone;
   uint8_t* b = oldPixelZone + l;
   uint8_t* c = oldPixelZone + l + l;
   double R, G, B;
                                                                                
   /// \todo : Replace by the 'well known' integer computation
   ///         counterpart. Refer to
   ///            http://lestourtereaux.free.fr/papers/data/yuvrgb.pdf
   ///         for code optimisation.
                                                                                
   for ( int i = 0; i < nbFrames; i++ )
   {
      for ( int j = 0; j < l; j++ )
      {
         R = 1.164 *(*a-16) + 1.596 *(*c -128) + 0.5;
         G = 1.164 *(*a-16) - 0.813 *(*c -128) - 0.392 *(*b -128) + 0.5;
         B = 1.164 *(*a-16) + 2.017 *(*b -128) + 0.5;
                                                                                
         if (R < 0.0)   R = 0.0;
         if (G < 0.0)   G = 0.0;
         if (B < 0.0)   B = 0.0;
         if (R > 255.0) R = 255.0;
         if (G > 255.0) G = 255.0;
         if (B > 255.0) B = 255.0;
                                                                                
         *(destination++) = (uint8_t)R;
         *(destination++) = (uint8_t)G;
         *(destination++) = (uint8_t)B;
         a++;
         b++;
         c++;
      }
   }
   delete[] oldPixelZone;
}

/**
 * \brief   Convert (Red plane, Green plane, Blue plane) to RGB pixels
 * \warning Works on all the frames at a time
 */
void PixelConvert::ConvertRGBPlanesToRGBPixels(
                          uint8_t* destination,
                          size_t imageDataSize )
{
   uint8_t* oldPixelZone = new uint8_t[ imageDataSize ];
   memmove( oldPixelZone, destination, imageDataSize );
                                                                                
   int l = XSize * YSize * ZSize;
                                                                                
   uint8_t* a = oldPixelZone;
   uint8_t* b = oldPixelZone + l;
   uint8_t* c = oldPixelZone + l + l;
                                                                                
   for (int j = 0; j < l; j++)
   {
      *(destination++) = *(a++);
      *(destination++) = *(b++);
      *(destination++) = *(c++);
   }
   delete[] oldPixelZone;
}

bool PixelConvert::ReadAndDecompressPixelData( void* destination, FILE* fp )
{
   if ( !fp )
   {
      return false;
   }
                                                                                
   if ( fseek(fp, PixelOffset, SEEK_SET) == -1 )
   {
      return false;
   }
                                                                                
   if ( BitsAllocated == 12 )
   {
      Decompress12BitsTo16Bits( (uint8_t*)destination, fp);
      return true;
   }
                                                                                
   //////////// Decompressed File
   if ( IsUncompressed )
   {
      size_t ItemRead = fread( destination, PixelDataLength, 1, fp);
      if ( ItemRead != 1 )
      {
         return false;
      }
      else
      {
         return true;
      }
   }
                                                                                
   ///////////// Run Length Encoding
   if ( IsRLELossless )
   {
      return ReadAndDecompressRLEFile( destination, fp );
   }
                                                                                
   ///////////// SingleFrame/Multiframe JPEG Lossless/Lossy/2000
   return ReadAndDecompressJPEGFile( (uint8_t*)destination, fp );
}

void PixelConvert::ComputeDecompressedImageDataSize()
{
   int bitsAllocated;
   // Number of "Bits Allocated" is fixed to 16 when it's 12, since
   // in this case we will expand the image to 16 bits (see
   //    \ref Decompress12BitsTo16Bits() )
   if (  BitsAllocated == 12 )
   {
      bitsAllocated = 16;
   }
                                                                                
   DecompressedSize = XSize * YSize * ZSize
                    * ( bitsAllocated / 8 )
                    * SamplesPerPixel;
}

} // end namespace gdcm
