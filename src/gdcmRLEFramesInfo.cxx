/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmRLEFramesInfo.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/12 04:35:47 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmRLEFramesInfo.h"

namespace gdcm 
{

RLEFramesInfo::~RLEFramesInfo()
{
   for(RLEFrameList::iterator it = Frames.begin(); it != Frames.end(); ++it)
   {
      delete (*it);
   }
   Frames.clear();
}
} // end namespace gdcm
