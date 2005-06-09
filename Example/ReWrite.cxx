/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: ReWrite.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/09 21:32:14 $
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

#include "gdcmArgMgr.h"

#include <iostream>

int main(int argc, char *argv[])
{
   START_USAGE(usage)
   " \n ReWrite :\n",
   " Re write a full gdcm-readable Dicom image",
   "     (usefull when the file header is not very straight).",
   "                 ",
   " usage: ReWrite filein=inputFileName fileout=anonymizedFileName ",
   "       [mode=write mode] [noshadow] [noseq][debug] ", 
   "        mode = a (ACR), x (Explicit VR Dicom), r (RAW : only pixels)",
   "        noshadow : user doesn't want to load Private groups (odd number)",
   "        noseq    : user doesn't want to load Sequences ",
   "        debug    : user wants to run the program in 'debug mode' ",
   FINISH_USAGE

   // ----- Initialize Arguments Manager ------   
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);
  
   if (am->ArgMgrDefined("usage")) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }
   char *fileName = am->ArgMgrWantString("filein",usage);
   if ( fileName == NULL )
   {
      delete am;
      return 0;
   }

   char *outputFileName = am->ArgMgrWantString("fileout",usage);
   if ( outputFileName == NULL )
   {
      delete am;
      return 0;
   }

   char *mode = am->ArgMgrGetString("filein","X");

   int loadMode;
   if ( am->ArgMgrDefined("noshadow") && am->ArgMgrDefined("noseq") )
       loadMode = NO_SEQ | NO_SHADOW;  
   else if ( am->ArgMgrDefined("noshadow") )
      loadMode = NO_SHADOW;
   else if ( am->ArgMgrDefined("noseq") )
      loadMode = NO_SEQ;
   else
      loadMode = 0;

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();
 
   // if unused Param we give up
   if ( am->ArgMgrPrintUnusedLabels() )
   { 
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }

   delete am;  // we don't need Argument Manager any longer

   int dataSize;

   gdcm::File *e1 = new gdcm::File();
   e1->SetLoadMode(loadMode);

   bool res = e1->Load( fileName );
   if ( !res )
   {
      delete e1;
      delete am;
      return 0;
   }
   if (!e1->IsReadable())
   {
       std::cerr << "Sorry, not a Readable DICOM / ACR File"  <<std::endl;
       delete e1;
       delete am; 
       return 0;
   }
   
   gdcm::FileHelper *f1 = new gdcm::FileHelper(e1);

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
                                << e1->GetEntryValue(0x0028,0x0004) 
             << std::endl;

   int numberOfScalarComponents=e1->GetNumberOfScalarComponents();
   std::cout << "NumberOfScalarComponents " << numberOfScalarComponents <<std::endl;
   transferSyntaxName = e1->GetTransferSyntaxName();
   std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" << std::endl;
   
   f1->GetImageData(); // somewhat important... can't remember

   switch (mode[0])
   {
   case 'A' :
            // Writting an ACR file
            // from a full gdcm readable File

      std::cout << "WriteACR" << std::endl;
      f1->WriteAcr(outputFileName);
      break;

   case 'D' : // Not documented in the 'usage', because the method is known to be bugged. 

           // Writting a DICOM Implicit VR file
           // from a full gdcm readable File

      std::cout << "WriteDCM Implicit VR" << std::endl;
      f1->WriteDcmImplVR(outputFileName);
      break;

   case 'X' :
              // writting a DICOM Explicit VR 
              // from a full gdcm readable File

      std::cout << "WriteDCM Explicit VR" << std::endl;
      f1->WriteDcmExplVR(outputFileName);
      break;

   case 'R' :
             //  Writting a Raw File, 

      std::cout << "WriteRaw" << std::endl;
      f1->WriteRawData(outputFileName);
      break;

   }
   delete e1;
   delete f1;
   delete am;
   return 0;
}

