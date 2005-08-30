/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exColorToRGB.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/30 15:13:06 $
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
#include "gdcmDocument.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"
#include "gdcmSeqEntry.h"

#include <stdlib.h> // for exit

int main(int argc, char *argv[])
{

   std::cout << "------------------------------------------------" << std::endl;
   std::cout << "Tranforms a full gdcm-readable 'color' Dicom image "
          << " (e.g Palette Color, YBR, 3-Planes RGB) "
          << " into an 'RGB_Pixels' Dicom Image "
          << " "            << std::endl;

   if (argc < 3)
   {
      std::cerr << "Usage :" << std::endl << 
      argv[0] << " input_dicom output_dicom" << std::endl;
      return 1;
   }

   std::string fileName = argv[1];
   std::string output   = argv[2];


// ============================================================
//   Read the input image.
// ============================================================
   // a gdcm::File contains all the Dicom Field but the Pixels Element

   std::cout << argv[1] << std::endl;

   gdcm::File *f = new gdcm::File();
   f->SetLoadMode( gdcm::LD_ALL);
   f->SetFileName( fileName );
   bool res = f->Load();        

   if (!res) {
       std::cerr << "Sorry, " << fileName <<"  not a gdcm-readable "
                 << "DICOM / ACR File"
                 <<std::endl;
       return 0;
   }
   std::cout << " ... is readable " << std::endl;

/*
   if (!f->IsMonochrome()) {
       std::cerr << "Sorry, " << fileName <<"  not a 'color' File "
                 << " "
                 <<std::endl;
       return 0;
   }
*/

// ============================================================
//   Load the pixels in memory.
// ============================================================

   // We need a gdcm::FileHelper, since we want to load the pixels        
   gdcm::FileHelper *fh = new gdcm::FileHelper(f);

   // (unit8_t DOESN'T mean it's mandatory for the image to be a 8 bits one) 

   uint8_t *imageData = fh->GetImageData();

   if ( imageData == 0 )
   {
       std::cerr << "Sorry, Pixels of" << fileName <<"  are not "
                 << " gdcm-readable."       << std::endl;
       return 0;
   }
 

   // ------ User wants write a new image without shadow groups -------------
   // ------                              without Sequences     -------------

 
   gdcm::FileHelper *copy = new gdcm::FileHelper( );
   copy->SetFileName( output );
   copy->Load();
 
   gdcm::DocEntry *d = f->GetFirstEntry();
   while(d)
   {
      // We skip SeqEntries, since user cannot do much with them
      if ( !(dynamic_cast<gdcm::SeqEntry*>(d))
      // We skip Shadow Groups, since nobody knows what they mean
           && !( d->GetGroup()%2 ) )
      { 

         if ( gdcm::BinEntry *b = dynamic_cast<gdcm::BinEntry*>(d) )
         {              
            copy->GetFile()->InsertBinEntry( b->GetBinArea(),b->GetLength(),
                                             b->GetGroup(),b->GetElement(),
                                             b->GetVR() );
         }
         else if ( gdcm::ValEntry *v = dynamic_cast<gdcm::ValEntry*>(d) )
         {   
             copy->GetFile()->InsertValEntry( v->GetValue(),
                                              v->GetGroup(),v->GetElement(),
                                              v->GetVR() ); 
         }
         else
         {
          // We skip gdcm::SeqEntries
         }
      }
      d = f->GetNextEntry();
   }
 
   // User knows the image is a 'color' one -RGB, YBR, Palette Color-
   // and wants to write it as RGB
   copy->SetImageData(imageData, fh->GetImageDataSize());
   copy->SetWriteModeToRGB();

   copy->WriteDcmExplVR( output );


   delete f;
   delete fh;
   delete copy;

   exit (0);
}

