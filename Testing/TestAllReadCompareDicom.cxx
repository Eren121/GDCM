#include "gdcmHeader.h"
#include "gdcmFile.h"

//Generated file:
#include "gdcmDataImages.h"

int TestAllReadCompareDicom(int argc, char* argv[]) 
{
   if ( argc > 1 )
   {
      std::cerr << "   Usage: " << argv[0]
                << " (no arguments needed)." << std::endl;
      return 1;
   }
   
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
   while( gdcmDataImages[i] != 0 )
   {
      ////// Check for existence of reference baseline directory

      std::string baseLineDir = GDCM_DATA_ROOT;
      baseLineDir += "/BaselineDicom/";

      FILE* testFILE = fopen( baseLineDir.c_str(), "r" );
      if (! testFILE )
      {
        std::cerr << "   The reference baseline directory " << std::endl
                  << "      "
                  << baseLineDir << std::endl
                  << "   couldn't be opened."
                  << std::endl;
        return 1;
      }
      else
        fclose( testFILE );

      ////// Step 1 (see above description):

      std::string filename = GDCM_DATA_ROOT;
      filename += "/";  //doh!
      filename += gdcmDataImages[i];
   
      std::cout << "   Testing: " << filename << std::endl;

      gdcmFile* tested = new gdcmFile( filename.c_str(), false, true );
      if( !tested->GetHeader()->IsReadable() )
      {
        std::cout << "      Image not gdcm compatible:"
                  << filename << std::endl;
        delete tested;
        return 1;
      }

      ////// Step 2:

      ////// Check for existence of reference baseline dicom file:

      std::string referenceFileName = baseLineDir + gdcmDataImages[i++];
      std::string::size_type slash_pos = referenceFileName.rfind( "." );
      if ( slash_pos != std::string::npos )
      {
         referenceFileName.replace( slash_pos + 1, 3, "dcm" );
      }

      testFILE = fopen( referenceFileName.c_str(), "r" );
      if (! testFILE )
      {
      ////// Step 3a:

         int testedDataSize    = tested->GetImageDataSize();
         void* testedImageData = tested->GetImageData(); // Kludge
         tested->WriteDcmExplVR( referenceFileName );
         std::cerr << "      Creating reference baseline file :" << std::endl
                   << "      " << referenceFileName 
                   << std::endl;
         delete tested;
         delete (char*)testedImageData;
         continue; 
      }
      else
         fclose( testFILE );

      ////// When reference file is not gdcm readable test is failed:
  
      gdcmFile* reference = new gdcmFile( referenceFileName.c_str(),
                                          false, true );
      if( !reference->GetHeader()->IsReadable() )
      {
         std::cout << "      Reference image " << std::endl
                   << "      " << referenceFileName <<std::endl
                   << "      is not gdcm compatible." << std::endl;
         delete tested;
         delete reference;
         return 1;
      }

      ////// Step 3b:

      int testedDataSize    = tested->GetImageDataSize();
      void* testedImageData = tested->GetImageData();
    
      int    referenceDataSize = reference->GetImageDataSize();
      void* referenceImageData = reference->GetImageData();

      if (testedDataSize != referenceDataSize)
      {
         std::cout << "        Pixel areas lengths differ: "
                   << testedDataSize << " # " << referenceDataSize
                   << std::endl;
         delete tested;
         delete reference;
         delete (char*)testedImageData;
         delete (char*)referenceImageData;
         return 1;
      }

      if (int res = memcmp(testedImageData, referenceImageData,
                           testedDataSize) != 0 )
      {
         std::cout << "        Pixel differ (as expanded in memory)."
                   << std::endl;
         delete tested;
         delete reference;
         delete (char*)testedImageData;
         delete (char*)referenceImageData;
         return 1;
      }
      std::cout << "      Passed." << std::endl ;

      //////////////// Clean up:
      delete tested;
      delete reference;
      delete (char*)testedImageData;
      delete (char*)referenceImageData;
   }

   return 0;
}
