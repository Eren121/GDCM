/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirPrivate.h,v $
  Language:  C++
  Date:      $Date: 2007/03/26 13:32:54 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMDIRPRIVATE_H
#define GDCMDICOMDIRPRIVATE_H

#include "gdcmDicomDirObject.h"

namespace gdcm 
{

/**
 * \brief   describes a PRIVATE  within a within a SERIE
 * (DicomDirSerie) of a given DICOMDIR (DicomDir)
 */
class GDCM_EXPORT DicomDirPrivate : public DicomDirObject 
{
   gdcmTypeMacro(DicomDirPrivate);

public:
/// \brief Constructs a DicomDirPrivate with a RefCounter
   static DicomDirPrivate *New(bool empty=false) {return new DicomDirPrivate(empty);}

   void Print( std::ostream &os = std::cout, std::string const &indent = "" );
  // void WriteContent( std::ofstream *fp, FileType t );

protected:
   DicomDirPrivate(bool empty=false); 
   ~DicomDirPrivate();
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
