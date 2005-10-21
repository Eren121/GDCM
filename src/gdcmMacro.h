/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmMacro.h,v $
  Language:  C++
  Date:      $Date: 2005/10/21 15:34:56 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMMACRO_H
#define GDCMMACRO_H

//-----------------------------------------------------------------------------
#define gdcmTypeMacro(type) \
   private : \
      type(type &); /* Not implemented */ \
      type &operator=(type &) /* Not implemented */

//-----------------------------------------------------------------------------
#endif
