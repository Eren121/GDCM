/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJPEGFragment.h,v $
  Language:  C++
  Date:      $Date: 2005/01/16 04:50:42 $
  Version:   $Revision: 1.7 $
                                                                                
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

namespace gdcm 
{

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

private:
   long    Offset;
   long    Length;

friend class Document;
friend class File;
friend class PixelReadConvert;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
