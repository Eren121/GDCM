/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestReadWriteReadCompare.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/02 10:05:26 $
  Version:   $Revision: 1.21 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"

//Generated file:
#include "gdcmDataImages.h"

int CompareInternal(std::string const & filename, std::string const & output)
{
   std::cout << "   Testing: " << filename << std::endl;

   //////////////// Step 1 (see above description):

   gdcm::File *file = new gdcm::File( filename );
   if( !file->IsReadable() )
   {
      std::cerr << "Test::TestReadWriteReadCompare: Image not gdcm compatible:"
                << filename << std::endl;
      delete file;
      return 1;
   }
   std::cout << "           step 1...";

   //////////////// Step 2:

   gdcm::FileHelper *filehelper = new gdcm::FileHelper( file );
   int dataSize    = filehelper->GetImageDataSize();
   uint8_t *imageData = filehelper->GetImageData(); //EXTREMELY IMPORTANT
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
   
   filehelper->SetImageData(imageData, dataSize);
   
   filehelper->SetWriteModeToRGB();
   filehelper->WriteDcmExplVR( output );
   std::cout << "2...";
 
   //////////////// Step 3:

   gdcm::FileHelper *reread = new gdcm::FileHelper( output );
   if( !reread->GetFile()->IsReadable() )
   {
     std::cerr << "Failed" << std::endl
               << "Test::TestReadWriteReadCompare: Could not reread image "
               << "written:" << filename << std::endl;
     delete file;
     delete filehelper;
     delete reread;
     return 1;
   }
   std::cout << "3...";
   // For the next step:
   int    dataSizeWritten = reread->GetImageDataSize();
   uint8_t *imageDataWritten = reread->GetImageData();

   //////////////// Step 4:
   // Test the image size
   if (file->GetXSize() != reread->GetFile()->GetXSize() ||
       file->GetYSize() != reread->GetFile()->GetYSize() ||
       file->GetZSize() != reread->GetFile()->GetZSize())
   {
      std::cout << "Failed" << std::endl
         << "        X Size differs: "
         << "X: " << file->GetXSize() << " # " 
                  << reread->GetFile()->GetXSize() << " | "
         << "Y: " << file->GetYSize() << " # " 
                  << reread->GetFile()->GetYSize() << " | "
         << "Z: " << file->GetZSize() << " # " 
                  << reread->GetFile()->GetZSize() << std::endl;
      delete file;
      delete filehelper;
      delete reread;
      return 1;
   }

   // Test the data size
   if (dataSize != dataSizeWritten)
   {
      std::cout << "Failed" << std::endl
         << "        Pixel areas lengths differ: "
         << dataSize << " # " << dataSizeWritten << std::endl;
      delete file;
      delete filehelper;
      delete reread;
      return 1;
   }

   // Test the data's content
   if (memcmp(imageData, imageDataWritten, dataSize) !=0)
   {
      std::cout << "Failed" << std::endl
         << "        Pixel differ (as expanded in memory)." << std::endl;
      delete file;
      delete filehelper;
      delete reread;
      return 1;
   }
   std::cout << "4...OK." << std::endl ;

   //////////////// Clean up:
   delete file;
   delete filehelper;
   delete reread;

   return 0;
}

int TestReadWriteReadCompare(int argc, char *argv[]) 
{
   int result = 0;
   if (argc == 3)
   {
      const std::string input = argv[1];
      const std::string output = argv[2];
      result += CompareInternal(input, output);
   }
   else if( argc > 3 || argc == 2 )
   {
      std::cerr << "Please read the manual" << std::endl;
   }
   else
   {
      std::cout<< "Test::TestReadWriteReadCompare: description " << std::endl;
      std::cout << "   For all images in gdcmData (and not blacklisted in "
                   "Test/CMakeLists.txt)" << std::endl;
      std::cout << "   apply the following multistep test: " << std::endl;
      std::cout << "   step 1: parse the image (as gdcmFile) and call"
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
      while( gdcmDataImages[i] != 0 )
      {
         std::string filename = GDCM_DATA_ROOT;
         filename += "/";
         filename += gdcmDataImages[i++];
         result += CompareInternal(filename, "TestReadWriteReadCompare.dcm"); 
      }
   }
   return result;
}
