/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmTestCompareTypes.cxx,v $
  Language:  C++
  Date:      $Date: 2005/05/29 23:06:41 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#define TYPE_LONG_LONG long long

typedef GDCM_TEST_COMPARE_TYPE_1 Type1;
typedef GDCM_TEST_COMPARE_TYPE_2 Type2;

void function(Type1**) {}

int main()
{
  Type2** p = 0;
  function(p);
  return 0;
}
