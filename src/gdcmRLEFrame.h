/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmRLEFrame.h,v $
  Language:  C++
  Date:      $Date: 2005/01/26 11:42:02 $
  Version:   $Revision: 1.12 $
                                                                                
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

#include <iostream>

namespace gdcm 
{
/**
 * \brief Utility class for summerizing the informations of a SINGLE RLE
 *        frame of an "Encapsulated RLE Compressed Image" (refer to
 *        PS 3.5-2003 annex G).
 *        This information is a mix of:
 *        - the RLE Header (see PS 3.5-2003 section G5) and
 *        - the lengths of each RLE segment [ which can be decuded from
 *          both the above RLE Header and the itemlength of the frame).
 *
 *        Each instance of this class (they can be as many instances for
 *        a given Document as they are frames and they are collected in
 *        a \ref RLEFramesInfo ) describes :
 *        - the total number of segments (up to 15),
 *        - the offsets of each segment of the frame,
 *        - the (corresponding) lengths of each segment of the frame.
 */
class GDCM_EXPORT RLEFrame
{
public:
   RLEFrame() { NumberFragments = 0; }
   void Print( std::ostream &os = std::cout, std::string indent = "" );

   void SetNumberOfFragments(unsigned int number) { NumberFragments = number; };   
   unsigned int GetNumberOfFragments() { return NumberFragments; };
   void SetOffset(unsigned int id,long offset);
   long GetOffset(unsigned int id);
   void SetLength(unsigned int id,long length);
   long GetLength(unsigned int id);

private:
   unsigned int NumberFragments;
   long Offset[15];
   long Length[15];
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
