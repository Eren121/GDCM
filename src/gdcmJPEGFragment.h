/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJPEGFragment.h,v $
  Language:  C++
  Date:      $Date: 2005/01/24 14:52:50 $
  Version:   $Revision: 1.12 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/


#ifndef GDCMJPEGFRAGMENT_H
#define GDCMJPEGFRAGMENT_H

#include "gdcmCommon.h"

#include <iostream>
#include <fstream>

namespace gdcm 
{
#define JOCTET uint8_t

/**
 * \brief Utility class for summerizing the informations of a JPEG
 *        fragment of an "Encapsulated JPEG Compressed Image".
 *        This information is a mix of:
 *        - the fragment offset
 *        - the fragment length 
 *
 *        Each instance of this class (they can be as many instances for
 *        a given Document as they are JPEG fragments and they are
 *        collected in a \ref JPEGFragmentsInfo )
 */
class GDCM_EXPORT JPEGFragment
{
public:
   JPEGFragment();
   void Print( std::ostream &os = std::cout, std::string indent = "" );
   void DecompressJPEGFramesFromFile(std::ifstream *fp, uint8_t *buffer, int nBits, int & statesuspension);
   void DecompressJPEGSingleFrameFragmentsFromFile(JOCTET *buffer, size_t totalLength, uint8_t* raw, int nBits);
   void DecompressJPEGFragmentedFramesFromFile(JOCTET *buffer, uint8_t* raw, int nBits, size_t &howManyRead, size_t &howManyWritten, size_t totalLength);

   bool gdcm_read_JPEG_file8 (std::ifstream* fp, void* image_buffer, int & statesuspension );
   bool gdcm_read_JPEG_file12 (std::ifstream* fp, void* image_buffer, int & statesuspension );
   bool gdcm_read_JPEG_file16 (std::ifstream* fp, void* image_buffer, int & statesuspension );

//private:
   uint32_t    Offset;
   uint32_t    Length;

   uint8_t *pimage;


friend class Document;
friend class FileHelper;
friend class PixelReadConvert;
friend class JPEGFragmentsInfo;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
