/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestAllReadCompareDicom.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/11 15:58:36 $
  Version:   $Revision: 1.23 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmHeader.h"
#include "gdcmFile.h"

#include <iostream>
#include <fstream>

//Generated file:
#include "gdcmDataImages.h"

int InternalTest(std::string const & filename, 
                 std::string const & referenceFileName )
{
      std::cout << "   Testing: " << filename << std::endl;

      ////// Step 1:
      std::cout << "      1...";
      gdcm::File* tested = new gdcm::File( filename );
      if( !tested->GetHeader()->IsReadable() )
      {
        std::cout << " Failed" << std::endl
                   << "      Image not gdcm compatible:"
                  << filename << std::endl;
        delete tested;
        return 1;
      }

      ////// Step 2:
      ////// Check for existence of reference baseline dicom file:
      std::cout << "2...";

      //FILE* testFILE = fopen( referenceFileName.c_str(), "r" );
      std::ifstream testFILE( referenceFileName.c_str() );
      if (! testFILE )
      {
         uint8_t* testedImageData = tested->GetImageData(); // Kludge
         (void)testedImageData;

         tested->SetWriteModeToRGB();
         tested->WriteDcmExplVR( referenceFileName );
      }
      else
      {
         //fclose( testFILE );
      }
      testFILE.close();

      ////// Step 3a:
      ////// When reference file is not gdcm readable test is failed:
      std::cout << "3a...";

      gdcm::File* reference = new gdcm::File( referenceFileName );
      if( !reference->GetHeader()->IsReadable() )
      {
         std::cout << " Failed" << std::endl
                   << "              reference image " 
                   << referenceFileName 
                   << " is not gdcm compatible." << std::endl;
         delete tested;
         delete reference;
         return 1;
      }

      std::string PixelType = reference->GetHeader()->GetPixelType();

      ////// Step 3b:
      std::cout << "3b...";
      int testedDataSize    = tested->GetImageDataSize();
      uint8_t* testedImageData = tested->GetImageData();
    
      int    referenceDataSize = reference->GetImageDataSize();
      uint8_t* referenceImageData = reference->GetImageData();

      // Test the image size
      if (tested->GetHeader()->GetXSize() != reference->GetHeader()->GetXSize() ||
          tested->GetHeader()->GetYSize() != reference->GetHeader()->GetYSize() ||
          tested->GetHeader()->GetZSize() != reference->GetHeader()->GetZSize())
      {
         std::cout << "Failed" << std::endl
            << "        Size differs: "
            << "X: " << tested->GetHeader()->GetXSize() << " # " 
                     << reference->GetHeader()->GetXSize() << " | "
            << "Y: " << tested->GetHeader()->GetYSize() << " # " 
                     << reference->GetHeader()->GetYSize() << " | "
            << "Z: " << tested->GetHeader()->GetZSize() << " # " 
                     << reference->GetHeader()->GetZSize() << std::endl;
         delete reference;
         delete tested;
         return 1;
      }

      // Test the data size
      if (testedDataSize != referenceDataSize)
      {
         std::cout << " Failed" << std::endl
                   << "        pixel ("
                   << PixelType
                   <<") areas lengths differ: "
                   << testedDataSize << " # " << referenceDataSize
                   << std::endl;
         delete tested;
         delete reference;
         return 1;
      }

      // Test the data content
      if (int res = memcmp(testedImageData, referenceImageData,
                           testedDataSize) != 0 )
      {
         (void)res;
         std::cout << " Failed" << std::endl
                   << "        pixel (" 
                   << PixelType
                   << ") differ (as expanded in memory)."
                   << std::endl;
         delete tested;
         delete reference;
         return 1;
      }

      //////////////// Clean up:
      delete tested;
      delete reference;

      std::cout << "OK." << std::endl;
      
      return 0;
}

int TestAllReadCompareDicom(int argc, char* argv[]) 
{
   if ( argc == 3 )
   {
      // The test is specified a specific filename, use it instead of looping
      // over all images
      const std::string input = argv[1];
      const std::string reference = argv[2];
      return InternalTest( input, reference );
   }
   else if ( argc > 3 || argc == 2 )
   {
      std::cerr << "   Usage: " << argv[0]
                << " (no arguments needed)." << std::endl;
      std::cerr << "or   Usage: " << argv[0]
                << " filename.dcm reference.dcm" << std::endl;
      return 1;
   }
   // else other cases:
   
   std::cout << "   Description (Test::TestAllReadCompareDicom): "
             << std::endl;
   std::cout << "   For all images in gdcmData (and not blacklisted in "
                "Test/CMakeLists.txt)"
             << std::endl;
   std::cout << "   apply the following to each filename.xxx: "
             << std::endl;
   std::cout << "   step 1: parse the image (as gdcmHeader) and call"
             << " IsReadable(). "
             << std::endl;
   std::cout << "   step 2: find in GDCM_DATA_ROOT/BaselineDicom/filename.dcm"
             << std::endl
             << "           (with format DICOM V3, explicit Value"
             << "Representation)"
             << std::endl;
   std::cout << "   step 3a: when image NOT found on step 2, write "
             << std::endl
             << "            GDCM_DATA_ROOT/BaselineDicom/filename.dcm"
             << std::endl
             << "           (with format DICOM V3, explicit Value"
             << "Representation)"
             << std::endl;
   std::cout << "   step 3b: when image found on step 2, and when IsReadable()"
             << std::endl
             << "            compare it (in memory with memcmp) with the"
             << std::endl
             << "            image we are testing (the one of step 1). "
             << std::endl << std::endl;

   int i = 0;
   int result = 0;
   while( gdcmDataImages[i] != 0 )
   {
      ////// Check for existence of reference baseline directory

      std::string baseLineDir = GDCM_DATA_ROOT;
      baseLineDir += "/BaselineDicom/";

      std::ifstream* testDIR = new std::ifstream(baseLineDir.c_str(), std::ios::in | std::ios::binary);
      if (!testDIR )
      {
         std::cerr << "   The reference baseline directory " << std::endl
                   << "      "
                   << baseLineDir << std::endl
                   << "   couldn't be opened."
                   << std::endl;
         return 1;
      }
      else
      {
         testDIR->close();
      }

      ////// Step 1 (see above description):
      std::string filename = GDCM_DATA_ROOT;
      filename += "/";
      filename += gdcmDataImages[i];
      
      std::string referenceFileName = baseLineDir + gdcmDataImages[i++];
      std::string::size_type slash_pos = referenceFileName.rfind( "." );
      if ( slash_pos != std::string::npos )
      {
         referenceFileName.replace( slash_pos + 1, 3, "dcm" );
      }

      if( InternalTest( filename, referenceFileName ) != 0 )
      {
         result++;
      }
   }

   return result;
}
