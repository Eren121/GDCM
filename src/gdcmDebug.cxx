/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDebug.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/10 17:17:52 $
  Version:   $Revision: 1.16 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include <iostream>
#include "gdcmDebug.h"

namespace gdcm 
{

/// warning message level to be displayed
static int DebugFlag = 0;
//-----------------------------------------------------------------------------
/**
 * \brief   Accessor
 * @param   flag Set the debug flag
 */ 
void Debug::SetDebugFlag (int flag) 
{
   DebugFlag = flag;
}

/**
 * \brief   Accessor
 * @param   level Get the debug flag
 */ 
int Debug::GetDebugFlag ()
{
   return DebugFlag;
}

} // end namespace gdcm


