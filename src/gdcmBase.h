/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmBase.h,v $
  Language:  C++
  Date:      $Date: 2005/01/11 15:15:37 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMBASE_H
#define GDCMBASE_H

#include "gdcmCommon.h"
#include <iostream>

namespace gdcm 
{
//-----------------------------------------------------------------------------
/**
 * \brief Base class of all gdcm classes
 *
 * Contains the Print related methods :
 *  - Print 
 *  - SetPrintLevel / GetPrintLevel 
 */
class GDCM_EXPORT Base
{
public:
   Base( );
   virtual ~Base();

   virtual void Print(std::ostream &os = std::cout); 

   /// \brief Sets the print level for the Dicom Header Elements
   /// \note 0 for Light Print; 1 for 'medium' Print, 2 for Heavy
   void SetPrintLevel(int level) { PrintLevel = level; };

   /// \brief Gets the print level for the Dicom Header Elements
   int GetPrintLevel() { return PrintLevel; };

protected:
   /// \brief Amount of printed details for each Header Entry (Dicom Element):
   /// 0 : stands for the least detail level.
   int PrintLevel;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
