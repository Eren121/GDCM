/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirImage.h,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:06 $
  Version:   $Revision: 1.9 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMIMAGE_H
#define GDCMDICOMIMAGE_H

#include "gdcmDicomDirObject.h"

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirImage : public gdcmDicomDirObject
{
public:
   gdcmDicomDirImage(gdcmSQItem *s, TagDocEntryHT *ptagHT); 
   gdcmDicomDirImage(TagDocEntryHT *ptagHT); 

   ~gdcmDicomDirImage();

   virtual void Print(std::ostream &os = std::cout);
};

//-----------------------------------------------------------------------------
#endif
