#include <iostream>
#include "gdcm.h"

int main(int argc, char* argv[])
{
 
   gdcmHeader *e1;
   gdcmFile   *f1;
   std::string fileName;   
   if (argc != 2) {
      std::cout << " usage : PrintDocument fileName" << std::endl;
   }

   if (argc > 1) {
      fileName=argv[1];
   } else {
      fileName += GDCM_DATA_ROOT;
      fileName += "/test.acr";
   }
   
   e1= new gdcmHeader( fileName.c_str(), false );

   f1 = new gdcmFile(e1);

   e1->SetPrintLevel(2);
   
   e1->Print();
      
   std::cout << "\n\n" << std::endl; 

   int dataSize = f1->GetImageDataSize();
   std::cout <<std::endl <<" dataSize " << dataSize << std::endl;
   int nX,nY,nZ,sPP,planarConfig;
   std::string pixelType;
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
             << std::endl 
             << " PhotometricInterpretation=" 
                                << e1->GetEntryByNumber(0x0028,0x0004)
             << std::endl;

   int numberOfScalarComponents=e1->GetNumberOfScalarComponents();
   std::cout << " NumberOfScalarComponents " << numberOfScalarComponents <<std::endl;

   std::string transferSyntaxName = e1->GetTransfertSyntaxName();
   std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" << std::endl;
   
   if (  transferSyntaxName != "Implicit VR - Little Endian"
      && transferSyntaxName != "Explicit VR - Little Endian"     
      && transferSyntaxName != "Deflated Explicit VR - Little Endian"      
      && transferSyntaxName != "Explicit VR - Big Endian"
      && transferSyntaxName != "Uncompressed ACR-NEMA"     )
  {
      std::cout << std::endl << "==========================================="
                  << std::endl; 
         f1->ParsePixelData();
      std::cout << std::endl << "==========================================="
                  << std::endl; 
   }      
   
   if(e1->IsReadable())
      std::cout <<std::endl<<fileName<<" is Readable"<<std::endl;
   else
      std::cout <<std::endl<<fileName<<" is NOT Readable"<<std::endl;
   std::cout<<std::flush;
   delete e1;

   return 0;
   
}
