/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmGlobal.h,v $
  Language:  C++
  Date:      $Date: 2004/12/03 20:16:58 $
  Version:   $Revision: 1.5 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMGLOBAL_H
#define GDCMGLOBAL_H

#include "gdcmCommon.h"

namespace gdcm 
{
class DictSet;
class VR;
class TS;
class DicomDirElement;
//-----------------------------------------------------------------------------
/**
 * \brief   This class contains all globals elements that might be
 *          instanciated only once (singletons).
 */
class GDCM_EXPORT Global
{
public:
   Global();
   ~Global();

   static DictSet *GetDicts();
   static VR *GetVR();
   static TS *GetTS();
   static DicomDirElement *GetDicomDirElements();

private:
   static DictSet *Dicts; 
   static VR *ValRes;
   static TS *TranSyn; 
   static DicomDirElement *ddElem;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
