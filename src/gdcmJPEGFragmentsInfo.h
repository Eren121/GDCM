/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJPEGFragmentsInfo.h,v $
  Language:  C++
  Date:      $Date: 2004/10/18 12:49:22 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/


#ifndef GDCMJPEGFRAGMENTSINFO_H
#define GDCMJPEGFRAGMENTSINFO_H

#include "gdcmJPEGFragment.h"
#include <list>

namespace gdcm 
{

/**
 * \brief Utility class for gathering the informations of the collection
 *        of JPEG fragment[s] (see \ref JPEGFragment)  when handling
 *        "Encapsulated JPEG Compressed Images". 
 *        The informations on each frame are obtained during the parsing
 *        of a Document (refer to
 *         \ref Document::ComputeJPEGFragmentInfo() ).
 *        They shall be used when (if necessary) decoding the fragments.
 *
 *        This class is simply a stl list<> of \ref JPEGFragment.
 */
class GDCM_EXPORT JPEGFragmentsInfo
{
friend class Document;
friend class File;
friend class PixelConvert;
private:
   typedef std::list< JPEGFragment* > JPEGFragmentsList;
   JPEGFragmentsList Fragments;
public:
   ~JPEGFragmentsInfo();
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
