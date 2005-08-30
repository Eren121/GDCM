/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exPrintWritePrint.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/30 15:13:06 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"


int main(int argc, char *argv[])
{  
   gdcm::File *f;
   gdcm::FileHelper *fh;
   std::string fileNameToWrite;
   void *imageData;
   int dataSize;

   std::cout << " This program allows to see at a glance" << std::endl;
   std::cout << " if the gdcm::File remains unimpaired"   << std::endl;
   std::cout << " after a Write"                          << std::endl;
   std::cout << " In a future step, we could move it to"  << std::endl;
   std::cout << " gdcm Testing, for a systematic checking"<< std::endl;
   std::cout << " of  the entire dataset"                 << std::endl;
   std::cout << " Later ..."                              << std::endl;


   if (argc < 3) {
         std::cerr << "usage: " << std::endl 
                   << argv[0] << " OriginalFileName writtingMode "
                   << std::endl 
                   << " a : ACR, produces a file named OriginalFileName.ACR"
                   << " x : DICOM Explicit VR, produces a file named OriginalFileName.XDCM"
                   << " d : DICOM Implicit VR, produces a file named OriginalFileName.DCM"
                   << "                  WARNING : bugggggggg on shadow SQ with endianness change !"
                   << " r : RAW, produces a file named OriginalFileName.RAW"
                   << std::endl;
         return 0;
   }

   std::cout << std::endl
             << "--------------------- file :" << argv[1] 
             << std::endl;
     
   std::string fileName = argv[1]; 
   std::string mode = argv[2];

   f = new gdcm::File( );
   f->SetLoadMode( gdcm::LD_NOSEQ );
   f->SetFileName( fileName );
   f->Load( );

   if (!f->IsReadable())
   {
       std::cerr << "Sorry, not a Readable DICOM / ACR File"  <<std::endl;
       return 0;
   }
   
   fh = new gdcm::FileHelper(f);
  // ---     

   f->Print();

   imageData= fh->GetImageData();
   dataSize = fh->GetImageDataSize();

   fh->SetWriteModeToRGB();

   switch (mode[0])
   {
   case 'a' :
            // ecriture d'un fichier ACR 
            // à partir d'un dcmFile correct.

      fileNameToWrite = fileName + ".ACR";
      std::cout << "WriteACR" << std::endl;
      fh->WriteAcr(fileNameToWrite);
      break;

   case 'd' : 

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
   }

   std::cout << "-----------------------------------------------------------------" 
          << std::endl;
   f->Print();
   delete f;
   delete fh;
   return 0;
}

