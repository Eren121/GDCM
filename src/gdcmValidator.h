/*=========================================================================
 
  Program:   gdcm
  Module:    $RCSfile: gdcmValidator.h,v $
  Language:  C++
  Date:      $Date: 2005/11/28 15:20:34 $
  Version:   $Revision: 1.4 $
 
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
 
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
 
=========================================================================*/

#ifndef GDCMVALIDATOR_H
#define GDCMVALIDATOR_H

#include "gdcmCommandManager.h"

namespace gdcm 
{
/**
 * \brief Class to perform some verifications on a gdcm::Document
 */
class ElementSet;
class GDCM_EXPORT Validator : public CommandManager
{
   gdcmTypeMacro(Validator);
   
public:

/// \brief Constructs a Validator with a RefCounter
   static Validator *New() {return new Validator();}
   void SetInput(ElementSet *input);

protected:
   Validator();
   ~Validator();
};

} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
