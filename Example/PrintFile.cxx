/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PrintFile.cxx,v $
  Language:  C++
  Date:      $Date: 2004/11/24 16:39:17 $
  Version:   $Revision: 1.13 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include <iostream>
#include "gdcm.h"

int main(int argc, char* argv[])
{
 
   gdcm::Header *e1;
   gdcm::File   *f1;
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
   
   e1= new gdcm::Header( fileName.c_str() );

   f1 = new gdcm::File(e1);

   e1->SetPrintLevel(2);
   
   e1->Print();
      
   std::cout << "\n\n" << std::endl; 

   int dataSize = f1->GetImageDataSize();
   std::cout <<std::endl;
   std::cout <<" dataSize " << dataSize << std::endl;
   std::cout <<" dataSizeRaw " << f1->GetImageDataSizeRaw() << std::endl;
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
             << " SamplesPerPixel="     << sPP
             << " PlanarConfiguration=" << planarConfig 
             << std::endl 
             << " PhotometricInterpretation=" 
                                << e1->GetEntryByNumber(0x0028,0x0004)
             << std::endl;

   int numberOfScalarComponents=e1->GetNumberOfScalarComponents();
   std::cout << " NumberOfScalarComponents " << numberOfScalarComponents <<std::endl
             << " LUT=" << (e1->HasLUT() ? "TRUE" : "FALSE")
             << std::endl;

  
  if ( e1->GetEntryByNumber(0x0002,0x0010) == gdcm::GDCM_NOTLOADED ) {
     std::cout << "Transfert Syntax not loaded. " << std::endl
               << "Better you increase MAX_SIZE_LOAD_ELEMENT_VALUE"
               << std::endl;
      return 0;
  }
  
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
         f1->GetPixelConverter()->Print();
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
