/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDirList.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/14 21:52:05 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmDirList.h"

int TestDirList(int , char* [])
{  
   std::string path = GDCM_DATA_ROOT;
   std::cerr << path << std::endl;

   gdcm::DirList list(path);
   list.Print();

   return 0;
}
