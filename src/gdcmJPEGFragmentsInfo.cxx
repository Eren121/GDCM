/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJPEGFragmentsInfo.cxx,v $
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

#include "gdcmJPEGFragmentsInfo.h"

gdcmJPEGFragmentsInfo::~gdcmJPEGFragmentsInfo()
{
   for(JPEGFragmentsList::iterator it  = Fragments.begin();
                                   it != Fragments.end();
                                 ++it )
   {
      delete (*it);
   }
   Fragments.clear();
}
