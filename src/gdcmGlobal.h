/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmGlobal.h,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:07 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMGLOBAL_H
#define GDCMGLOBAL_H

#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmDictSet.h"
#include "gdcmDicomDirElement.h"

//-----------------------------------------------------------------------------
/**
 * \brief   This class contains all globals elements that might be
 *          instanciated only once (singletons).
 */
class GDCM_EXPORT gdcmGlobal {
public:
   gdcmGlobal(void);
   ~gdcmGlobal();

   static gdcmDictSet *GetDicts(void);
   static gdcmVR *GetVR(void);
   static gdcmTS *GetTS(void);
   static gdcmDicomDirElement *GetDicomDirElements(void);

private:
   static gdcmDictSet *Dicts; 
   static gdcmVR *VR;
   static gdcmTS *TS; 
   static gdcmDicomDirElement *ddElem;
};

//-----------------------------------------------------------------------------
#endif
