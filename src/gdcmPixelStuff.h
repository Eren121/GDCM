/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelStuff.h,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:07 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/


#ifndef GDCMPIXELSTUFF_H
#define GDCMPIXELSTUFF_H

#include "gdcmCommon.h"

//-----------------------------------------------------------------------------
/*
 * Container for Pixel related stuff
 * \note   This is a singleton
 */
class GDCM_EXPORT gdcmPixelStuff {
public:
   gdcmPixelStuff(void);
   ~gdcmPixelStuff();


};

//-----------------------------------------------------------------------------
#endif
