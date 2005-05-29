/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmTestConvertTypes.cxx,v $
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

#define TYPE_UNSIGNED___INT64 unsigned __int64

typedef GDCM_TEST_CONVERT_TYPE_TO TypeTo;
typedef GDCM_TEST_CONVERT_TYPE_FROM TypeFrom;

void function(TypeTo& l, TypeFrom const& r)
{
  l = static_cast<TypeTo>(r);
}

int main()
{
  TypeTo tTo = TypeTo();
  TypeFrom tFrom = TypeFrom();
  function(tTo, tFrom);
  return 0;
}
