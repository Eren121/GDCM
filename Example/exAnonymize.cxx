/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exAnonymize.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/02 17:19:45 $
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
   std::cout << "Anonymize a full gdcm-readable  Dicom image"      << std::endl;
   std::cout << " Warning : probably segfaults if pixels are not "
          << " gdcm readable. Use exAnonymizeNoLoad"            << std::endl;

   if( argc < 3 )
    {
    std::cerr << "Usage " << argv[0] << " Source image.dcm  " 
              << " Output image.dcm " << std::endl;
    return 1;
    }

   std::string fileName       = argv[1];
   std::string outputFileName = argv[2];

// ============================================================
//   Read the input image.
// ============================================================

   std::cout << argv[1] << std::endl;

   f1 = new gdcm::File( fileName );
   if (!f1->IsReadable()) {
       std::cerr << "Sorry, " << fileName <<"  not a gdcm-readable "
           << "DICOM / ACR File"
                 <<std::endl;
       return 0;
   }
   std::cout << " ... is readable " << std::endl;


// ============================================================
//   Load the pixels in memory.
// ============================================================

   // We need a gdcm::FileHelper, since we want to load the pixels        
   gdcm::FileHelper *fh1 = new gdcm::FileHelper(f1);

   // (unit8_t DOESN'T mean it's mandatory for the image to be a 8 bits one) 

   uint8_t *imageData = fh1->GetImageData();

   if ( imageData == 0 )
   {
       std::cerr << "Sorry, Pixels of" << fileName <<"  are not "
           << " gdcm-readable."       << std::endl
                 << "Use exAnonymizeNoLoad" << std::endl;
  
       return 0;
   } 

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

// The gdcm::File is modified in memory

   f1->AnonymizeFile();

// ============================================================
//   Write a new file
// ============================================================

   fh1->WriteDcmExplVR(outputFileName);
   std::cout <<"End Anonymize" << std::cout;

// ============================================================
//   Remove the Anonymize list
// ============================================================  
   f1->ClearAnonymizeList();
    
   delete f1;
   delete fh1; 
   return 0;
}

