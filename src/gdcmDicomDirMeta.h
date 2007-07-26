/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirMeta.h,v $
  Language:  C++
  Date:      $Date: 2007/07/26 08:36:49 $
  Version:   $Revision: 1.23 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMDIRMETA_H
#define GDCMDICOMDIRMETA_H

#include "gdcmDicomDirObject.h"

namespace GDCM_NAME_SPACE 
{

//-----------------------------------------------------------------------------
/**
 * \brief   Meta Elements (group 0002) of a DicomDir
 */
class GDCM_EXPORT DicomDirMeta : public DicomDirObject 
{
   gdcmTypeMacro(DicomDirMeta);

public:
/// \brief Constructs a DicomDirMeta with a RefCounter
   static DicomDirMeta *New(bool empty=false) {return new DicomDirMeta(empty);}

   virtual void Print(std::ostream &os = std::cout, std::string const &indent = "" );
   virtual void WriteContent(std::ofstream *fp, FileType t, bool insideMetaElements);
   int ComputeGroup0002Length( );
   
protected:
   DicomDirMeta(bool empty=false); 
   ~DicomDirMeta();
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
