/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exReadWriteFile.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/03 15:44:20 $
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
#include "gdcmDocument.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"
#include "gdcmSeqEntry.h"

// ----- WARNING ----- WARNING ----- WARNING ----- WARNING ----- WARNING ---
//
// This program is NOT intendend to be run as is.
//
// It just shows a set of possible uses.
// User MUST read it, 
//           comment out the useless parts
//           invoke it with an ad hoc image(*)
//           check the resulting image      
// 
// (*) For samples, user can refer to gdcmData
//         and read README.txt file
//
// ----- WARNING ----- WARNING ----- WARNING ----- WARNING ----- WARNING ---

 
int main(int argc, char *argv[])
{
   if (argc < 3)
   {
      std::cerr << "Usage :" << std::endl << 
      argv[0] << " input_dicom output_dicom" << std::endl;
      return 1;
   }

   std::string filename = argv[1];
   std::string output   = argv[2];

   // First, let's create a gdcm::File
   // that will contain all the Dicom Field but the Pixels Element

   gdcm::File *e1= new gdcm::File( filename );


   // Ask content to be printed
   std::cout << std::endl
             << "--- Standard Print -------------------------------------------"
             << std::endl;
   e1->SetPrintLevel(2);   // to have a nice output
   //e1->SetPrintLevel(1); 
   e1->Print();            // user may comment out if too much verbose


  // Field by field Printing
  
   std::cout << std::endl
             << "--- Display only human readable values -----------------------"
             << std::endl;

   gdcm::ValEntry *valEntry;
   uint16_t group;
   uint16_t elem;
   int offset;
   std::string value;
   std::string vr;   // value representation
   std::string vm;   // value multiplicity
   std::string name; // held in the Dicom Dictionary


   gdcm::DocEntry *d = e1->GetFirstEntry();
   while( d )
   {
      // We skip SeqEntries, since user cannot do much with them
      if ( !(dynamic_cast<gdcm::SeqEntry*>(d))
      // We skip Shadow Groups, since nobody knows what they mean
           && !( d->GetGroup()%2 )
     // we skip BinEntries, since their content is not human-readable
           && !dynamic_cast<gdcm::BinEntry*>(d) )
     {      
         // If user just 'wants to see'
         //d->Print();
         //std::cout << std::endl;

         // If user wants to get info about the entry
         // (he is sure, here that DocEntry is a ValEntry)
         valEntry = dynamic_cast<gdcm::ValEntry*>(d);
         // Let's be carefull -maybe he commented out some previous line-
         if (!valEntry)
            continue;

         value  = valEntry->GetValue();
         group  = valEntry->GetGroup();
         elem   = valEntry->GetElement();
         vr     = valEntry->GetVR();
         // user wants really to know everything about entry!
         vm     = valEntry->GetVM();
         name   = valEntry->GetName();
         offset = valEntry->GetOffset();

         std::cout //<< std::hex << group << "," << elem 
          << valEntry->GetKey()
               << "     VR :[" << vr    << "] VM : [" << vm 
             << "] name : [" << name  << "]"
             << " value : [" << value << "]" 
 
              << std::endl;
      }
      d = e1->GetNextEntry();
   }

   std::cout << std::endl
             << "--- Use pre-defined acessors ---------------------------------"
             << std::endl;
 
   // ------ some pre-defined acessors may supply usefull informations ----

   // about Image
   int linesNumber   = e1->GetYSize();
   int rawsNumber    = e1->GetXSize();
   int framesNumber  = e1->GetYSize();// defaulted to 1 if not found

   std::cout << "lines : "   << linesNumber  << " columns : " << rawsNumber
             << " frames : " << framesNumber << std::endl;
 
   // about Pixels
   int pixelSize         = e1->GetPixelSize(); 
   std::string pixelType = e1->GetPixelType();
   bool isSigned         = e1->IsSignedPixelData();
  
   std::cout << "pixelSize : "   << pixelSize  << " pixelType : " << pixelType
             << " signed : "     << isSigned   << std::endl;
 
   // about pixels, too.
   // Better you forget these ones
  
   std::cout << "GetBitsStored()"      << e1->GetBitsStored()      << std::endl;
   std::cout << "GetBitsAllocated()"   << e1->GetBitsAllocated()   << std::endl;
   std::cout << "GetHighBitPosition()" << e1->GetHighBitPosition() << std::endl;

   std::cout << "GetSamplesPerPixel()"     
          << e1->GetSamplesPerPixel()     << std::endl;
   std::cout << "GetPlanarConfiguration()" 
          << e1->GetPlanarConfiguration() << std::endl; 
 
   // about 'image geography'
 
   float xs = e1->GetXSpacing();
   float ys = e1->GetYSpacing();
   float zs = e1->GetZSpacing();  // defaulted to 1.0 if not found

   float xo = e1->GetXOrigin();
   float yo = e1->GetYOrigin();
   float zo = e1->GetZOrigin();

   std::cout << "GetXSpacing()"     << xs      << std::endl;
   std::cout << "GetYSpacing()"     << ys      << std::endl;
   std::cout << "GetXSpacing()"     << zs      << std::endl;

   std::cout << "GetXOrigin()"      << xo      << std::endl;
   std::cout << "GetYOrigin()"      << yo      << std::endl;
   std::cout << "GetZOrigin()"      << zo      << std::endl;

   // about its way to store colors (if user is aware)

   // checks Photometric Interpretation
   std::cout << "IsMonochrome()"   << e1->IsMonochrome()     << std::endl;
   std::cout << "IsYBRFull()"      << e1->IsYBRFull()        << std::endl;
   std::cout << "IsPaletteColor()" << e1->IsPaletteColor()   << std::endl;
   // checks if LUT are found
   std::cout << "HasLUT()"         << e1->HasLUT()           << std::endl;

   std::cout << "GetNumberOfScalarComponents()"    
          << e1->GetNumberOfScalarComponents()<< std::endl;
   std::cout << "GetNumberOfScalarComponentsRaw()" 
          << e1->GetNumberOfScalarComponentsRaw()<< std::endl;
  

   std::cout << std::endl
             << "--- Get values on request ------------------------------------"
             << std::endl;
   // ------ User is aware, and wants to get fields with no accesor --------

   std::cout << "Manufacturer :["     << e1->GetEntryValue(0x0008,0x0070)
             << "]" << std::endl; 
   std::cout << "Institution :["      << e1->GetEntryValue(0x0008,0x0080)
             << "]" << std::endl;
   std::cout << "Patient's name :["   << e1->GetEntryValue(0x0010,0x0010)
             << "]" << std::endl;
   std::cout << "Physician's name :[" << e1->GetEntryValue(0x0008,0x0090)
             << "]" << std::endl; 
   std::cout << "Study Date :["       << e1->GetEntryValue(0x0008,0x0020)
             << "]" << std::endl; 
   std::cout << "Study inst UID :["   << e1->GetEntryValue(0x0020,0x000d)
             << "]" << std::endl;
   std::cout << "Serie inst UID :["   << e1->GetEntryValue(0x0020,0x000e)
             << "]" << std::endl;
   std::cout << "Frame ref UID :["   << e1->GetEntryValue(0x0020,0x0052)
             << "]" << std::endl;

   // ------ User wants to load the pixels---------------------------------
   
   // Hope now he knows enought about the image ;-)

   // First, create a gdcm::FileHelper
   gdcm::FileHelper *fh1 = new gdcm::FileHelper(e1);

   // Load the pixels, transforms LUT (if any) into RGB Pixels 
   uint8_t *imageData = fh1->GetImageData();
   // Get the image data size
   size_t dataSize    = fh1->GetImageDataSize();

   // Probabely, a straigh user won't load both ...

   // Load the pixels, DO NOT transform LUT (if any) into RGB Pixels 
   uint8_t *imageDataRaw = fh1->GetImageDataRaw();
   // Get the image data size
   size_t dataRawSize    = fh1->GetImageDataRawSize();

   // TODO : Newbee user would appreciate any comment !
 
   std::cout << "GetImageDataSize()"    
          << fh1->GetImageDataSize()    << std::endl;
   std::cout << "GetImageDataRawSize()" 
          << fh1->GetImageDataRawSize() << std::endl;
   // User Data
   std::cout << "GetRGBDataSize()"      
          << fh1->GetRGBDataSize()      << std::endl;
   std::cout << "GetRawDataSize()"      
          << fh1->GetRawDataSize()      << std::endl;
   std::cout << "GetUserDataSize()"     
          << fh1->GetUserDataSize()     << std::endl;
 

   std::cout << std::endl
             << "--- write a new image(1) -------------------------------------"
             << std::endl;
 
   // ------ User wants write a new image without shadow groups -------------

   gdcm::FileHelper *copy = new gdcm::FileHelper( output );
 
   d = e1->GetFirstEntry();
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
      d = e1->GetNextEntry();
   }

   std::cout << std::endl
             << "--- write a new image(2) -------------------------------------"
             << std::endl;
 
   // User knows the image is a 'color' one -RGB, YBR, Palette Color-
   // and wants to write it as RGB
   copy->SetImageData(imageData, dataSize);
   copy->SetWriteModeToRGB();
   copy->WriteDcmExplVR( output );

   // User wants to see if there is any difference before and after writting

   std::cout << "GetImageDataSize()"    
          << fh1->GetImageDataSize()    << std::endl;
   std::cout << "GetImageDataRawSize()" 
          << fh1->GetImageDataRawSize() << std::endl;
   // User Data
   std::cout << "GetRGBDataSize()"      
          << fh1->GetRGBDataSize()      << std::endl;
   std::cout << "GetRawDataSize()"      
          << fh1->GetRawDataSize()      << std::endl;
   std::cout << "GetUserDataSize()"     
          << fh1->GetUserDataSize()     << std::endl;
   // User wants to keep the Palette Color -if any- 
   // and write the image as it was
   copy->SetImageData(imageDataRaw, dataRawSize);
   copy->SetWriteModeToRGB();
   copy->WriteDcmExplVR( output );


   std::cout << std::endl
             << "------------------------------------------------------------"
             << std::endl;
   // User is in a fancy mood and wants to forge a bonm image
   // just to see how other Dicom viewers act


   // TODO : finish it 


   std::cout << std::endl
             << "------------------------------------------------------------"
             << std::endl;
   delete e1;
   delete fh1;
   delete copy;

   exit (0);
}

