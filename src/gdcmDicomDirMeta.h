/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirMeta.h,v $
  Language:  C++
  Date:      $Date: 2004/08/26 15:29:52 $
  Version:   $Revision: 1.6 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMDIRMETA_H
#define GDCMDICOMDIRMETA_H

#include "gdcmObject.h"

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirMeta : public gdcmObject 
{
public:
   gdcmDicomDirMeta(TagDocEntryHT *ptagHT); 

   ~gdcmDicomDirMeta();

   virtual void Print(std::ostream &os = std::cout);
   virtual void Write(FILE * fp, FileType t);
};

//-----------------------------------------------------------------------------
#endif
