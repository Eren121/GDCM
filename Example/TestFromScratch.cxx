/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestFromScratch.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/25 15:44:22 $
  Version:   $Revision: 1.14 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmDictEntry.h"
#include "gdcmDocEntry.h"
#include "gdcmBinEntry.h"
#include "gdcmValEntry.h"
#include "gdcmDebug.h"

// The whole purpose of this example is to create a DICOM from scratch
// well almost the only thing we allow use is to copy the string entry
// From the previous image read.

int main(int argc, char *argv[])
{
   if( argc < 2 )
   {
      std::cerr << "Usage: " << argv[0] << " Image.dcm" << std::endl;
      return 1;
   }

   // Doesn't seems to do anything:
   gdcm::Debug::SetDebugOn();

   // Doesn't link properly:
   //gdcm::Debug::GetReference().SetDebug(1);

   std::string filename = argv[1];
   gdcm::FileHelper *f1 = new gdcm::FileHelper( filename );
   gdcm::File *h1 = f1->GetFile();

   int dataSize = f1->GetImageDataSize();
   std::cout << "DataSize:      " << dataSize << std::endl;
   // Since we know the image is 16bits:
   uint8_t* imageData = f1->GetImageData();
 
   // Hopefully default to something
   gdcm::File *h2 = new gdcm::File();

   // Copy of the header content
   gdcm::DocEntry* d = h1->GetFirstEntry();
   while(d)
   {
      if ( gdcm::ValEntry* v = dynamic_cast<gdcm::ValEntry*>(d) )
      {   
         // Do not bother with field from private dict
         if( v->GetName() != "gdcm::Unknown" )
         {  
            h2->InsertValEntry( v->GetValue(),
                                v->GetGroup(),v->GetElement(),
                                v->GetVR() ); 
         }
      }
      //else
         // We skip pb of SQ recursive exploration
         // We skip bin entries

      d = h1->GetNextEntry();
   }
   h2->Print( std::cout );

   gdcm::FileHelper *f2 = new gdcm::FileHelper( h2 );
   f2->SetImageData(imageData, dataSize);

   f2->SetWriteTypeToDcmExplVR();
   f2->Write( "output.dcm" );

   delete f1;
   delete f2;
   delete h2;

   return 0;
}
