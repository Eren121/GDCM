/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirImage.h,v $
  Language:  C++
  Date:      $Date: 2005/01/16 04:50:41 $
  Version:   $Revision: 1.14 $
                                                                                
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

namespace gdcm 
{
//-----------------------------------------------------------------------------
/**
 * \ingroup DicomDirImage
 * \brief   describes an IMAGE within a SERIE
 * (DicomDirSerie) of a given DICOMDIR (DicomDir)
 */
 class GDCM_EXPORT DicomDirImage : public DicomDirObject
{
public:
   DicomDirImage(); 
   ~DicomDirImage();

   void Print(std::ostream &os = std::cout, std::string const & indent = "" );
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
