/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirImage.h,v $
  Language:  C++
  Date:      $Date: 2007/05/23 14:18:08 $
  Version:   $Revision: 1.19 $
                                                                                
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

namespace GDCM_NAME_SPACE 
{
//-----------------------------------------------------------------------------
/**
 * \brief   describes an IMAGE within a SERIE
 * (DicomDirSerie) of a given DICOMDIR (DicomDir)
 */
 class GDCM_EXPORT DicomDirImage : public DicomDirObject
{
   gdcmTypeMacro(DicomDirImage);

public:
/// \brief Constructs a DicomDirImage with a RefCounter
   static DicomDirImage *New(bool empty=false) {return new DicomDirImage(empty);}

   void Print(std::ostream &os = std::cout, std::string const &indent = "" );

protected:
   DicomDirImage(bool empty=false); 
   ~DicomDirImage();
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
