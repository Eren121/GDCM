#include <iostream>
#include <stdio.h>
#include "gdcm.h"

int main(int argc, char* argv[])
{  
   std::string toto;
   char zozo[200];


   gdcmHeader* e1;
   gdcmFile  * f1;

   //gdcmDocument * d;  //not used
   void* imageData;
   int dataSize;

   if (argc < 3) {
         std::cerr << "usage: " << std::endl 
                   << argv[0] << " OriginalFileName writtingMode "
                << std::endl 
                   << "(a : ACR, produces a file named OriginalFileName.ACR"
                   << " x : DICOM Explicit VR, produces a file named OriginalFileName.XDCM"
                   << " r : RAW, produces a file named OriginalFileName.RAW"
                   << " v : explicit VR + computes the video inv image --> OriginalFileName.VDCM"
                << std::endl;

         return 0;
   }
/*
   if (0) {  // Just to keep the code for further use
      std::cout <<std::endl << "-------- Test gdcmHeader ------" <<std::endl;
      e1 = new gdcmHeaderHelper(argv[1]);
      if (!f1->GetHeader()->IsReadable()) {
         std::cout << "Sorry, not a DICOM / ACR File"  <<std::endl;
         exit(0);
      }
      std::cout << std::endl << "----------------------> after new gdcmHeader"
                << std::endl;
      e1->PrintEntry();
      std::cout <<std::endl <<"---------------------------------------" 
                <<std::endl;
   }
*/

   std::cout << std::endl
             << "--------------------- file :" << argv[1] 
             << std::endl;
     
   toto = argv[1]; 

   e1 = new gdcmHeader( toto.c_str() );
   if (!e1->IsReadable())
   {
       std::cerr << "Sorry, not a Readable DICOM / ACR File"  <<std::endl;
       return 0;
   }
  // e1->Print(); 
   
   f1 = new gdcmFile(e1);
// ---     

   dataSize = f1->GetImageDataSize();
   std::cout <<std::endl <<" dataSize " << dataSize << std::endl;
   int nX,nY,nZ,sPP,planarConfig;
   std::string pixelType, transferSyntaxName;
   nX=e1->GetXSize();
   nY=e1->GetYSize();
   nZ=e1->GetZSize();
   std::cout << " DIMX=" << nX << " DIMY=" << nY << " DIMZ=" << nZ << std::endl;

   pixelType    = e1->GetPixelType();
   sPP          = e1->GetSamplesPerPixel();
   planarConfig = e1->GetPlanarConfiguration();
   
   std::cout << " pixelType="           << pixelType 
             << " SampleserPixel="      << sPP
             << " PlanarConfiguration=" << planarConfig 
             << " PhotometricInterpretation=" 
                                << e1->GetEntryByNumber(0x0028,0x0004) 
             << std::endl;

   int numberOfScalarComponents=e1->GetNumberOfScalarComponents();
   std::cout << "NumberOfScalarComponents " << numberOfScalarComponents <<std::endl;
   transferSyntaxName = e1->GetTransfertSyntaxName();
   std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" << std::endl;
   
   if (  transferSyntaxName != "Implicit VR - Little Endian"
      && transferSyntaxName != "Explicit VR - Little Endian"     
      && transferSyntaxName != "Deflated Explicit VR - Little Endian"      
      && transferSyntaxName != "Explicit VR - Big Endian"
      && transferSyntaxName != "Uncompressed ACR-NEMA"     ) {
      std::cout << std::endl << "==========================================="
                << std::endl; 
      f1->ParsePixelData();
      std::cout << std::endl << "==========================================="
                << std::endl; 
   }
   imageData= f1->GetImageData();

   switch (argv[2][0]) {
   case 'a' :
            // ecriture d'un fichier ACR 
            // à partir d'un dcmHeader correct.

      sprintf(zozo, "%s.ACR", toto.c_str());
      printf ("WriteACR\n");
      f1->WriteAcr(zozo);
      break;

   case 'd' :  // Not document in the 'usage', because the method is knowed to be bugged. 

           // ecriture d'un fichier DICOM Implicit VR 
           // à partir d'un dcmHeader correct.

      sprintf(zozo, "%s.DCM", toto.c_str());
      printf ("WriteDCM Implicit VR\n");
      f1->WriteDcmImplVR(zozo);
      break;

   case 'x' :
              // ecriture d'un fichier DICOM Explicit VR 
              // à partir d'un dcmHeader correct.

      sprintf(zozo, "%s.XDCM", toto.c_str());
      std::cout << "WriteDCM Explicit VR" << std::endl;
      f1->WriteDcmExplVR(zozo);
      break;

   case 'r' :
             //  Ecriture d'un Raw File, a afficher avec 
             // affim filein= dimx= dimy= nbit= signe=

      sprintf(zozo, "%s.RAW", toto.c_str());
      std::cout << "WriteRaw" << std::endl;
      f1->WriteRawData(zozo);
      break;

   case 'v' :

     if ( f1->GetHeader()->GetBitsAllocated() == 8)
     {
        std::cout << "videoinv for 8 bits" << std::endl;
        for (int i=0; i<dataSize; i++) 
        {
           ((uint8_t*)imageData)[i] += 127;
        }
     }
     else
     {
        std::cout << "videoinv for 16 bits" << std::endl;    
        for (int i=0; i<dataSize/2; i++) 
        {
           ((uint16_t*)imageData)[i] += 60000; //32767;
        }
     }
     sprintf(zozo, "%s.VDCM", toto.c_str());
     printf ("WriteDCM Explicit VR + VideoInv\n");
     f1->WriteDcmExplVR(zozo);
     break;

   }
  return 0;
}

