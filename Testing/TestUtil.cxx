/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestUtil.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/27 10:43:19 $
  Version:   $Revision: 1.8 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
// This test should test everything in Util, since I didn't know any other 
// way to test this class.

#include "gdcmUtil.h"
#include <iostream>

int TestUtil(int , char * [])
{
   // Time 
   std::cout << "Time:" << gdcm::Util::GetCurrentDateTime() << std::endl;

   // Processor ID
   unsigned int processorID;;
   processorID = gdcm::Util::GetCurrentProcessID();
   std::cout << "Current Processor ID " <<  processorID << std::endl;

   // MAC Adress
   std::cout << "Mac Address:" << gdcm::Util::GetMACAddress() << std::endl;

   // Unique UID test
   std::string gdcmUid;
   for (int i=0; i<10; i++)
   {
      gdcmUid = gdcm::Util::CreateUniqueUID();
      std::cout << "Current UID for gdcm " <<  gdcmUid << std::endl;
   }
   
   // DicomString test
   const char ref[] = "MONOCHROME1";
   std::string a = "MONOCHROME1";
   a += '\0';
   std::string b = "MONOCHROME1 ";
   std::string c = gdcm::Util::DicomString("MONOCHROME1");
   std::string d = "MONOCHROME1";

   if( !gdcm::Util::DicomStringEqual(a,ref) ) 
      return 1;
   if( !gdcm::Util::DicomStringEqual(b,ref) ) 
      return 1;
   if( !gdcm::Util::DicomStringEqual(c,ref) ) 
      return 1;
   if(  gdcm::Util::DicomStringEqual(d,ref) ) 
      return 1;

   return 0;
}
