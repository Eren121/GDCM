/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDicomDirElement.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/26 16:43:10 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmDicomDirElement.h"

#include <iostream>

int TestDicomDirElement(int , char *[])
{
   gdcm::DicomDirElement *ddElt = new gdcm::DicomDirElement();
   if (ddElt == 0)
   {
   std::cout << "new DicomDirElement failed" << std::endl;
   return 1;
   }  
   ddElt->Print( std::cout );

   delete ddElt;
   return 0;
}
