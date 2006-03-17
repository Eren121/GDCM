/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: ReWriteExtended.cxx,v $
  Language:  C++
  Date:      $Date: 2006/03/17 14:56:29 $
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
#include "gdcmUtil.h"
#include "gdcmArgMgr.h"

#include <iostream>

int main(int argc, char *argv[])
{
   START_USAGE(usage)
   " \n ReWriteExtended :\n",
   " Re write a full gdcm-readable Dicom image using new features           ",
   "     (DO NOT use right now; checking no achieved !).                    ",
   "                                                                        ",
   " usage: ReWriteExtended filein=inputFileName fileout=outputFileName     ",
   "      filecontent = 1 : USER_OWN_IMAGE                                  ",
   "                  = 2 : FILTERED_IMAGE                                  ",
   "                  = 3 : CREATED_IMAGE                                   ",
   "                  = 4 : UNMODIFIED_PIXELS_IMAGE                         ", 
   "       [mode=write mode] [noshadow] [noseq][debug]                      ", 
   "                                                                        ",
   "        mode = a (ACR), x (Explicit VR Dicom), r (RAW : only pixels)    ",
   "        noshadowseq: user doesn't want to load Private Sequences        ",
   "        noshadow : user doesn't want to load Private groups (odd number)",
   "        noseq    : user doesn't want to load Sequences                  ",
   "        rgb      : user wants to transform LUT (if any) to RGB pixels   ",
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

   const char *mode = am->ArgMgrGetString("mode","X");
   
   int filecontent =  am->ArgMgrGetInt("filecontent", gdcm::UNMODIFIED_PIXELS_IMAGE);
   
   int loadMode = gdcm::LD_ALL;
   if ( am->ArgMgrDefined("noshadowseq") )
      loadMode |= gdcm::LD_NOSHADOWSEQ;
   else 
   {
   if ( am->ArgMgrDefined("noshadow") )
         loadMode |= gdcm::LD_NOSHADOW;
      if ( am->ArgMgrDefined("noseq") )
         loadMode |= gdcm::LD_NOSEQ;
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

   gdcm::File *f = gdcm::File::New();
   f->SetLoadMode( loadMode );
   f->SetFileName( fileName );
   bool res = f->Load();  
   if ( !res )
   {
      f->Delete();
      return 0;
   }
  
   if (!f->IsReadable())
   {
       std::cerr << "Sorry, not a Readable DICOM / ACR File"  <<std::endl;
       f->Delete();
       return 0;
   }
   
   gdcm::FileHelper *fh = gdcm::FileHelper::New(f);
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
             << f->GetEntryString(0x0028,0x0004) 
             << std::endl;

   int numberOfScalarComponents=f->GetNumberOfScalarComponents();
   std::cout << "NumberOfScalarComponents " << numberOfScalarComponents 
             <<std::endl;
   transferSyntaxName = f->GetTransferSyntaxName();
   std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" 
             << std::endl;


   // We trust user. (just an example; *never* trust an user !)  
   fh->SetContentType((gdcm::ImageContentType)filecontent);
   
   /// \todo Here, give the detail of operations a 'decent' user should perform,
   ///       according to what *he* wants to do.


   // an user shouldn't add images to a 'native' serie.
   // He is allowed to create his own Serie, within a 'native' Study :
   // if he wants to do so, he has to call gdcm::Util::GetUniqueUID 
   // only once for a given image set, belonging to a single 'user Serie'
   
   std::string SerieInstanceUID;   
   switch(filecontent)
   {
      case gdcm::USER_OWN_IMAGE :
         SerieInstanceUID = gdcm::Util::CreateUniqueUID();
         f->SetEntryString(SerieInstanceUID,0x0020,0x000e);
      break;
      
      case gdcm::FILTERED_IMAGE :
      /// \todo : to be finished!
      break;      

      case gdcm::CREATED_IMAGE :
      /// \todo : to be finished!
      break;

      case gdcm::UNMODIFIED_PIXELS_IMAGE :
      /// \todo : to be finished!
      break;      
   }

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
         // fh->WriteDcmExplVR(outputFileName);
         // Try this one :
         fh->SetWriteTypeToDcmExplVR();
         fh->Write(outputFileName);
         break;

      case 'R' :
      case 'r' :
      //  Writting a Raw File, 
         std::cout << "WriteRaw" << std::endl;
         fh->WriteRawData(outputFileName);
         break;
 
 // Just for fun :
 // Write a 'Video inverse' version of the file.
 // *Not* described, on purpose,  in the USAGE  
      case 'V' :
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
         std::cout << "WriteDCM Explicit VR + VideoInv" << std::endl;
         fh->WriteDcmExplVR(outputFileName);
         break;
   }

   f->Delete();
   fh->Delete();
   return 0;
}

