/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exAnonymizeNoLoad.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/26 16:21:55 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmCommon.h"
#include "gdcmDebug.h"

#include <iostream>

int main(int argc, char *argv[])
{  
   gdcm::File *f1;
 
   gdcm::Debug::DebugOn();
   std::cout << "------------------------------------------------" << std::endl;
   std::cout << "Anonymize a gdcm-readable Dicom image"            << std::endl;
   std::cout << "even if pixels are not gdcm readable (JPEG2000)"  << std::endl;
   std::cout << "Warning : the image is overwritten"               << std::endl;
   std::cout << "        : to preserve image integrity "
             << " use exAnonymize "                                << std::endl;
 
   if( argc < 3 )
    {
    std::cerr << "Usage " << argv[0] << " Source image.dcm  " 
              << std::endl;
    return 1;
    }

   std::string fileName       = argv[1];

// ============================================================
//   Read the input image.
// ============================================================

   std::cout << argv[1] << std::endl;

   f1 = new gdcm::File( fileName );
   if (!f1->IsReadable()) {
       std::cerr << "Sorry, " << fileName <<"  not a gdcm-readable "
           << "DICOM / ACR File"
                 <<std::endl;
   }
   std::cout << " ... is readable " << std::endl;

// ============================================================
//   No need to load the pixels in memory.
//   File will be overwritten
// ============================================================


// ============================================================
//  Choose the fields to anonymize.
// ============================================================
   // Institution name 
   f1->AddAnonymizeElement(0x0008, 0x0080, "Xanadoo"); 
   // Patient's name 
   f1->AddAnonymizeElement(0x0010, 0x0010, "Fantomas");   
   // Patient's ID
   f1->AddAnonymizeElement( 0x0010, 0x0020,"1515" );   
   // Study Instance UID
   f1->AddAnonymizeElement(0x0020, 0x000d, "9.99.999.9999" );
   // Telephone
   f1->AddAnonymizeElement(0x0010, 0x2154, "3615" );
  // Aware use will add new fields here


// No need to load the pixels.


// ============================================================
//   Overwrite the file
// ============================================================

   std::cout <<"Let's AnonymizeNoLoad " << std::endl;;

// The gdcm::File remains untouched in memory

   f1->AnonymizeNoLoad();

// No need to write the File : modif were done on disc !
// File was overwritten ...

   std::cout <<"End AnonymizeNoLoad" << std::endl;

// ============================================================
//   Remove the Anonymize list
// ============================================================  
   f1->ClearAnonymizeList();
    
   delete f1;
   return 0;
}

