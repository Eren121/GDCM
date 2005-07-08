/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestCopyDicom.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/08 13:39:57 $
  Version:   $Revision: 1.41 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"

//Generated file:
#include "gdcmDataImages.h"

// return true if the file exists
bool FileExists(const char *filename);
bool RemoveFile(const char *source);

int CopyDicom(std::string const &filename, 
              std::string const &output )
{
      std::cout << "   Testing: " << filename << std::endl;
      if( FileExists( output.c_str() ) )
      {
         if( !RemoveFile( output.c_str() ) )
         {
            std::cout << "Ouch, the file exist, but I cannot remove it" << std::endl;
            return 1;
         }
      }

      //////////////// Step 1:
      std::cout << "      1...";
      
      gdcm::File *originalH = new gdcm::File( );
      gdcm::File *copyH     = new gdcm::File( );

      originalH->SetFileName( filename );
      originalH->Load( );
      //First of all copy the file, field by field

      //////////////// Step 2:
      std::cout << "2...";
      gdcm::DocEntry *d=originalH->GetFirstEntry();
      while(d)
      {
         if ( gdcm::BinEntry *b = dynamic_cast<gdcm::BinEntry*>(d) )
         {
            copyH->InsertBinEntry( b->GetBinArea(),b->GetLength(),
                                   b->GetGroup(),b->GetElement(),
                                   b->GetVR() );
         }
         else if ( gdcm::ValEntry *v = dynamic_cast<gdcm::ValEntry*>(d) )
         {   
             copyH->InsertValEntry( v->GetValue(),
                                    v->GetGroup(),v->GetElement(),
                                    v->GetVR() ); 
         }
         else
         {
          // We skip pb of SQ recursive exploration
         }

         d=originalH->GetNextEntry();
      }

      gdcm::FileHelper *original = new gdcm::FileHelper( originalH );
      gdcm::FileHelper *copy     = new gdcm::FileHelper( copyH );

      size_t dataSize = original->GetImageDataSize();
      uint8_t *imageData = original->GetImageData();

      // Useless to set the image data, because it's already made when
      // copying the corresponding BinEntry that contains the pixel data
      copy->SetImageData(imageData, dataSize);

      //////////////// Step 3:
      std::cout << "3...";
      copy->SetWriteModeToRGB();
      if( !copy->WriteDcmExplVR(output) )
      {
         std::cout << " Failed" << std::endl
                   << "       " << output << " not written" << std::endl;

         delete original;
         delete copy;
         delete originalH;
         delete copyH;

         return 1;
      }

      delete copy;
      delete copyH;

      //////////////// Step 4:
      std::cout << "4...";
      copy = new gdcm::FileHelper( output );

      //Is the file written still gdcm parsable ?
      if ( !copy->GetFile()->IsReadable() )
      { 
         std::cout << " Failed" << std::endl
                   << "        " << output << " not readable" << std::endl;

         delete original;
         delete originalH;

         return 1;
      }

      //////////////// Step 5:
      std::cout << "5...";
      size_t    dataSizeWritten = copy->GetImageDataSize();
      uint8_t *imageDataWritten = copy->GetImageData();

      if (dataSize != dataSizeWritten)
      {
         std::cout << " Failed" << std::endl
                   << "        Pixel areas lengths differ: "
                   << dataSize << " # " << dataSizeWritten << std::endl;

         delete original;
         delete copy;
         delete originalH;

         return 1;
      }

      if (int res = memcmp(imageData, imageDataWritten, dataSize) !=0)
      {
         (void)res;
         std::cout << " Failed" << std::endl
                   << "        Pixel differ (as expanded in memory)." << std::endl;

         delete original;
         delete copy;
         delete originalH;

         return 1;
      }
      std::cout << "OK." << std::endl ;

      delete original;
      delete copy;
      delete originalH;

      return 0;
}

// Here we load a gdcmFile and then try to create from scratch a copy of it,
// copying field by field the dicom image

int TestCopyDicom(int argc, char *argv[])
{
   if ( argc == 3 )
   {
      // The test is specified a specific filename, use it instead of looping
      // over all images
      const std::string input = argv[1];
      const std::string reference = argv[2];
      return CopyDicom( input, reference );
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
   std::cout << "   step 1: parse the image (as gdcmFile) and call"
             << " IsReadable(). After that, call GetImageData() and "
             << "GetImageDataSize() "
             << std::endl;
   std::cout << "   step 2: create a copy of the readed file and the new"
             << " pixel data are set to the copy"
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

      if( CopyDicom( filename, output ) != 0 )
      {
         retVal++;
      }

      i++;
   }
   return retVal;
}

