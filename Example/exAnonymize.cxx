/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exAnonymize.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/06 14:56:22 $
  Version:   $Revision: 1.1 $
                                                                                
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

#include <iostream>

int main(int argc, char *argv[])
{  

   gdcm::File *f1;
     
   if( argc < 3 )
    {
    std::cerr << "Usage " << argv[0] << " Source image.dcm  " 
              << " Output image.dcm " << std::endl;
    return 1;
    }

   std::string fileName       = argv[1];
   std::string outputFileName = argv[2];

// --------------------- we read the input image

   std::cout << argv[1] << std::endl;

   f1 = new gdcm::File( fileName );
   if (!f1->IsReadable()) {
       std::cerr << "Sorry, " << fileName <<"  not a Readable DICOM / ACR File"
                 <<std::endl;
       return 0;
   }

// ============================================================
//   Don't load the pixels in memory.
//   Overwrite the file
// ============================================================

// First, we set values to replace the ones we want to hide
  
   // Patient's name 
   f1->AddAnonymizeElement(0x0010, 0x0010, "Tartempion");   
   // Patient's ID
   f1->AddAnonymizeElement( 0x0010, 0x0020,"007" );   
   // Study Instance UID
   f1->AddAnonymizeElement(0x0020, 0x000d, "6.66.666.6666" );

// --------------------- we overwrite the file

// No need to load the pixels.
// The gdcm::File remains untouched in memory

   f1->AnonymizeNoLoad();

// No need to write the File : modif were done on disc !

// ============================================================
//   Load the pixels in memory.
//   Write a new file
// ============================================================

   // We need a gdcm::FileHelper, since we want to load the pixels        
   gdcm::FileHelper *fh1 = new gdcm::FileHelper(f1);

// --- Don't forget to load the Pixels ...
// (unit8_t DOESN'T mean it's mandatory for the image to be a 8 bits one) 

   //uint8_t *imageData= fh1->GetImageData();
   fh1->GetImageData();

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

   f1->AnonymizeFile();
 
   fh1->WriteDcmExplVR(outputFileName);

  //If we reach here everything is fine, return 0 then:
  
   f1->ClearAnonymizeList();    
   delete f1;
   delete fh1; 
  return 0;
}

