/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmRLEFramesInfo.h,v $
  Language:  C++
  Date:      $Date: 2004/10/06 09:58:08 $
  Version:   $Revision: 1.1 $
                                                                                
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
 *        "Encapsulated RLE Compressed Images" (see PS-3.3 annex G). 
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
   RLEFrameList Frames;
public:
   ~gdcmRLEFramesInfo();
};

//-----------------------------------------------------------------------------
#endif
