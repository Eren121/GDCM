#include <iostream>
#include <stdio.h>
#include "gdcm.h"

int main(int argc, char* argv[])
{  
   std::string toto;
   char zozo[200];

   gdcmHeader* e1, *e2;
   gdcmFile  * f1, *f2;

   void* imageData, *imageData2;
   int dataSize, dataSize2;
     
   if( argc < 2 )
    {
    std::cerr << "Usage " << argv[0] << " image.dcm" << std::endl;
    return 1;
    }

   toto = argv[1];

// --------------------- we read the input image


   e1 = new gdcmHeader(toto, false, true);
   if (!e1->IsReadable()) {
       std::cerr << "Sorry, " << toto <<"  not a Readable DICOM / ACR File"
                 <<std::endl;
       return 0;
   }
   
   f1 = new gdcmFile(e1);
   imageData= f1->GetImageData();
   dataSize = f1->GetImageDataSize();

// --------------------- we write it as an Explicit VR DICOM file

      sprintf(zozo, "temp.XDCM" );
      std::cout << "WriteDCM Explicit VR" << std::endl;
      f1->WriteDcmExplVR(zozo);

// --------------------- we read the written image
      
   e2 = new gdcmHeader(zozo, false, true);
   if (!e2->IsReadable()) {
       std::cerr << "Sorry, " << zozo << " not a Readable DICOM / ACR File"  
                 <<std::endl;
       return 0;
   }
   f2 = new gdcmFile(e2);
   imageData2= f2->GetImageData();
   dataSize2 = f2->GetImageDataSize();

// --------------------- we compare the pixel areas

  if (dataSize != dataSize2) {
     std::cout << " ----------------------------------------- " 
          << "Bad shot! Lengthes are different : " 
          << dataSize << " # " << dataSize2
          << " for file : " << toto << std::endl;

     return 1;
  }
  if (int res=memcmp(imageData,imageData2,dataSize) !=0) {
     std::cout << " ----------------------------------------- " 
          << "Bad shot! Pixels are different : " 
          << " for file : " << toto << std::endl;
     std::cout << "memcmp(imageData,imageData2,dataSize) = " << res << std::endl;
     return 1;
  }
  
  //If we reach here everythin is fine, return 0 then:
  return 0;
}

