/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestVR.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/06 20:03:26 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmVR.h"

int TestVR(int , char *[])
{
   gdcm::VR vr;
   // There should be 16 entries
   vr.Print( std::cout );

   vr.IsVROfStringRepresentable( "" );
   vr.IsVROfBinaryRepresentable( "" );
   vr.IsVROfSequence( "" );

   return 0;
}
