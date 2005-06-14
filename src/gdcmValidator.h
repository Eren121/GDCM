/*=========================================================================
 
  Program:   gdcm
  Module:    $RCSfile: gdcmValidator.h,v $
  Language:  C++
  Date:      $Date: 2005/06/14 20:33:54 $
  Version:   $Revision: 1.1 $
 
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
 
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
 
=========================================================================*/

#ifndef GDCMVALIDATOR_H
#define GDCMVALIDATOR_H

#include "gdcmBase.h"

namespace gdcm 
{
/**
 * \brief
 */
class ElementSet;
class GDCM_EXPORT Validator : public Base
{
public:
   Validator();
   ~Validator();

   void SetInput(ElementSet *input);

};

} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
