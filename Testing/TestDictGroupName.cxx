/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDictGroupName.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/08 13:39:57 $
  Version:   $Revision: 1.3 $
                                                                                
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

#include <iomanip>

int CompareDictGroupName(gdcm::DictGroupName &groupName,
                         uint16_t group,std::string ref)
{
   std::string val = groupName.GetName(group);
   std::cout << "Group : 0x" << std::hex << std::setw(4) << group 
             << std::dec << " : " << val << " - "
             << (bool)(val==ref) << std::endl;

   return val!=ref;
}

int TestDictGroupName(int , char *[])
{
   gdcm::DictGroupName groupName;
   groupName.Print( std::cout );

   int ret = 0;

   std::cout << std::endl;
   ret += CompareDictGroupName(groupName,0x0002,"Meta Elements");
   ret += CompareDictGroupName(groupName,0x7fe0,"Pixels");
   ret += CompareDictGroupName(groupName,0x0007,gdcm::GDCM_UNFOUND);

   return ret;
}
