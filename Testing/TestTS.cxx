/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestTS.cxx,v $
  Language:  C++
  Date:      $Date: 2004/11/16 04:28:20 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmTS.h"

int TestTS(int , char *[])
{
   gdcm::TS ts;
   // There should be 150 entries
   ts.Print( std::cout );

   return ts.GetValue( "" ) != gdcm::GDCM_UNFOUND;
}
