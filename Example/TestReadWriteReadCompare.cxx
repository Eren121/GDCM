#include "gdcmHeader.h"
#include "gdcmFile.h"

//Generated file:
#include "gdcmDataImages.h"

int main(int argc, char* argv[]) 
{
   if (argc<2)
   {
      std::cerr << "Test::TestReadWriteReadCompare: Usage: " << argv[0]
                << " fileToCheck.dcm " << std::endl;
   }
   
   std::cout<< "Test::TestReadWriteReadCompare: description " << std::endl;
   std::cout << "   For all images in gdcmData (and not blacklisted in "
                "Test/CMakeLists.txt)" << std::endl;
   std::cout << "   apply the following multistep test: " << std::endl;
   std::cout << "   step 1: parse the image (as gdcmHeader) and call"
             << " IsReadable(). " << std::endl;
   std::cout << "   step 2: write the corresponding image in DICOM V3 "
             << "with explicit" << std::endl
             << "           Value Representation in temporary file "
             << "TestReadWriteReadCompare.dcm." << std::endl;
   std::cout << "   step 3: read the image written on step2 and call "
             << " IsReadable(). " << std::endl;
   std::cout << "   step 4: compare (in memory with memcmp) that the two "
             << "images " << std::endl
             << "           match (as expanded by gdcm)." << std::endl;

   int i = 0;
   //while( gdcmDataImages[i] != 0 ) 
   {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/";
      //filename += gdcmDataImages[i++];
     filename +=argv[1];
   
      std::cout << "   Testing: " << filename << std::endl;

      //////////////// Step 1 (see above description): 

      gdcmHeader *header = new gdcmHeader( filename );
      if( !header->IsReadable() )
      {
         std::cerr << "Test::TestReadWriteReadCompare: Image not gdcm compatible:"
                   << filename << std::endl;
         delete header;
         return 1;
      }
      std::cout << "           step 1 ...";

      //////////////// Step 2:

      gdcmFile*  file = new gdcmFile( header );
      int dataSize    = file->GetImageDataSize();
      void* imageData = file->GetImageData(); //EXTREMELY IMPORTANT
             // Sure, it is : It's up to the user to decide if he wants to
             // GetImageData or if he wants to GetImageDataRaw
             // (even if we do it by setting a flag, he will have to decide) 

      /// \todo Following line commented out because gdcmFile::SetImageData() is
      /// brain dead: it sets ImageDataSize to its argument and PixelRead to a.
      /// Later on, when writing gdcmFile::WriteBase() 
      /// and because PixelRead == 1 we call
      ///    PixelElement->SetLength( ImageDataSizeRaw );
      /// where we use ImageDataSizeRAW instead of ImageDataSize !
      /// But when the original image made the transformation LUT -> RGB, 
      /// ImageDataSizeRaw is the third of ImageDataSize, and there is no
      /// reason (since we called gdcmFile::SetImageData) to use the Raw image
      /// size... This "bug" in gdcmFile made that we had to black list
      /// images 8BitsUncompressedColor.dcm, OT-PAL-8-face.dcm and 
      /// US-PAL-8-10x-echo.dcm...
      /// In conclusion fix gdcmFile, and then uncomment the following line.
      
      // --> I did. ctest doesn't break. But ... is it enought to say it's OK ?
      
      file->SetImageData(imageData, dataSize);
      
      file->WriteDcmExplVR( "TestReadWriteReadCompare.dcm" );
      std::cout << "2...";
    
      //////////////// Step 3:

      gdcmFile* reread = new gdcmFile( "TestReadWriteReadCompare.dcm" );
      if( !reread->GetHeader()->IsReadable() )
      {
        std::cerr << "Test::TestReadWriteReadCompare: Could not reread image "
                  << "written:" << filename << std::endl;
        delete header;
        delete file;
        delete reread;
        return 1;
      }
      std::cout << "3...";
      // For the next step:
      int    dataSizeWritten = reread->GetImageDataSize();
      void* imageDataWritten = reread->GetImageData();

      //////////////// Step 4:
 
      if (dataSize != dataSizeWritten)
      {
         std::cout << std::endl
            << "        Pixel areas lengths differ: "
            << dataSize << " # " << dataSizeWritten << std::endl;
         delete (char*)imageData;
         delete (char*)imageDataWritten;
         delete header;
         delete file;
         delete reread;
         return 1;
      }

      if (int res = memcmp(imageData, imageDataWritten, dataSize) !=0)
      {
         (void)res;
         std::cout << std::endl
            << "        Pixel differ (as expanded in memory)." << std::endl;
         delete (char*)imageData;
         delete (char*)imageDataWritten;
         delete header;
         delete file;
         delete reread;
         return 1;
      }
      std::cout << "4...OK." << std::endl ;

      //////////////// Clean up:
      delete (char*)imageData;
      delete (char*)imageDataWritten;
      delete header;
      delete file;
      delete reread;
   }

  return 0;
}
