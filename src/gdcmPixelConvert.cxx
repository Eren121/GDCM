/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelConvert.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/15 10:43:28 $
  Version:   $Revision: 1.14 $
                                                                                
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

namespace gdcm
{
                                                                                
#define str2num(str, typeNum) *((typeNum *)(str))

// For JPEG 2000, body in file gdcmJpeg2000.cxx
bool gdcm_read_JPEG2000_file (FILE* fp, void* image_buffer);

// For JPEG 8 Bits, body in file gdcmJpeg8.cxx
bool gdcm_read_JPEG_file8    (FILE* fp, void* image_buffer);

// For JPEG 12 Bits, body in file gdcmJpeg12.cxx
bool gdcm_read_JPEG_file12   (FILE* fp, void* image_buffer);

// For JPEG 16 Bits, body in file gdcmJpeg16.cxx
// Beware this is misleading there is no 16bits DCT algorithm, only
// jpeg lossless compression exist in 16bits.
bool gdcm_read_JPEG_file16   (FILE* fp, void* image_buffer);


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
void PixelConvert::ReadAndDecompress12BitsTo16Bits( FILE* fp )
               throw ( FormatError )
{
   int nbPixels = XSize * YSize;
   uint16_t* localDecompres = (uint16_t*)Decompressed;
                                                                                
   for( int p = 0; p < nbPixels; p += 2 )
   {
      uint8_t b0, b1, b2;
      size_t ItemRead;
                                                                                
      ItemRead = fread( &b0, 1, 1, fp );
      if ( ItemRead != 1 )
      {
         throw FormatError( "PixelConvert::ReadAndDecompress12BitsTo16Bits()",
                                "Unfound first block" );
      }
                                                                                
      ItemRead = fread( &b1, 1, 1, fp );
      if ( ItemRead != 1 )
      {
         throw FormatError( "PixelConvert::ReadAndDecompress12BitsTo16Bits()",
                                "Unfound second block" );
      }
                                                                                
      ItemRead = fread( &b2, 1, 1, fp );
      if ( ItemRead != 1 )
      {
         throw FormatError( "PixelConvert::ReadAndDecompress12BitsTo16Bits()",
                                "Unfound second block" );
      }
                                                                                
      // Two steps are necessary to please VC++
      //
      // 2 pixels 12bit =     [0xABCDEF]
      // 2 pixels 16bit = [0x0ABD] + [0x0FCE]
      //                        A                     B                 D
      *localDecompres++ =  ((b0 >> 4) << 8) + ((b0 & 0x0f) << 4) + (b1 & 0x0f);
      //                        F                     C                 E
      *localDecompres++ =  ((b2 & 0x0f) << 8) + ((b1 >> 4) << 4) + (b2 >> 4);
                                                                                
      /// \todo JPR Troubles expected on Big-Endian processors ?
   }
}

/**
 * \brief     Try to deal with RLE 16 Bits. 
 *            We assume the RLE has allready been parsed and loaded in
 *            Decompressed (through \ref ReadAndDecompressJPEGFile ).
 *            We here need to make 16 Bits Pixels from Low Byte and
 *            High Byte 'Planes'...(for what it may mean)
 * @return    Boolean
 */
bool PixelConvert::DecompressRLE16BitsFromRLE8Bits( int NumberOfFrames )
{
   size_t PixelNumber = XSize * YSize;
   size_t uncompressedSize = XSize * YSize * NumberOfFrames;

   // We assumed Decompressed contains the decoded RLE pixels but as
   // 8 bits per pixel. In order to convert those pixels to 16 bits
   // per pixel we cannot work in place within Decompressed and hence
   // we copy it in a safe place, say copyDecompressed.

   uint8_t* copyDecompressed = new uint8_t[ uncompressedSize * 2 ];
   memmove( copyDecompressed, Decompressed, uncompressedSize * 2 );

   uint8_t* x = Decompressed;
   uint8_t* a = copyDecompressed;
   uint8_t* b = a + PixelNumber;

   for ( int i = 0; i < NumberOfFrames; i++ )
   {
      for ( unsigned int j = 0; j < PixelNumber; j++ )
      {
         *(x++) = *(a++);
         *(x++) = *(b++);
      }
   }

   delete[] copyDecompressed;
      
   /// \todo check that operator new []didn't fail, and sometimes return false
   return true;
}

/**
 * \brief Implementation of the RLE decoding algorithm for uncompressing
 *        a RLE fragment. [refer to PS 3.5-2003, section G.3.2 p 86]
 * @param subDecompressed Sub region of \ref Decompressed where the de
 *        decoded fragment should be placed.
 * @param fragmentSize The length of the binary fragment as found on the disk.
 * @param uncompressedSegmentSize The expected length of the fragment ONCE
 *        decompressed.
 * @param fp File Pointer: on entry the position should be the one of
 *        the fragment to be decoded.
 */
bool PixelConvert::ReadAndDecompressRLEFragment( uint8_t* subDecompressed,
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
         fread( subDecompressed, count + 1, 1, fp);
         numberOfReadBytes   += count + 1;
         subDecompressed     += count + 1;
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
               subDecompressed[i] = newByte;
            }
            subDecompressed     += -count + 1;
            numberOfOutputBytes += -count + 1;
         }
      }
      // if count = 128 output nothing
                                                                                
      if ( numberOfReadBytes > fragmentSize )
      {
         dbg.Verbose(0, "PixelConvert::ReadAndDecompressRLEFragment: we "
                        "read more bytes than the segment size.");
         return false;
      }
   }
   return true;
}

/**
 * \brief     Reads from disk the Pixel Data of 'Run Length Encoded'
 *            Dicom encapsulated file and uncompress it.
 * @param     fp already open File Pointer
 *            at which the pixel data should be copied
 * @return    Boolean
 */
bool PixelConvert::ReadAndDecompressRLEFile( FILE* fp )
{
   uint8_t* subDecompressed = Decompressed;
   long decompressedSegmentSize = XSize * YSize;

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
         (void)ReadAndDecompressRLEFragment( subDecompressed,
                                             (*it)->Length[k],
                                             decompressedSegmentSize, 
                                             fp );
         subDecompressed += decompressedSegmentSize;
      }
   }
                                                                                
   if ( BitsAllocated == 16 )
   {
      // Try to deal with RLE 16 Bits
      (void)DecompressRLE16BitsFromRLE8Bits( ZSize );
   }
                                                                                
   return true;
}

/**
 * \brief Swap the bytes, according to \ref SwapCode.
 */
void PixelConvert::ConvertSwapZone()
{
   unsigned int i;
                                                                                
   if( BitsAllocated == 16 )
   {
      uint16_t* im16 = (uint16_t*)Decompressed;
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
            dbg.Verbose( 0, "PixelConvert::ConvertSwapZone: SwapCode value "
                            "(16 bits) not allowed." );
      }
   }
   else if( BitsAllocated == 32 )
   {
      uint32_t s32;
      uint16_t high;
      uint16_t low;
      uint32_t* im32 = (uint32_t*)Decompressed;
      switch ( SwapCode )
      {
         case 0:
         case 1234:
            break;
         case 4321:
            for( i = 0; i < DecompressedSize / 4; i++ )
            {
               low     = im32[i] & 0x0000ffff;  // 4321
               high    = im32[i] >> 16;
               high    = ( high >> 8 ) | ( high << 8 );
               low     = ( low  >> 8 ) | ( low  << 8 );
               s32     = low;
               im32[i] = ( s32 << 16 ) | high;
            }
            break;
         case 2143:
            for( i = 0; i < DecompressedSize / 4; i++ )
            {
               low     = im32[i] & 0x0000ffff;   // 2143
               high    = im32[i] >> 16;
               high    = ( high >> 8 ) | ( high << 8 );
               low     = ( low  >> 8 ) | ( low  << 8 );
               s32     = high;
               im32[i] = ( s32 << 16 ) | low;
            }
            break;
         case 3412:
            for( i = 0; i < DecompressedSize / 4; i++ )
            {
               low     = im32[i] & 0x0000ffff; // 3412
               high    = im32[i] >> 16;
               s32     = low;
               im32[i] = ( s32 << 16 ) | high;
            }
            break;
         default:
            dbg.Verbose( 0, "PixelConvert::ConvertSwapZone: SwapCode value "
                            "(32 bits) not allowed." );
      }
   }
}

/**
 * \brief Deal with endianity i.e. re-arange bytes inside the integer
 */
void PixelConvert::ConvertReorderEndianity()
{
   if ( BitsAllocated != 8 )
   {
      ConvertSwapZone();
   }

   // Special kludge in order to deal with xmedcon broken images:
   if (  ( BitsAllocated == 16 )
       && ( BitsStored < BitsAllocated )
       && ( ! PixelSign ) )
   {
      int l = (int)( DecompressedSize / ( BitsAllocated / 8 ) );
      uint16_t *deb = (uint16_t *)Decompressed;
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
 * @param     fp File Pointer
 * @return    Boolean
 */
bool PixelConvert::ReadAndDecompressJPEGFile( FILE* fp )
{
   uint8_t* localDecompressed = Decompressed;
   // Loop on the fragment[s]
   for( JPEGFragmentsInfo::JPEGFragmentsList::iterator
        it  = JPEGInfo->Fragments.begin();
        it != JPEGInfo->Fragments.end();
      ++it )
   {
      fseek( fp, (*it)->Offset, SEEK_SET );

      if ( IsJPEG2000 )
      {
         if ( ! gdcm_read_JPEG2000_file( fp,localDecompressed ) )
         {
            return false;
         }
      }
      else if ( BitsStored == 8)
      {
         // JPEG Lossy : call to IJG 6b
         if ( ! gdcm_read_JPEG_file8( fp, localDecompressed ) )
         {
            return false;
         }
      }
      else if ( BitsStored == 12)
      {
         // Reading Fragment pixels
         if ( ! gdcm_read_JPEG_file12 ( fp, localDecompressed ) )
         {
            return false;
         }
      }
      else if ( BitsStored == 16)
      {
         // Reading Fragment pixels
         if ( ! gdcm_read_JPEG_file16 ( fp, localDecompressed ) )
         {
            return false;
         }
         //assert( IsJPEGLossless );
      }
      else
      {
         // other JPEG lossy not supported
         dbg.Error("PixelConvert::ReadAndDecompressJPEGFile: unknown "
                   "jpeg lossy compression ");
         return false;
      }
                                                                                
      // Advance to next free location in Decompressed 
      // for next fragment decompression (if any)
      int length = XSize * YSize * SamplesPerPixel;
      int numberBytes = BitsAllocated / 8;
                                                                                
      localDecompressed += length * numberBytes;
   }
   return true;
}

/**
 * \brief  Re-arrange the bits within the bytes.
 * @return Boolean
 */
bool PixelConvert::ConvertReArrangeBits() throw ( FormatError )
{
   if ( BitsStored != BitsAllocated )
   {
      int l = (int)( DecompressedSize / ( BitsAllocated / 8 ) );
      if ( BitsAllocated == 16 )
      {
         uint16_t mask = 0xffff;
         mask = mask >> ( BitsAllocated - BitsStored );
         uint16_t* deb = (uint16_t*)Decompressed;
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
         uint32_t* deb = (uint32_t*)Decompressed;
         for(int i = 0; i<l; i++)
         {
            *deb = (*deb >> (BitsStored - HighBitPosition - 1)) & mask;
            deb++;
         }
      }
      else
      {
         dbg.Verbose(0, "PixelConvert::ConvertReArrangeBits: weird image");
         throw FormatError( "PixelConvert::ConvertReArrangeBits()",
                                "weird image !?" );
      }
   }
   return true;
}

/**
 * \brief   Convert (Y plane, cB plane, cR plane) to RGB pixels
 * \warning Works on all the frames at a time
 */
void PixelConvert::ConvertYcBcRPlanesToRGBPixels()
{
   uint8_t* localDecompressed = Decompressed;
   uint8_t* copyDecompressed = new uint8_t[ DecompressedSize ];
   memmove( copyDecompressed, localDecompressed, DecompressedSize );
                                                                                
   // to see the tricks about YBR_FULL, YBR_FULL_422,
   // YBR_PARTIAL_422, YBR_ICT, YBR_RCT have a look at :
   // ftp://medical.nema.org/medical/dicom/final/sup61_ft.pdf
   // and be *very* affraid
   //
   int l        = XSize * YSize;
   int nbFrames = ZSize;
                                                                                
   uint8_t* a = copyDecompressed;
   uint8_t* b = copyDecompressed + l;
   uint8_t* c = copyDecompressed + l + l;
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
                                                                                
         *(localDecompressed++) = (uint8_t)R;
         *(localDecompressed++) = (uint8_t)G;
         *(localDecompressed++) = (uint8_t)B;
         a++;
         b++;
         c++;
      }
   }
   delete[] copyDecompressed;
}

/**
 * \brief   Convert (Red plane, Green plane, Blue plane) to RGB pixels
 * \warning Works on all the frames at a time
 */
void PixelConvert::ConvertRGBPlanesToRGBPixels()
{
   uint8_t* localDecompressed = Decompressed;
   uint8_t* copyDecompressed = new uint8_t[ DecompressedSize ];
   memmove( copyDecompressed, localDecompressed, DecompressedSize );
                                                                                
   int l = XSize * YSize * ZSize;
                                                                                
   uint8_t* a = copyDecompressed;
   uint8_t* b = copyDecompressed + l;
   uint8_t* c = copyDecompressed + l + l;
                                                                                
   for (int j = 0; j < l; j++)
   {
      *(localDecompressed++) = *(a++);
      *(localDecompressed++) = *(b++);
      *(localDecompressed++) = *(c++);
   }
   delete[] copyDecompressed;
}

bool PixelConvert::ReadAndDecompressPixelData( FILE* fp )
{
   ComputeDecompressedImageDataSize();
   if ( HasLUT )
      DecompressedSize *= 3;
   AllocateDecompressed();
   //////////////////////////////////////////////////
   //// First stage: get our hands on the Pixel Data.
   if ( !fp )
   {
     dbg.Verbose( 0, "PixelConvert::ReadAndDecompressPixelData: "
                     "unavailable file pointer." );
      return false;
   }
                                                                                
   if ( fseek( fp, PixelOffset, SEEK_SET ) == -1 )
   {
     dbg.Verbose( 0, "PixelConvert::ReadAndDecompressPixelData: "
                     "unable to find PixelOffset in file." );
      return false;
   }
                                                                                
   //////////////////////////////////////////////////
   //// Second stage: read from disk dans uncompress.
   if ( BitsAllocated == 12 )
   {
      ReadAndDecompress12BitsTo16Bits( fp);
   }
   else if ( IsUncompressed )
   {
      size_t ItemRead = fread( Decompressed, PixelDataLength, 1, fp );
      if ( ItemRead != 1 )
      {
         dbg.Verbose( 0, "PixelConvert::ReadAndDecompressPixelData: "
                         "reading of uncompressed pixel data failed." );
         return false;
      }
   } 
   else if ( IsRLELossless )
   {
      if ( ! ReadAndDecompressRLEFile( fp ) )
      {
         dbg.Verbose( 0, "PixelConvert::ReadAndDecompressPixelData: "
                         "RLE decompressor failed." );
         return false;
      }
   }
   else
   {
      // Default case concerns JPEG family
      if ( ! ReadAndDecompressJPEGFile( fp ) )
      {
         dbg.Verbose( 0, "PixelConvert::ReadAndDecompressPixelData: "
                         "JPEG decompressor failed." );
         return false;
      }
   }

   ////////////////////////////////////////////
   //// Third stage: twigle the bytes and bits.
   ConvertReorderEndianity();
   ConvertReArrangeBits();
   ConvertHandleColor();

   return true;
}

void PixelConvert::ConvertHandleColor()
{
   //////////////////////////////////
   // Deal with the color decoding i.e. handle:
   //   - R, G, B planes (as opposed to RGB pixels)
   //   - YBR (various) encodings.
   //   - LUT[s] (or "PALETTE COLOR").
   //
   // The classification in the color decoding schema is based on the blending
   // of two Dicom tags values:
   // * "Photometric Interpretation" for which we have the cases:
   //  - [Photo A] MONOCHROME[1|2] pictures,
   //  - [Photo B] RGB or YBR_FULL_422 (which acts as RGB),
   //  - [Photo C] YBR_* (with the above exception of YBR_FULL_422)
   //  - [Photo D] "PALETTE COLOR" which indicates the presence of LUT[s].
   // * "Planar Configuration" for which we have the cases:
   //  - [Planar 0] 0 then Pixels are already RGB
   //  - [Planar 1] 1 then we have 3 planes : R, G, B,
   //  - [Planar 2] 2 then we have 1 gray Plane and 3 LUTs
   //
   // Now in theory, one could expect some coherence when blending the above
   // cases. For example we should not encounter files belonging at the
   // time to case [Planar 0] and case [Photo D].
   // Alas, this was only theory ! Because in practice some odd (read ill
   // formated Dicom) files (e.g. gdcmData/US-PAL-8-10x-echo.dcm) we encounter:
   //     - "Planar Configuration" = 0,
   //     - "Photometric Interpretation" = "PALETTE COLOR".
   // Hence gdcm shall use the folowing "heuristic" in order to be tolerant
   // towards Dicom-non-conformance files:
   //   << whatever the "Planar Configuration" value might be, a
   //      "Photometric Interpretation" set to "PALETTE COLOR" forces
   //      a LUT intervention >>
   //
   // Now we are left with the following handling of the cases:
   // - [Planar 0] OR  [Photo A] no color decoding (since respectively
   //       Pixels are already RGB and monochrome pictures have no color :),
   // - [Planar 1] AND [Photo B] handled with ConvertRGBPlanesToRGBPixels()
   // - [Planar 1] AND [Photo C] handled with ConvertYcBcRPlanesToRGBPixels()
   // - [Planar 2] OR  [Photo D] requires LUT intervention.

   if ( ! IsDecompressedRGB() )
   {
      // [Planar 2] OR  [Photo D]: LUT intervention done outside
      return;
   }
                                                                                
   if ( PlanarConfiguration == 1 )
   {
      if ( IsYBRFull )
      {
         // [Planar 1] AND [Photo C] (remember YBR_FULL_422 acts as RGB)
         ConvertYcBcRPlanesToRGBPixels();
      }
      else
      {
         // [Planar 1] AND [Photo C]
         ConvertRGBPlanesToRGBPixels();
      }
   }
                                                                                
   // When planarConf is 0, pixels are allready in RGB
}

/**
 * \brief Predicate to know wether the image[s] (once decompressed) is RGB.
 * \note See comments of \ref HandleColor
 */
bool PixelConvert::IsDecompressedRGB()
{
   if (   IsMonochrome
       || ( PlanarConfiguration == 2 )
       || IsPaletteColor )
   {
      return false;
   }
   return true;
}

void PixelConvert::ComputeDecompressedImageDataSize()
{
   int bitsAllocated = BitsAllocated;
   // Number of "Bits Allocated" is fixed to 16 when it's 12, since
   // in this case we will expand the image to 16 bits (see
   //    \ref ReadAndDecompress12BitsTo16Bits() )
   if (  BitsAllocated == 12 )
   {
      bitsAllocated = 16;
   }
                                                                                
   DecompressedSize = XSize * YSize * ZSize
                    * ( bitsAllocated / 8 )
                    * SamplesPerPixel;
}

} // end namespace gdcm

// NOTES on File internal calls
// User
// ---> GetImageData
//     ---> GetImageDataIntoVector
//        |---> GetImageDataIntoVectorRaw
//        | lut intervention
// User
// ---> GetImageDataRaw
//     ---> GetImageDataIntoVectorRaw
