/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: ReWrite.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/30 14:40:28 $
  Version:   $Revision: 1.10 $
                                                                                
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
   " Re write a full gdcm-readable Dicom image                              ",
   "     (usefull when the file header is not very straight).               ",
   "                                                                        ",
   " usage: ReWrite filein=inputFileName fileout=anonymizedFileName         ", 
   "       [mode=write mode] [noshadow] [noseq][debug]                      ", 
   "                                                                        ",
   "        mode = a (ACR), x (Explicit VR Dicom), r (RAW : only pixels)    ",
   "        noshadowseq: user doesn't want to load Private Sequences        ",
   "        noshadow : user doesn't want to load Private groups (odd number)",
   "        noseq    : user doesn't want to load Sequences                  ",
   "        rgb      : user wants to tranform LUT (if any) to RGB pixels    ",
   "        debug    : user wants to run the program in 'debug mode'        ",
   FINISH_USAGE

   // ----- Initialize Arguments Manager ------   
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage")) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }
   char *fileName = am->ArgMgrWantString("filein",usage);
   if ( fileName == NULL )
   {
      std::cout << "'filein= ...' is mandatory" << std::endl;
      delete am;
      return 0;
   }

   char *outputFileName = am->ArgMgrWantString("fileout",usage);
   if ( outputFileName == NULL )
   {
      std::cout << "'fileout= ...' is mandatory" << std::endl;
      delete am;
      return 0;
   }

   char *mode = am->ArgMgrGetString("mode",(char *)"X");

   int loadMode = GDCM_LD_ALL;
   if ( am->ArgMgrDefined("noshadowseq") )
      loadMode |= GDCM_LD_NOSHADOWSEQ;
   else 
   {
   if ( am->ArgMgrDefined("noshadow") )
         loadMode |= GDCM_LD_NOSHADOW;
      if ( am->ArgMgrDefined("noseq") )
         loadMode |= GDCM_LD_NOSEQ;
   }

   bool rgb = ( 0 != am->ArgMgrDefined("RGB") );

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();
 
   // if unused Params we give up
   if ( am->ArgMgrPrintUnusedLabels() )
   { 
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }

   delete am;  // we don't need Argument Manager any longer

   // ----------- End Arguments Manager ---------

   gdcm::File *f = new gdcm::File();
   f->SetLoadMode( loadMode );
   f->SetFileName( fileName );
   bool res = f->Load();  
   if ( !res )
   {
      delete f;
      return 0;
   }
  
   if (!f->IsReadable())
   {
       std::cerr << "Sorry, not a Readable DICOM / ACR File"  <<std::endl;
       delete f;
       return 0;
   }
   
   gdcm::FileHelper *fh = new gdcm::FileHelper(f);
   void *imageData; 
   int dataSize;
  
   if (rgb)
   {
      dataSize  = fh->GetImageDataSize();
      imageData = fh->GetImageData(); // somewhat important... can't remember
      fh->SetWriteModeToRGB();
   }
   else
   {
      dataSize  = fh->GetImageDataRawSize();
      imageData = fh->GetImageDataRaw();// somewhat important... can't remember
      fh->SetWriteModeToRaw();
   }

   if ( imageData == 0 ) // to avoid warning
   {
      std::cout << "Was unable to read pixels " << std::endl;
   }
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
   std::cout << "NumberOfScalarComponents " << numberOfScalarComponents 
             <<std::endl;
   transferSyntaxName = f->GetTransferSyntaxName();
   std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" 
             << std::endl;

   switch (mode[0])
   {
   case 'A' :
   case 'a' :
            // Writting an ACR file
            // from a full gdcm readable File

      std::cout << "WriteACR" << std::endl;
      fh->WriteAcr(outputFileName);
      break;

   case 'D' : // Not documented in the 'usage', because the method 
   case 'd' : //                             is known to be bugged. 
           // Writting a DICOM Implicit VR file
           // from a full gdcm readable File

      std::cout << "WriteDCM Implicit VR" << std::endl;
      fh->WriteDcmImplVR(outputFileName);
      break;

   case 'X' :
   case 'x' :
              // writting a DICOM Explicit VR 
              // from a full gdcm readable File

      std::cout << "WriteDCM Explicit VR" << std::endl;
      fh->WriteDcmExplVR(outputFileName);
      break;

   case 'R' :
   case 'r' :
             //  Writting a Raw File, 

      std::cout << "WriteRaw" << std::endl;
      fh->WriteRawData(outputFileName);
      break;

   }
   delete f;
   delete fh;
   return 0;
}

