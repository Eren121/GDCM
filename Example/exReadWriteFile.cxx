/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exReadWriteFile.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/14 15:16:57 $
  Version:   $Revision: 1.4 $
                                                                                
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

 
int main(int argc, char *argv[])
{


std::cout << " --- WARNING --- WARNING --- WARNING --- WARNING ---" <<std::endl;
std::cout << " "                                                    <<std::endl; 
std::cout << " This source program is NOT intendend to be run as is"<<std::endl;
std::cout << " "                                                    <<std::endl;
std::cout << " It just shows a set of possible uses."               <<std::endl;
std::cout << "User MUST read it, "                                  <<std::endl;
std::cout << "          comment out the useless parts "             <<std::endl;
std::cout << "          invoke it with an ad hoc image(*) "         <<std::endl;
std::cout << "           check the resulting image   "              <<std::endl; 
std::cout << "  "                                                   <<std::endl;
std::cout << " (*) For samples, user can refer to gdcmData"         <<std::endl;
std::cout << "         and read README.txt file "                   <<std::endl;
std::cout << " "                                                    <<std::endl;
std::cout << "This source program will be splitted into smaller elementary" 
          <<  " programs"                                           <<std::endl;
std::cout << " "                                                    <<std::endl;
std::cout << " --- WARNING --- WARNING --- WARNING --- WARNING ---" <<std::endl;

   if (argc < 3)
   {
      std::cerr << "Usage :" << std::endl << 
      argv[0] << " input_dicom output_dicom" << std::endl;
      return 1;
   }

   std::string filename = argv[1];
   std::string output   = argv[2];

   // First, let's create a gdcm::File
   // that will contain all the Dicom fields but the Pixels Element

   gdcm::File *f1= new gdcm::File( filename );


   // Ask content to be printed
   std::cout << std::endl
             << "--- Standard Print -------------------------------------------"
             << std::endl;
   f1->SetPrintLevel(2);   // to have a nice output
   //f1->SetPrintLevel(1); 
   f1->Print();            // user may comment out if too much verbose


  // User asks for field by field Printing
  
   std::cout << std::endl
             << "--- Display only human readable values -----------------------"
             << std::endl;

   gdcm::ValEntry *valEntry;
   std::string value;
   std::string vr;   // value representation
   std::string vm;   // value multiplicity
   std::string name; // held in the Dicom Dictionary


   gdcm::DocEntry *d = f1->GetFirstEntry();
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
         vr     = valEntry->GetVR();
         // user wants really to know everything about entry!
         vm     = valEntry->GetVM();
         name   = valEntry->GetName();

         std::cout //<< std::hex << group << "," << elem 
          << valEntry->GetKey()
               << "     VR :[" << vr    << "] VM : [" << vm 
             << "] name : [" << name  << "]"
             << " value : [" << value << "]" 
 
              << std::endl;
      }
      d = f1->GetNextEntry();
   }

   std::cout << std::endl
             << "--- Use pre-defined acessors ---------------------------------"
             << std::endl;
 
   // ------ some pre-defined acessors may supply usefull informations ----

   // about Image
   int linesNumber   = f1->GetYSize();
   int rawsNumber    = f1->GetXSize();
   int framesNumber  = f1->GetYSize();// defaulted to 1 if not found

   std::cout << "lines : "   << linesNumber  << " columns : " << rawsNumber
             << " frames : " << framesNumber << std::endl;
 
   // about Pixels
   int pixelSize         = f1->GetPixelSize(); 
   std::string pixelType = f1->GetPixelType();
   bool isSigned         = f1->IsSignedPixelData();
  
   std::cout << "pixelSize : "   << pixelSize  << " pixelType : " << pixelType
             << " signed : "     << isSigned   << std::endl;
 
   // about pixels, too.
   // Better you forget these ones
  
   std::cout << "GetBitsStored()"      << f1->GetBitsStored()      << std::endl;
   std::cout << "GetBitsAllocated()"   << f1->GetBitsAllocated()   << std::endl;
   std::cout << "GetHighBitPosition()" << f1->GetHighBitPosition() << std::endl;

   std::cout << "GetSamplesPerPixel()"     
          << f1->GetSamplesPerPixel()     << std::endl;
   std::cout << "GetPlanarConfiguration()" 
          << f1->GetPlanarConfiguration() << std::endl; 
 
   // about 'image geography'
 
   float xs = f1->GetXSpacing();
   float ys = f1->GetYSpacing();
   float zs = f1->GetZSpacing();  // defaulted to 1.0 if not found

   float xo = f1->GetXOrigin();
   float yo = f1->GetYOrigin();
   float zo = f1->GetZOrigin();

   std::cout << "GetXSpacing()"     << xs      << std::endl;
   std::cout << "GetYSpacing()"     << ys      << std::endl;
   std::cout << "GetXSpacing()"     << zs      << std::endl;

   std::cout << "GetXOrigin()"      << xo      << std::endl;
   std::cout << "GetYOrigin()"      << yo      << std::endl;
   std::cout << "GetZOrigin()"      << zo      << std::endl;

   // about its way to store colors (if user is aware)

   // checks Photometric Interpretation
   std::cout << "IsMonochrome()"   << f1->IsMonochrome()     << std::endl;
   std::cout << "IsYBRFull()"      << f1->IsYBRFull()        << std::endl;
   std::cout << "IsPaletteColor()" << f1->IsPaletteColor()   << std::endl;
   // checks if LUT are found
   std::cout << "HasLUT()"         << f1->HasLUT()           << std::endl;

   std::cout << "GetNumberOfScalarComponents()"    
          << f1->GetNumberOfScalarComponents()<< std::endl;
   std::cout << "GetNumberOfScalarComponentsRaw()" 
          << f1->GetNumberOfScalarComponentsRaw()<< std::endl;
  

   std::cout << std::endl
             << "--- Get values on request ------------------------------------"
             << std::endl;
   // ------ User is aware, and wants to get fields with no accesor --------

   std::cout << "Manufacturer :["     << f1->GetEntryValue(0x0008,0x0070)
             << "]" << std::endl; 
   std::cout << "Institution :["      << f1->GetEntryValue(0x0008,0x0080)
             << "]" << std::endl;
   std::cout << "Patient's name :["   << f1->GetEntryValue(0x0010,0x0010)
             << "]" << std::endl;
   std::cout << "Physician's name :[" << f1->GetEntryValue(0x0008,0x0090)
             << "]" << std::endl; 
   std::cout << "Study Date :["       << f1->GetEntryValue(0x0008,0x0020)
             << "]" << std::endl; 
   std::cout << "Study inst UID :["   << f1->GetEntryValue(0x0020,0x000d)
             << "]" << std::endl;
   std::cout << "Serie inst UID :["   << f1->GetEntryValue(0x0020,0x000e)
             << "]" << std::endl;
   std::cout << "Frame ref UID :["   << f1->GetEntryValue(0x0020,0x0052)
             << "]" << std::endl;
 
   // User wants to get info about the 'real world' vs image


   // ------ User wants to load the pixels---------------------------------
   
   // Hope now he knows enought about the image ;-)

   // First, create a gdcm::FileHelper
   gdcm::FileHelper *fh1 = new gdcm::FileHelper(f1);

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
 
   d = f1->GetFirstEntry();
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
   copy->SetWriteModeToRaw();
   copy->WriteDcmExplVR( output );


   std::cout << std::endl
             << "------------------------------------------------------------"
             << std::endl;
   // User is in a fancy mood and wants to forge a bomb image
   // just to see how other Dicom viewers act


   // TODO : finish it 


   std::cout << std::endl
             << "------------------------------------------------------------"
             << std::endl;
   delete f1;
   delete fh1;
   delete copy;

   exit (0);
}

