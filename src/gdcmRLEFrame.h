/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmRLEFrame.h,v $
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


#ifndef GDCMRLEFRAME_H
#define GDCMRLEFRAME_H

#include "gdcmCommon.h"

/**
 * \brief Utility class for summerizing the informations of a SINGLE RLE
 *        frame of an "Encapsulated RLE Compressed Image" (see PS-3.3 annex G).
 *        This information is a mix of:
 *        - the RLE Header (see PS-3.3 section G5) and
 *        - the lengths of each RLE segment [ which can be decuded from
 *          both the above RLE Header and the itemlength of the frame).
 *
 *        Each instance of this class (they can be as many instances for
 *        a given gdcmDocument as they are frames and they are collected in
 *        a \ref gdcmRLEFramesInfo ) describes :
 *        - the total number of segments (up to 15),
 *        - the offsets of each segment of the frame,
 *        - the (corresponding) lengths of each segment of the frame.
 */
class GDCM_EXPORT gdcmRLEFrame
{
friend class gdcmDocument;
friend class gdcmFile;
   int     NumberFragments;
   uint8_t Offset[15];
   long    Length[15];
   gdcmRLEFrame() { NumberFragments = 0; }
   
};

//-----------------------------------------------------------------------------
#endif
