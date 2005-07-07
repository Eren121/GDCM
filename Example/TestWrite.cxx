/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestWrite.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/07 17:31:53 $
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

#include <iostream>

int main(int argc, char *argv[])
{  
   std::string fileNameToWrite;

   gdcm::File *f;
   gdcm::FileHelper *fh;

   void *imageData;
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
      std::cout <<std::endl << "-------- Test gdcmFile ------" <<std::endl;
      f = new gdcmFileHelper(argv[1]);
      if (!fh->GetFile()->IsReadable()) {
         std::cout << "Sorry, not a DICOM / ACR File"  <<std::endl;
         exit(0);
      }
      std::cout << std::endl << "----------------------> after new gdcmFile"
                << std::endl;
      f->PrintEntry();
      std::cout <<std::endl <<"---------------------------------------" 
                <<std::endl;
   }
*/

   std::cout << std::endl
             << "--------------------- file :" << argv[1] 
             << std::endl;
     
   std::string fileName = argv[1]; 
   std::string mode = argv[2];

   //f = new gdcm::File( fileName.c_str() );

   // new style :
   f = new gdcm::File( );
   f->SetLoadMode(0);
   f->SetFileName( fileName );
   bool res = f->Load();  
   if ( !res )
   {
       std::cerr << "Sorry, not a Readable DICOM / ACR File"  <<std::endl;
       return 0;
   }
  // f->Print(); 
   
   fh = new gdcm::FileHelper(f);
   dataSize = fh->GetImageDataSize();
   imageData= fh->GetImageData();


// ---     

   std::cout <<std::endl <<" dataSize " << dataSize << std::endl;
   int nX,nY,nZ,sPP,planarConfig;
   std::string pixelType, transferSyntaxName;
   nX=f->GetXSize();
   nY=f->GetYSize();
   nZ=f->GetZSize();
   std::cout << " DIMX=" << nX << " DIMY=" << nY << " DIMZ=" << nZ << std::endl;

   pixelType    = f->GetPixelType();
   sPP          = f->GetSamplesPerPixel();
   planarConfig = f->GetPlanarConfiguration();
   
   std::cout << " pixelType="           << pixelType 
             << " SampleserPixel="      << sPP
             << " PlanarConfiguration=" << planarConfig 
             << " PhotometricInterpretation=" 
                                << f->GetEntryValue(0x0028,0x0004) 
             << std::endl;

   int numberOfScalarComponents=f->GetNumberOfScalarComponents();
   std::cout << "NumberOfScalarComponents " << numberOfScalarComponents <<std::endl;
   transferSyntaxName = f->GetTransferSyntaxName();
   std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" << std::endl;

/*   
  if (  transferSyntaxName != "Implicit VR - Little Endian"
      && transferSyntaxName != "Explicit VR - Little Endian"     
      && transferSyntaxName != "Deflated Explicit VR - Little Endian"      
      && transferSyntaxName != "Explicit VR - Big Endian"
      && transferSyntaxName != "Uncompressed ACR-NEMA"     ) {
      std::cout << std::endl << "==========================================="
                << std::endl; 
      fh->GetPixelReadConverter()->Print();
      std::cout << std::endl << "==========================================="
                << std::endl; 
   }
*/
   switch (mode[0])
   {
   case 'a' :
            // ecriture d'un fichier ACR 
            // à partir d'un dcmFile correct.

      fileNameToWrite = fileName + ".ACR";
      std::cout << "WriteACR" << std::endl;
      fh->WriteAcr(fileNameToWrite);
      break;

   case 'd' :  // Not document in the 'usage', because the method is knowed to be bugged. 

           // ecriture d'un fichier DICOM Implicit VR 
           // à partir d'un dcmFile correct.

      fileNameToWrite = fileName + ".DCM";
      std::cout << "WriteDCM Implicit VR" << std::endl;
      fh->WriteDcmImplVR(fileNameToWrite);
      break;

   case 'x' :
              // ecriture d'un fichier DICOM Explicit VR 
              // à partir d'un dcmFile correct.

      fileNameToWrite = fileName + ".XDCM";
      std::cout << "WriteDCM Explicit VR" << std::endl;
      fh->WriteDcmExplVR(fileNameToWrite);
      break;

   case 'r' :
             //  Ecriture d'un Raw File, a afficher avec 
             // affim filein= dimx= dimy= nbit= signe=

      fileNameToWrite = fileName + ".RAW";
      std::cout << "WriteRaw" << std::endl;
      fh->WriteRawData(fileNameToWrite);
      break;

   case 'v' :

     if ( fh->GetFile()->GetBitsAllocated() == 8)
     {
        std::cout << "videoinv for 8 bits" << std::endl;
        for (int i=0; i<dataSize; i++) 
        {
           ((uint8_t*)imageData)[i] = 255 - ((uint8_t*)imageData)[i];
        }
     }
     else
     {
        std::cout << "videoinv for 16 bits" << std::endl;    
        for (int i=0; i<dataSize/2; i++) 
        {
           ((uint16_t*)imageData)[i] =  65535 - ((uint16_t*)imageData)[i];
        }
     }
     fileNameToWrite = fileName + ".VDCM";
     std::cout << "WriteDCM Explicit VR + VideoInv" << std::endl;
     fh->WriteDcmExplVR(fileNameToWrite);
     break;

   }
   delete f;
   delete fh;
   return 0;
}

