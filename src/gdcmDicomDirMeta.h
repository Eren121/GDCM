/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirMeta.h,v $
  Language:  C++
  Date:      $Date: 2004/11/25 15:46:11 $
  Version:   $Revision: 1.11 $
                                                                                
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
namespace gdcm 
{

//-----------------------------------------------------------------------------
class GDCM_EXPORT DicomDirMeta : public DicomDirObject 
{
public:
   DicomDirMeta(TagDocEntryHT *ptagHT); 

   ~DicomDirMeta();

   virtual void Print(std::ostream &os = std::cout);
   virtual void WriteContent(std::ofstream * fp, FileType t);
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
