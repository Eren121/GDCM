/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: Write.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/20 16:16:58 $
  Version:   $Revision: 1.15 $
                                                                                
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

int main(int argc, char* argv[])
{  
   std::string zozo;

   gdcm::Header* e1;
   gdcm::FileHelper  * f1;

   //gdcmDocument * d;  //not used
   uint8_t* imageData;
   int dataSize;

   if (argc < 3) {
         std::cerr << "usage: " << std::endl 
                   << argv[0] << " fileName writtingMode "
                << std::endl 
                   << "(a : ACR, d : DICOM Implicit VR,"
                   << " x : DICOM Explicit VR,  r : RAW)"
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
     
   std::string toto = argv[1]; 

   e1 = new gdcm::Header( toto.c_str() );
   if (!e1->IsReadable()) {
       std::cerr << "Sorry, not a Readable DICOM / ACR File"  <<std::endl;
       return 0;
   }
  // e1->Print(); 
   
   f1 = new gdcm::FileHelper(e1);
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
                                << e1->GetEntry(0x0028,0x0004) 
             << std::endl;

   int numberOfScalarComponents=e1->GetNumberOfScalarComponents();
   std::cout << "NumberOfScalarComponents " << numberOfScalarComponents <<std::endl;
   transferSyntaxName = e1->GetTransferSyntaxName();
   std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" << std::endl;
   
/*   if (  transferSyntaxName != "Implicit VR - Little Endian"
      && transferSyntaxName != "Explicit VR - Little Endian"     
      && transferSyntaxName != "Deflated Explicit VR - Little Endian"      
      && transferSyntaxName != "Explicit VR - Big Endian"
      && transferSyntaxName != "Uncompressed ACR-NEMA"     ) {
      std::cout << std::endl << "==========================================="
                << std::endl; 
      f1->GetPixelReadConverter()->Print();
      std::cout << std::endl << "==========================================="
                << std::endl; 
   }*/
   imageData= f1->GetImageData();
   (void)imageData;  // to avoid warnings

   switch (argv[2][0])
   {
   case 'a' :
            // ecriture d'un fichier ACR 
            // à partir d'un dcmHeader correct.

      zozo = toto + ".ACR";
      std::cout << "WriteACR" << std::endl;
      f1->WriteAcr(zozo);
      break;

   case 'd' :
           // ecriture d'un fichier DICOM Implicit VR 
           // à partir d'un dcmHeader correct.

      zozo = toto + ".DCM";
      std::cout << "WriteDCM Implicit VR" << std::endl;
      f1->WriteDcmImplVR(zozo);
      break;

   case 'x' :
              // ecriture d'un fichier DICOM Explicit VR 
              // à partir d'un dcmHeader correct.

      zozo = toto + ".DCM";
      std::cout << "WriteDCM Implicit VR" << std::endl;
      f1->WriteDcmExplVR(zozo);
      break;

   case 'r' :
             //  Ecriture d'un Raw File, a afficher avec 
             // affim filein= dimx= dimy= nbit= signe=

      zozo = toto + ".RAW";
      std::cout << "WriteRaw" << std::endl;
      f1->WriteRawData(zozo);
      break;

   }
  return 0;
}

