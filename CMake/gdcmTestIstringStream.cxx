/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmTestIstringStream.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/04 16:36:43 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include <stdint.h>
#include <sstream>

int main()
{
  std::istringstream os;
  int vm;
  std::string s = "5";
  os.str(s);

  os >> vm;

  return vm;
}

