/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestVR.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/27 10:43:19 $
  Version:   $Revision: 1.5 $
                                                                                
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
   gdcm::VR *tempVrDict=0;
   std::cout << "------ Test Default VR Dictionary : ----------" << std::endl;
  // Just to improve test coverage:
  // tempVrDict = new gdcm::Dict("dummyFileNameThatDoesntExist");
  // tempVrDict->Print();
  // std::cout << "----- end Test Default VR Dictionary : -----" << std::endl;

   // Lets delete it.
   delete tempVrDict;

   gdcm::VR *vr = new gdcm::VR();
 
   // There should be 16 entries
   vr->Print( std::cout );

   vr->IsVROfStringRepresentable( "PN" );
   vr->IsVROfStringRepresentable( "FD" );

   vr->IsVROfBinaryRepresentable( "FD" );
   vr->IsVROfBinaryRepresentable( "PN" );

   vr->IsVROfSequence( "" );

   return 0;
}
