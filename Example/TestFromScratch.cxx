/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestFromScratch.cxx,v $
  Language:  C++
  Date:      $Date: 2004/12/10 16:48:37 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmHeader.h"
#include "gdcmFile.h"
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
   dbg.SetDebug(-1);
   // Doesn't link properly:
   //gdcm::Debug::GetReference().SetDebug(1);

   std::string filename = argv[1];
   //gdcm::File *f1 = new gdcm::File( "/home/malaterre/Creatis/gdcmData/012345.002.050.dcm" );
   gdcm::File *f1 = new gdcm::File( filename );
   gdcm::Header *h1 = f1->GetHeader();

   int dataSize = f1->GetImageDataSize();
   std::cout << "DataSize:      " << dataSize << std::endl;
   // Since we know the image is 16bits:
   uint8_t* imageData = f1->GetImageData();
 
   // Hopefully default to something
   gdcm::Header *h2 = new gdcm::Header();

   h1->Initialize();
   gdcm::DocEntry* d = h1->GetNextEntry();

   // Copy of the header content
   while(d)
   {
      if ( gdcm::BinEntry* b = dynamic_cast<gdcm::BinEntry*>(d) )
      {
         // We skip bin entries
      }
      else if ( gdcm::ValEntry* v = dynamic_cast<gdcm::ValEntry*>(d) )
      {   
          h2->ReplaceOrCreateByNumber( 
                              v->GetValue(),
                              v->GetGroup(), 
                              v->GetElement(),
                              v->GetVR() ); 
      }
      else
      {
       // We skip pb of SQ recursive exploration
      }

      d = h1->GetNextEntry();
   }
   //h2->Print( std::cout );

   gdcm::File *f2 = new gdcm::File( h2 );
   f2->SetImageData(imageData, dataSize);

   f2->SetWriteTypeToDcmExplVR();
   f2->Write( "output.dcm" );

   delete f1;
   delete f2;
   delete h2;

   return 0;
}
