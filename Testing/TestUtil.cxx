/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestUtil.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/06 19:10:07 $
  Version:   $Revision: 1.6 $
                                                                                
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
   // Unique UID test
   std::cout << gdcm::Util::CreateUniqueUID("") << std::endl;
   
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

   // MAC Adress
   std::cout << "Mac Address:" << gdcm::Util::GetMACAddress() << std::endl;

   return 0;
}
