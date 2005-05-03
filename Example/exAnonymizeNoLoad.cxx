/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exAnonymizeNoLoad.cxx,v $
  Language:  C++
  Date:      $Date: 2005/05/03 10:40:28 $
  Version:   $Revision: 1.6 $
                                                                                
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

   std::cout << "Anonymize a gdcm-readable Dicom image"            << std::endl;
   std::cout << "even if pixels are not gdcm readable (JPEG2000)"  << std::endl;
   std::cout << "Warning : the image is overwritten"               << std::endl;
   std::cout << "        : to preserve image integrity,"
             << " use exAnonymize "                                << std::endl;
   std::cout << "------------------------------------------------" << std::endl;
 
   if( argc < 2 )
    {
    std::cerr << "Usage " << argv[0] << " Source Document  " 
              << std::endl;
    return 1;
    }

   std::string fileName       = argv[1];

// ============================================================
//   Read the input image.
// ============================================================
   bool res;

   f1 = new gdcm::File( );
   f1->SetLoadMode(NO_SEQ - NO_SHADOW);
   res = f1->Load(fileName);
   // IsReadable() is no usable here, because we deal with
   // any kind of gdcm::Readable *document* (no only gdcm::File)
   if (res) {
       std::cout <<std::endl
           << "Sorry, " << fileName <<"  not a gdcm-readable "
           << "DICOM / ACR Document"
           <<std::endl;
        return 1;
   }
   std::cout << argv[1] << " is readable " << std::endl;

// ============================================================
//   No need to load the pixels in memory.
//   File will be overwritten
// ============================================================


// ============================================================
//  Choose the fields to anonymize.
// ============================================================
   // Institution name 
   f1->AddAnonymizeElement( 0x0008, 0x0080, "Xanadoo" ); 
   // Patient's name 
   f1->AddAnonymizeElement( 0x0010, 0x0010, "Fantomas" );   
   // Patient's ID
   f1->AddAnonymizeElement( 0x0010, 0x0020,"1515" );
   // Patient's Birthdate
   f1->AddAnonymizeElement( 0x0010, 0x0030,"11.11.1111" );
   // Patient's Adress
   f1->AddAnonymizeElement( 0x0010, 0x1040,"Sing-sing" );
   // Patient's Mother's Birth Name
   f1->AddAnonymizeElement( 0x0010, 0x1060,"Vampirella" );   
   // Study Instance UID
   f1->AddAnonymizeElement( 0x0020, 0x000d, "9.99.999.9999" );
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

