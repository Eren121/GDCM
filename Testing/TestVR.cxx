/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestVR.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/25 14:52:31 $
  Version:   $Revision: 1.11 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmVR.h"
#include "gdcmDebug.h"

int TestVR(int , char *[])
{
   int error = 0;
   gdcm::VR *vr = gdcm::VR::New();
 
   gdcm::Debug::DebugOn();

   // There should be 16 entries ...
   vr->Print( std::cout );

   // Valid VR
   if( !vr->IsValidVR( "PN" ) )
   {
      std::cerr << "'PN' is not a valid VR" << std::endl;
      error++;
   }
   if( !vr->IsValidVR( "FD" ) )
   {
      std::cerr << "'FD' is not a valid VR" << std::endl;
      error++;
   }
   if( vr->IsValidVR( "" ) )
   {
      std::cerr << "'' is a valid VR" << std::endl;
      error++;
   }
   if( vr->IsValidVR( "  " ) )
   {
      std::cerr << "'  ' is a valid VR" << std::endl;
      error++;
   }
   if( vr->IsValidVR( "\000/" ) )
   {
      std::cerr << "' /' is a valid VR" << std::endl;
      error++;
   }
   if( vr->IsValidVR( gdcm::GDCM_VRUNKNOWN ) )
   {
      std::cerr << "'  ' is a valid VR" << std::endl;
      error++;
   }

   // String representable
   if( !vr->IsVROfStringRepresentable( "PN" ) )
   {
      std::cerr << "'PN' is not a string representable" << std::endl;
      error++;
   }
   if( vr->IsVROfStringRepresentable( "FD" ) )
   {
      std::cerr << "'FD' is a string representable" << std::endl;
      error++;
   }

   // Binary representable
   if( !vr->IsVROfBinaryRepresentable( "FD" ) )
   {
      std::cerr << "FD is not a binary representable" << std::endl;
      error++;
   }
   if( vr->IsVROfBinaryRepresentable( "PN" ) )
   {
      std::cerr << "'PN' is a binary representable" << std::endl;
      error++;
   }

   // Sequence
   if( vr->IsVROfSequence( "" ) )
   {
      std::cerr << "'' is a sequence" << std::endl;
      error++;
   }
   if( !vr->IsVROfSequence( "SQ" ) )
   {
      std::cerr << "'SQ' is not a sequence" << std::endl;
      error++;
   }

   vr->Delete();
   return error;
}
