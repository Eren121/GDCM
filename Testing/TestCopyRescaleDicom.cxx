/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestCopyRescaleDicom.cxx,v $
  Language:  C++
  Date:      $Date: 2004/12/07 18:16:40 $
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
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"

//Generated file:
#include "gdcmDataImages.h"

bool FileExists(const char* filename);

bool RemoveFile(const char* source);

int CopyRescaleDicom(std::string const & filename, 
                     std::string const & output )
{
   std::cout << "   Testing: " << filename << std::endl;
   if( FileExists( output.c_str() ) )
   {
     // std::cerr << "Don't try to cheat, I am removing the file anyway" << std::endl;
      if( !RemoveFile( output.c_str() ) )
      {
         std::cout << "Ouch, the file exist, but I cannot remove it" << std::endl;
         return 1;
      }
   }

   //////////////// Step 1:
   std::cout << "      1...";
   gdcm::Header *originalH = new gdcm::Header( filename );
   gdcm::Header *copyH     = new gdcm::Header( );

   //First of all copy the header field by field

   // Warning :Accessor gdcmElementSet::GetEntry() should not exist 
   // It was commented out by Mathieu, that was a *good* idea
   // (the user does NOT have to know the way we implemented the Header !)
   // Waiting for a 'clean' solution, I keep the method ...JPRx


   //////////////// Step 2:
   std::cout << "2...";
   originalH->Initialize();
   gdcm::DocEntry* d = originalH->GetNextEntry();

   // Copy of the header content
   while(d)
   {
      if ( gdcm::BinEntry* b = dynamic_cast<gdcm::BinEntry*>(d) )
      {
         copyH->ReplaceOrCreateByNumber( 
                              b->GetBinArea(),
                              b->GetLength(),
                              b->GetGroup(), 
                              b->GetElement(),
                              b->GetVR() );
      }
      else if ( gdcm::ValEntry* v = dynamic_cast<gdcm::ValEntry*>(d) )
      {   
          copyH->ReplaceOrCreateByNumber( 
                              v->GetValue(),
                              v->GetGroup(), 
                              v->GetElement(),
                              v->GetVR() ); 
      }
      else
      {
       // We skip pb of SQ recursive exploration
      }

      d=originalH->GetNextEntry();
   }

   gdcm::File *original = new gdcm::File( originalH );
   gdcm::File *copy     = new gdcm::File( copyH );

   size_t dataSize = original->GetImageDataSize();
   uint8_t* imageData = original->GetImageData();

   size_t rescaleSize;
   uint8_t *rescaleImage;

   const std::string & bitsStored    = originalH->GetEntryByNumber(0x0028,0x0101);
   if( bitsStored == "16" )
   {
      std::cout << "Rescale...";
      copyH->ReplaceOrCreateByNumber( "8", 0x0028, 0x0100); // BitsAllocated
      copyH->ReplaceOrCreateByNumber( "8", 0x0028, 0x0101); // BitsStored
      copyH->ReplaceOrCreateByNumber( "7", 0x0028, 0x0102); // HighBit
      copyH->ReplaceOrCreateByNumber( "0", 0x0028, 0x0103); //Pixel Representation
 
      // We assume the value were from 0 to uint16_t max
      rescaleSize = dataSize / 2;
      rescaleImage = new uint8_t[dataSize];

      uint16_t* imageData16 = (uint16_t*)original->GetImageData();
      for(unsigned int i=0; i<rescaleSize; i++)
      {
         rescaleImage[i] = imageData16[i]/256;
      }
   }
   else
   {
      std::cout << "Same...";
      rescaleSize = dataSize;
      rescaleImage = new uint8_t[dataSize];
      memcpy(rescaleImage,original->GetImageData(),dataSize);
   }

   copy->SetImageData(rescaleImage, rescaleSize);

   //////////////// Step 3:
   std::cout << "3...";
   copy->SetWriteModeToRGB();
   if( !copy->WriteDcmExplVR(output) )
   {
      std::cout << " Failed" << std::endl
                << "        " << output << " not written" << std::endl;

      delete original;
      delete copy;
      delete originalH;
      delete copyH;
      delete[] rescaleImage;

      return 1;
   }

   delete copy;
   delete copyH;

   //////////////// Step 4:
   std::cout << "4...";
   copy = new gdcm::File( output );

   //Is the file written still gdcm parsable ?
   if ( !copy->GetHeader()->IsReadable() )
   { 
      std::cout << " Failed" << std::endl
                << "        " << output << " not readable" << std::endl;

      delete original;
      delete originalH;
      delete[] rescaleImage;

      return 1;
   }

   //////////////// Step 5:
   std::cout << "5...";
   size_t    dataSizeWritten = copy->GetImageDataSize();
   uint8_t* imageDataWritten = copy->GetImageData();

   if (originalH->GetXSize() != copy->GetHeader()->GetXSize() ||
       originalH->GetYSize() != copy->GetHeader()->GetYSize() ||
       originalH->GetZSize() != copy->GetHeader()->GetZSize())
   {
      std::cout << "Failed" << std::endl
         << "        X Size differs: "
         << "X: " << originalH->GetXSize() << " # " 
                  << copy->GetHeader()->GetXSize() << " | "
         << "Y: " << originalH->GetYSize() << " # " 
                  << copy->GetHeader()->GetYSize() << " | "
         << "Z: " << originalH->GetZSize() << " # " 
                  << copy->GetHeader()->GetZSize() << std::endl;
      delete original;
      delete copy;
      delete originalH;
      delete[] rescaleImage;

      return 1;
   }

   if (rescaleSize != dataSizeWritten)
   {
      std::cout << " Failed" << std::endl
                << "        Pixel areas lengths differ: "
                << dataSize << " # " << dataSizeWritten << std::endl;

      delete original;
      delete copy;
      delete originalH;
      delete[] rescaleImage;

      return 1;
   }

   if (int res = memcmp(rescaleImage, imageDataWritten, rescaleSize) !=0)
   {
      (void)res;
      std::cout << " Failed" << std::endl
                << "        Pixel differ (as expanded in memory)." << std::endl;

      delete original;
      delete copy;
      delete originalH;
      delete[] rescaleImage;

      return 1;
   }
   std::cout << "OK." << std::endl ;

   delete original;
   delete copy;
   delete originalH;
   delete[] rescaleImage;

   return 0;
}

// Here we load a gdcmFile and then try to create from scratch a copy of it,
// copying field by field the dicom image

int TestCopyRescaleDicom(int argc, char* argv[])
{
   if ( argc == 3 )
   {
      // The test is specified a specific filename, use it instead of looping
      // over all images
      const std::string input     = argv[1];
      const std::string reference = argv[2];
      return CopyRescaleDicom( input, reference );
   }
   else if ( argc > 3 || argc == 2 )
   {
      std::cout << "   Usage: " << argv[0]
                << " (no arguments needed)." << std::endl;
      std::cout << "or   Usage: " << argv[0]
                << " filename.dcm reference.dcm" << std::endl;
      return 1;
   }
   // else other cases:

   std::cout << "   Description (Test::TestCopyDicom): "
             << std::endl;
   std::cout << "   For all images in gdcmData (and not blacklisted in "
                "Test/CMakeLists.txt)"
             << std::endl;
   std::cout << "   apply the following to each filename.xxx: "
             << std::endl;
   std::cout << "   step 1: parse the image (as gdcmHeader) and call"
             << " IsReadable(). After that, call GetImageData() and "
             << "GetImageDataSize() "
             << std::endl;
   std::cout << "   step 2: create a copy of the readed file and the new"
             << " pixel datas are set to the copy"
             << std::endl;
   std::cout << "   step 3: write the copy of the image"
             << std::endl;
   std::cout << "   step 4: read the copy and call IsReadable()"
             << std::endl;
   std::cout << "   step 5: compare (in memory with memcmp) that the two "
             << "images " << std::endl
             << "           match (as expanded by gdcm)." << std::endl;
   std::cout << std::endl;

   int i =0;
   int retVal = 0;  //by default this is an error
   while( gdcmDataImages[i] != 0 )
   {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/";  //doh!
      filename += gdcmDataImages[i];

      std::string output = "output.dcm";

      if( CopyRescaleDicom( filename, output ) != 0 )
      {
         retVal++;
      }

      i++;
   }
   return retVal;
}

