/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirImage.h,v $
  Language:  C++
  Date:      $Date: 2004/08/26 15:29:52 $
  Version:   $Revision: 1.7 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMIMAGE_H
#define GDCMDICOMIMAGE_H

#include "gdcmObject.h"

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirImage : public gdcmObject
{
public:
   gdcmDicomDirImage(gdcmSQItem *s, TagDocEntryHT *ptagHT); 
   gdcmDicomDirImage(TagDocEntryHT *ptagHT); 

   ~gdcmDicomDirImage();

   virtual void Print(std::ostream &os = std::cout);
};

//-----------------------------------------------------------------------------
#endif
