/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmRLEFramesInfo.h,v $
  Language:  C++
  Date:      $Date: 2004/10/08 16:27:20 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/


#ifndef GDCMRLEFRAMESINFO_H
#define GDCMRLEFRAMESINFO_H

#include "gdcmRLEFrame.h"
#include <list>

/**
 * \brief Utility class for gathering the informations of the collection
 *        of RLE frame[s] (see \ref gdcmRLEFrame)  when handling
 *        "Encapsulated RLE Compressed Images" (see PS 3.5-2003 annex G). 
 *        Note: a classical image can be considered as the degenerated case
 *              of a multiframe image. In this case the collection is limited
 *              to a single individual frame.
 *        The informations on each frame are obtained during the parsing
 *        of a gdcmDocument (refer to \ref gdcmDocument::Parse7FE0() ).
 *        They shall be used when (if necessary) decoding the frames.
 *
 *        This class is simply a stl list<> of \ref gdcmRLEFrame.
 */
class GDCM_EXPORT gdcmRLEFramesInfo
{
   typedef std::list< gdcmRLEFrame* > RLEFrameList;
friend class gdcmDocument;
friend class gdcmFile;
friend class gdcmPixelConvert;
   RLEFrameList Frames;
public:
   ~gdcmRLEFramesInfo();
};

//-----------------------------------------------------------------------------
#endif
