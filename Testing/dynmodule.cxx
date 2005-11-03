/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: dynmodule.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/03 16:19:57 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmFile.h"

#ifdef _WIN32
#  define __ELX_DLLEXPORT __declspec(dllexport)
#else
#  define __ELX_DLLEXPORT
#endif

extern "C" __ELX_DLLEXPORT void afunc(void)
{
  std::cerr << "Hello, we are in the dll-function afunc!" << std::endl;

  // Create an instance of a gdcm class. This makes sure
  // that the problematic gdcm-library is linked.
  gdcm::File *file = gdcm::File::New();
  
  std::cerr << "End of function afunc"  << std::endl; 
  
}


