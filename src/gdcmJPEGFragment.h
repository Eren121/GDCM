/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJPEGFragment.h,v $
  Language:  C++
  Date:      $Date: 2004/10/10 16:44:00 $
  Version:   $Revision: 1.1 $
                                                                                
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

/**
 * \brief Utility class for summerizing the informations of a JPEG
 *        fragment of an "Encapsulated JPEG Compressed Image".
 *        This information is a mix of:
 *        - the fragment offset
 *        - the fragment length 
 *
 *        Each instance of this class (they can be as many instances for
 *        a given gdcmDocument as they are JPEG fragments and they are
 *        collected in a \ref gdcmJPEGFragmentsInfo )
 */
class GDCM_EXPORT gdcmJPEGFragment
{
friend class gdcmDocument;
friend class gdcmFile;
friend class gdcmPixelConvert;
   long    Offset;
   long    Length;
   gdcmJPEGFragment()
   {
      Offset = 0;
      Length = 0;
   }
};

//-----------------------------------------------------------------------------
#endif
