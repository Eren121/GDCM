/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestUtil.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/14 16:46:15 $
  Version:   $Revision: 1.12 $
                                                                                
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
#include "gdcmDebug.h"
#include <iostream>

int TestUtil(int , char *[])
{
   // CreateCleanString
   std::string x = "a#@-bc\tdef";
   std::string y = gdcm::Util::CreateCleanString(x);
   std::cout << "[" << x <<"] --> [" << y <<"]" << std::endl;

   // CountSubstring : substring id "#@-"
   x = "abcd#@-wyz*@-lmn#@-uvw-#@ijk";
   std::cout << "count '#@-' in [" << x << "] : " 
             << gdcm::Util::CountSubstring(x, "#@-") << std::endl;

   // Tokenize : tokens are '#', '@', '-'
   std::vector<std::string> tokens;
   gdcm::Util::Tokenize (x, tokens, "#@-");
   for (unsigned int ui=0; ui<tokens.size();ui++)
   {
      std::cout << "[" << tokens[ui] << "]" << std::endl;
   }
   tokens.clear();

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

// ----------------------------------------------------------
// Let's test gdcm::Debug, now.
    std::cout << "GetDebugFlag : " << gdcm::Debug::GetDebugFlag() <<std::endl;
    gdcm::Debug::SetDebugFilename ("DummyFileNameToWriteTo.txt");
    std::cout << "We set a Debug file"   <<std::endl;
    if ( !gdcm::Debug::GetDebugFlag() )
    {
       std::cout << "Debug Flag should be TRUE... " << std::endl;
       return 1;
    }
    std::cout << "GetDebugFlag : " << gdcm::Debug::GetDebugFlag()<<std::endl;
    gdcm::Debug::SetDebugFlag ( false );
    std::cout << "GetDebugFlag : " << gdcm::Debug::GetDebugFlag()<<std::endl;
    gdcm::Debug::SetDebugFilename ("DummyFileNameToWriteTo2.txt");    

   return 0;
}
