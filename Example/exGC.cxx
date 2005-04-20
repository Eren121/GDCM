/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exGC.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/20 11:25:35 $
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
#include "gdcmDocument.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"
#include "gdcmSeqEntry.h"

typedef struct  // Maybe we should add it to gdcm ?
{
   uint8_t r;
   uint8_t g;
   uint8_t b;
} rgb8_t;


// This small application, for a given Creatis user (G.C.)
// may be taken as an example

int main(int argc, char *argv[])
{

   // we need a user friendly way for passign parameters on the command line !

   std::cout << "------------------------------------------------" << std::endl;
   std::cout << "Transforms a full gdcm-readable 'color' Dicom image "
          << " (e.g Palette Color, YBR, 3-Planes RGB) "
          << " into an 'RGB_Pixels' Dicom Image " << std::endl
          << " Blacks out any 'grey' pixel (r=g=b) "
          << " Blacks out any 'dark' pixel (r,g,b < threshold) " 
          << " "            << std::endl;
   std::cout << "------------------------------------------------" << std::endl;

   if (argc < 3)
   {
      std::cerr << "Usage :" << std::endl << 
      argv[0] << " input_dicom output_dicom threshold background" << std::endl;
      return 1;
   }

   std::string fileName = argv[1];
   std::string output   = argv[2];

   int threshold = 0;
   if (argc > 3)
      threshold        = atoi( argv[3] );
  
   int background = 0;
   if (argc > 4)
      background        = atoi( argv[4] );

// ============================================================
//   Read the input image.
// ============================================================
   // a gdcm::File contains all the Dicom Field but the Pixels Element

   gdcm::File *f1= new gdcm::File( fileName );


   std::cout << argv[1] << std::endl;

   f1 = new gdcm::File( fileName );
   if (!f1->IsReadable()) {
       std::cerr << "Sorry, " << fileName <<"  not a gdcm-readable "
                 << "DICOM / ACR File"
                 <<std::endl;
       return 0;
   }
   std::cout << " ... is readable " << std::endl;

/*
   if (!f1->IsMonochrome()) {
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
   gdcm::FileHelper *fh1 = new gdcm::FileHelper(f1);

   // (unit8_t DOESN'T mean it's mandatory for the image to be a 8 bits one) 

   uint8_t *imageData = fh1->GetImageData();

   if ( imageData == 0 )
   {
       std::cerr << "Sorry, Pixels of" << fileName <<"  are not "
                 << " gdcm-readable."  << std::endl;
       return 0;
   }

   // ------ User wants write a new image without shadow groups -------------
   // ------                              without Sequences     -------------

 
   gdcm::FileHelper *copy = new gdcm::FileHelper( output );
 
   gdcm::DocEntry *d = f1->GetFirstEntry();
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
      d = f1->GetNextEntry();
   }

   int imageSize = fh1->GetImageDataSize();
// Black up all 'grey' pixels
   int i;
   int n = 0;
   for (i = 0; i<imageSize/3; i++)
   {
      if ( ((rgb8_t *)imageData)[i].r == ((rgb8_t *)imageData)[i].g
         &&
           ((rgb8_t *)imageData)[i].r == ((rgb8_t *)imageData)[i].b )
      {
         n++;
         ((rgb8_t *)imageData)[i].r = (unsigned char)background;
         ((rgb8_t *)imageData)[i].g = (unsigned char)background;
         ((rgb8_t *)imageData)[i].b = (unsigned char)background;
      }
   }
   
    std::cout << n << " points put to black (within " 
              << imageSize/3 << ")" << std::endl;

   n = 0;
   for (i = 0; i<imageSize/3; i++)
   {
   if ( ((rgb8_t *)imageData)[i].r < threshold
     &&
        ((rgb8_t *)imageData)[i].g < threshold
     &&
        ((rgb8_t *)imageData)[i].b < threshold )
      {
         n++;
        ((rgb8_t *)imageData)[i].r = (unsigned char)background;
        ((rgb8_t *)imageData)[i].g = (unsigned char)background;
        ((rgb8_t *)imageData)[i].b = (unsigned char)background;  
      }
   }
   
   std::cout << n << " points put to black (within " 
             << imageSize/3 << ")" << std::endl; 
   // User knows the image is a 'color' one -RGB, YBR, Palette Color-
   // and wants to write it as RGB
   copy->SetImageData(imageData, imageSize);
   copy->SetWriteModeToRGB();

   copy->WriteDcmExplVR( output );

   delete f1;
   delete fh1;
   delete copy;

   exit (0);
}
