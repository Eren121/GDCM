/*=========================================================================

  Program:   gdcm
  Module:    $RCSfile: TestReadWriteJPEGReadCompare.cxx,v $
  Language:  C++
  Date:      $Date: 2007/08/28 16:57:00 $
  Version:   $Revision: 1.2 $

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmDebug.h"

#include "gdcmGlobal.h"
#include "gdcmTS.h"

//Generated file:
#include "gdcmDataImages.h"


const unsigned int MAX_NUMBER_OF_DIFFERENCE = 10;


static int CompareInternalJPEG(std::string const &filename, std::string const &output)
{
   std::cout << "----------------------------------------------------------------------" << std::endl
             << "   Testing: " << filename << std::endl;

   //////////////// Step 1 (see above description):

   GDCM_NAME_SPACE::File *file = GDCM_NAME_SPACE::File::New( );
   file->SetFileName( filename );
   file->Load ();
   if( !file->IsReadable() )
   {
      std::cout << "Failed" << std::endl
                << "Test::TestReadWriteJPEGReadCompare: Image not gdcm compatible:"
                << filename << std::endl;
      file->Delete();
      return 1;
   }
   std::cout << "           step 1...";

   //////////////// Step 2:
   GDCM_NAME_SPACE::FileHelper *filehelper = GDCM_NAME_SPACE::FileHelper::New( file );
   int dataSize       = filehelper->GetImageDataSize();
   uint8_t *imageData = filehelper->GetImageData(); //EXTREMELY IMPORTANT
          // Sure, it is : It's up to the user to decide if he wants to
          // GetImageData or if he wants to GetImageDataRaw
          // (even if we do it by setting a flag, *he* will have to decide)

   //filehelper->SetImageData(imageData, dataSize);

         // Just to be sure the further Write() doesn't corrupt imageData ...
         std::cout << std::endl;    
         int i,j;
         for(i=0, j=0;i<dataSize && j<MAX_NUMBER_OF_DIFFERENCE;i++)
         {
               std::cout << std::hex << "(" << i << " : " 
                         << std::hex << (int)(imageData[i]) << ") "
                         << std::dec;
               ++j;
         }
         std::cout << std::endl;

   filehelper->SetWriteModeToRGB();
  
   filehelper->SetWriteTypeToJPEG(  ); 
   filehelper->SetUserData(imageData,dataSize); // This one ensures the compression
   filehelper->Write( output ); 

         // Just to be sure the previous Write() didn't corrupt imageData ..   
         for(i=0, j=0;i<dataSize && j<MAX_NUMBER_OF_DIFFERENCE;i++)
         {
               std::cout << std::hex << "(" << i << " : " 
                         << std::hex << (int)(imageData[i]) << ") "
                         << std::dec;
               ++j;
         }
         std::cout << std::endl << std::endl;
 
   std::cout << "2...";

   //////////////// Step 3:
   GDCM_NAME_SPACE::File *fileout = GDCM_NAME_SPACE::File::New();
   fileout->SetFileName( output );
   fileout->Load();

   if( !fileout->IsReadable() )
   {
      std::cout << "Failed" << std::endl
                << "Test::TestReadWriteReadCompare: Could not parse the newly "
                << "written image:" << filename << std::endl;
      file->Delete();
      filehelper->Delete();
      fileout->Delete();
      return 1;
   }

   GDCM_NAME_SPACE::FileHelper *reread = GDCM_NAME_SPACE::FileHelper::New( fileout );

   std::cout << "3...";
   // For the next step:
   int     dataSizeWritten   = reread->GetImageDataSize();
   uint8_t *imageDataWritten = reread->GetImageData();
   
         // Just to see
         std::cout << std::endl;
         for(i=0, j=0;i<dataSize && j<MAX_NUMBER_OF_DIFFERENCE;i++)
         {
               std::cout << std::hex << "(" << i << " : " 
                         << std::hex << (int)(imageDataWritten[i]) << ") "
                         << std::dec;
               ++j;
         }
         std::cout << std::endl << std::endl;
 
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
      file->Delete();
      filehelper->Delete();
      fileout->Delete();
      reread->Delete();
      return 1;
   }

   // Test the data size
   // beware of odd length Pixel Element!
   int dataSizeFixed = dataSize + dataSize%2;
   int dataSizeWrittenFixed = dataSizeWritten + dataSizeWritten%2;

   if (dataSizeFixed != dataSizeWrittenFixed)
   {
      std::cout << "Failed" << std::endl
         << "        Pixel areas lengths differ: "
         << dataSize << " # " << dataSizeWritten << std::endl;
      file->Delete();
      filehelper->Delete();
      fileout->Delete();
      reread->Delete();
      return 1;
   }

   // Test the data content
   
   if (memcmp(imageData, imageDataWritten, dataSize) !=0)
   {
         std::string PixelType = filehelper->GetFile()->GetPixelType();
         std::string ts        = filehelper->GetFile()->GetTransferSyntax();

         std::cout << " Failed" << std::endl
                   << "        pixel (" 
                   << PixelType
                   << ") differ (as expanded in memory)."
                   << std::endl
                   << "        compression : " 
                   << GDCM_NAME_SPACE::Global::GetTS()->GetValue(ts) << std::endl;

         std::cout << "        list of the first " << MAX_NUMBER_OF_DIFFERENCE
                   << " pixels differing (pos : original - written) :" 
                   << std::endl;
  
         int i;
         unsigned int j;
         for(i=0, j=0;i<dataSize && j<MAX_NUMBER_OF_DIFFERENCE;i++)
         {
            //if(imageData[i]!=imageDataWritten[i])
            if (abs ((int)imageData[i]-(int)imageDataWritten[i]) > 2)
              {
               std::cout << std::hex << "(" << i << " : " 
                         << std::hex << (int)(imageData[i]) << " - "
                         << std::hex << (int)(imageDataWritten[i]) << ") "
                         << std::dec;
               ++j;
              }
         }
         std::cout << std::endl;
   
     if (j !=0 ) { 
      file->Delete();
      filehelper->Delete();
      fileout->Delete();
      reread->Delete();
      return 1;
     }
   }
   std::cout << "========================================= 4...OK." << std::endl ;

   //////////////// Clean up:
   file->Delete();
   filehelper->Delete();
   fileout->Delete();
   reread->Delete();

   return 0;
}

// -------------------------------------------------------------------------------------------

int TestReadWriteJPEGReadCompare(int argc, char *argv[]) 
{
   int result = 0;

   if (argc == 4)
      GDCM_NAME_SPACE::Debug::DebugOn();

   if (argc >= 3)
   {
      const std::string input  = argv[1];
      const std::string output = argv[2];
      result += CompareInternalJPEG(input, output);
   }
   else if( argc > 4 || argc == 2 )
   {
      std::cout << "Please read the manual" << std::endl;
   }
   else
   {
      std::cout<< "Test::TestReadWriteJPEGReadCompare: description " << std::endl;
      std::cout << "   For all images in gdcmData (and not blacklisted in "
                   "Test/CMakeLists.txt)" << std::endl;
      std::cout << "   apply the following multistep test: " << std::endl;
      std::cout << "   step 1: parse the image (as gdcmFile) and call"
                << " IsReadable(). " << std::endl;
      std::cout << "   step 2: write the corresponding image in JPEG DICOM V3 "
                << "with explicit Value Representation " << std::endl
                << "            in temporary file "
                << "TestReadWriteJPEGReadCompare.dcm." << std::endl;
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
         result += CompareInternalJPEG(filename, "TestReadWriteJPEGReadCompare.dcm"); 
      }
   }
   return result;
}
