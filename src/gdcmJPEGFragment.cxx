/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJPEGFragment.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/18 14:28:32 $
  Version:   $Revision: 1.5 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
                                                                                
#include "gdcmJPEGFragment.h"
#include "gdcmDebug.h"

namespace gdcm
{

// For JPEG 2000, body in file gdcmJpeg2000.cxx
bool gdcm_read_JPEG2000_file (std::ifstream* fp, void* image_buffer);

// For JPEG 8 Bits, body in file gdcmJpeg8.cxx
bool gdcm_read_JPEG_file8 (std::ifstream *fp, void *image_buffer);
bool gdcm_read_JPEG_memory8    (const JOCTET *buffer, const size_t buflen, 
                                void *image_buffer,
                                size_t *howManyRead, size_t *howManyWritten);
//
// For JPEG 12 Bits, body in file gdcmJpeg12.cxx
bool gdcm_read_JPEG_file12 (std::ifstream *fp, void *image_buffer);
bool gdcm_read_JPEG_memory12   (const JOCTET *buffer, const size_t buflen, 
                                void *image_buffer,
                                size_t *howManyRead, size_t *howManyWritten);

// For JPEG 16 Bits, body in file gdcmJpeg16.cxx
// Beware this is misleading there is no 16bits DCT algorithm, only
// jpeg lossless compression exist in 16bits.
bool gdcm_read_JPEG_file16 (std::ifstream *fp, void *image_buffer);
bool gdcm_read_JPEG_memory16   (const JOCTET *buffer, const size_t buflen, 
                                void* image_buffer,
                                size_t *howManyRead, size_t *howManyWritten);

/**
 * \brief Default constructor.
 */
JPEGFragment::JPEGFragment()
{
   Offset = 0;
   Length = 0;
}

/**
 * \brief        Print self.
 * @param os     Stream to print to.
 * @param indent Indentation string to be prepended during printing.
 */
void JPEGFragment::Print( std::ostream &os, std::string indent )
{
   os << indent
      << "JPEG fragment: offset : " <<  Offset
      << "   length : " <<  Length
      << std::endl;
}

/**
 * \brief Decompress 8bits JPEG Fragment
 * @param fp ifstream to write to
 * @param buffer     output (data decompress)
 * @param nBits      8/12 or 16 bits jpeg
 */
void JPEGFragment::DecompressJPEGFramesFromFile(std::ifstream *fp, uint8_t *buffer, int nBits)
{
   // First thing need to reset file to proper position:
   fp->seekg( Offset, std::ios::beg);

   if ( nBits == 8 )
   {
      // JPEG Lossy : call to IJG 6b
      if ( ! gdcm_read_JPEG_file8( fp, buffer) )
      {
         //return false;
      }
   }
   else if ( nBits <= 12 )
   {
      // Reading Fragment pixels
      if ( ! gdcm_read_JPEG_file12 ( fp, buffer) )
      {
         //return false;
      }
   }
   else if ( nBits <= 16 )
   {
      // Reading Fragment pixels
      if ( ! gdcm_read_JPEG_file16 ( fp, buffer) )
      {
         //return false;
      }
      //gdcmAssertMacro( IsJPEGLossless );
   }
   else
   {
      // other JPEG lossy not supported
      gdcmErrorMacro( "Unknown jpeg lossy compression ");
      //return false;
   }

}

void JPEGFragment::DecompressJPEGSingleFrameFragmentsFromFile(JOCTET *buffer, size_t totalLength, uint8_t* raw, int nBits)
{
   size_t howManyRead = 0;
   size_t howManyWritten = 0;
   
   if ( nBits == 8)
   {
      if ( ! gdcm_read_JPEG_memory8( buffer, totalLength, raw,
                                     &howManyRead, &howManyWritten ) ) 
      {
         gdcmErrorMacro( "Failed to read jpeg8 ");
         delete [] buffer;
         //return false;
      }
   }
   else if ( nBits <= 12)
   {
      if ( ! gdcm_read_JPEG_memory12( buffer, totalLength, raw,
                                      &howManyRead, &howManyWritten ) ) 
      {
         gdcmErrorMacro( "Failed to read jpeg12 ");
            delete [] buffer;
            //return false;
      }
   }
   else if ( nBits <= 16)
   {
      
      if ( ! gdcm_read_JPEG_memory16( buffer, totalLength, raw,
                                      &howManyRead, &howManyWritten ) ) 
      {
         gdcmErrorMacro( "Failed to read jpeg16 ");
         delete [] buffer;
         //return false;
      }
   }
   else
   {
      // other JPEG lossy not supported
      gdcmErrorMacro( "Unsupported jpeg lossy compression ");
      delete [] buffer;
      //return false;
   }      

}

void JPEGFragment::DecompressJPEGFragmentedFramesFromFile(JOCTET *buffer, uint8_t* raw, int nBits, size_t &howManyRead, size_t &howManyWritten, size_t totalLength)
{
   if ( nBits == 8 )
   {
     if ( ! gdcm_read_JPEG_memory8( buffer+howManyRead, totalLength-howManyRead,
                                  raw+howManyWritten,
                                  &howManyRead, &howManyWritten ) ) 
       {
         gdcmErrorMacro( "Failed to read jpeg8");
         //delete [] buffer;
         //return false;
       }
   }
   else if ( nBits <= 12 )
   {
   
     if ( ! gdcm_read_JPEG_memory12( buffer+howManyRead, totalLength-howManyRead,
                                   raw+howManyWritten,
                                   &howManyRead, &howManyWritten ) ) 
       {
         gdcmErrorMacro( "Failed to read jpeg12");
         //delete [] buffer;
         //return false;
      }
   }
   else if ( nBits <= 16 )
   {
   
     if ( ! gdcm_read_JPEG_memory16( buffer+howManyRead, totalLength-howManyRead,
                                   raw+howManyWritten,
                                   &howManyRead, &howManyWritten ) ) 
       {
         gdcmErrorMacro( "Failed to read jpeg16 ");
         //delete [] buffer;
         //return false;
       }
   }
   else
   {
      // other JPEG lossy not supported
      gdcmErrorMacro( "Unsupported jpeg lossy compression ");
      //delete [] buffer;
      //return false;
   }
}

} // end namespace gdcm

