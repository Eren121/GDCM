/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJpeg2000.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/22 03:05:42 $
  Version:   $Revision: 1.13 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
//-----------------------------------------------------------------------------
#include "gdcmFile.h"

namespace gdcm 
{

//-----------------------------------------------------------------------------
 /**
 * \brief   routine for JPEG decompression 
 * @param fp pointer to an already open file descriptor 
 *                      JPEG2000 encoded image
 * @param image_buffer to receive uncompressed pixels
 * @return 1 on success, 0 on error
 * @warning : not yet made
 */

bool gdcm_read_JPEG2000_file (std::ifstream* fp,void* image_buffer) {
   (void)fp;                  //FIXME
   (void)image_buffer;        //FIXME
   std::cout << "Sorry JPEG 2000 File not yet taken into account" << std::endl;
   return false;
}
} // end namespace gdcm

//-----------------------------------------------------------------------------

