/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDictGroupName.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/05 10:56:24 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmDictGroupName.h"
#include "gdcmGlobal.h"
#include "gdcmCommon.h"

int TestDictGroupName(int , char *[])
{
   gdcm::DictGroupName groupName;
   groupName.Print( std::cout );

   std::cout << std::endl;
   std::cout << "Group : 0x0002 : " << groupName.GetName(0x0002) << " - " 
             << (bool)(groupName.GetName(0x0002)=="Meta Elements") << std::endl;
   std::cout << "Group : 0x0007 : " << groupName.GetName(0x0007) << " - " 
             << (bool)(groupName.GetName(0x0007)==gdcm::GDCM_UNFOUND) << std::endl;

   return groupName.GetName( 0x7fe0 ) != "";
}
