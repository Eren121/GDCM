/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmRLE.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/08 08:56:48 $
  Version:   $Revision: 1.26 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmFile.h"
#include "gdcmDebug.h"
#include "gdcmPixelConvert.h"
#include <stdio.h>

//-----------------------------------------------------------------------------
/**
 * \ingroup   gdcmFile
 * \brief     Reads a 'Run Length Encoded' Dicom encapsulated file
 * @param     fp already open File Pointer
 * @param     image_buffer destination Address (in caller's memory space) 
 *            at which the pixel data should be copied 
 * @return    Boolean 
 */
bool gdcmFile::gdcm_read_RLE_file( FILE* fp, void* image_buffer )
{
   char* im = (char *)image_buffer;
   long uncompressedSegmentSize = Header->GetXSize() * Header->GetYSize();
   

   // Loop on the frame[s]
   for( gdcmRLEFramesInfo::RLEFrameList::iterator
        it  = Header->RLEInfo.Frames.begin();
        it != Header->RLEInfo.Frames.end();
      ++it )
   {
       std::cout << "...new frame...\n ";
      // Loop on the fragments
      for( unsigned int k = 1; k <= (*it)->NumberFragments; k++ )
      {
         fseek( fp, (*it)->Offset[k] ,SEEK_SET);  
         (void)gdcm_read_RLE_fragment( (uint8_t**) (&im), (*it)->Length[k],
                                      uncompressedSegmentSize, fp );
      }
   }
 
   if ( Header->GetBitsAllocated() == 16 )
   {
     // Try to deal with RLE 16 Bits
     /*
      image_buffer = (void*)gdcmPixelConvert::UncompressRLE16BitsFromRLE8Bits(
                                             Header->GetXSize(),
                                             Header->GetYSize(),
                                             Header->GetZSize(),
                                             (uint8_t*) im);
     */
      im = (char *)image_buffer;
      //  need to make 16 Bits Pixels from Low Byte and Hight Byte 'Planes'

      int l = Header->GetXSize()*Header->GetYSize();
      int nbFrames = Header->GetZSize();

      char * newDest = new char[l*nbFrames*2];
      char *x  = newDest;
      char * a = (char *)image_buffer;
      char * b = a + l;

      for (int i=0;i<nbFrames;i++)
      {
         for (int j=0;j<l; j++)
         {
            *(x++) = *(a++);
            *(x++) = *(b++);
         }
      }
      memmove(image_buffer,newDest,ImageDataSize);
      delete[] newDest;
   }
      
   return true;
}


// ----------------------------------------------------------------------------
/**
 * \brief Implementation of the RLE decoding algorithm for uncompressing
 *        a RLE fragment. [refer to PS 3.5-2003, section G.3.2 p 86]
 */
bool gdcmFile::gdcm_read_RLE_fragment( uint8_t** decodedZone,
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
         fread( *decodedZone, count + 1, 1, fp);
         numberOfReadBytes += count + 1;
         *decodedZone        += count + 1;
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
               (*decodedZone)[i] = newByte;  
            }
            *decodedZone        += -count + 1;
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

// ----------------------------------------------------------------------------
