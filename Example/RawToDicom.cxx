/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: RawToDicom.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/16 16:38:24 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

/**
 * Writes a Dicom file from a Raw File
 * User has to supply parameters. 
 * 
 */
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmDebug.h"
#include "gdcmArgMgr.h"

#include <iostream>
#include <sstream>



int main(int argc, char *argv[])
{

   START_USAGE(usage)
   " \n RawToDicom : \n                                                       ",
   " Writes a Dicom file from a Raw File                                      ",
   " usage: RawToDicom filein=inputFileName                                   ",
   "                   fileout=outputFileName                                 ",
   "                   rows=nb of Rows                                        ",
   "                   lines=nb of Lines,                                     ",
   "                   pixeltype={8U|8S|16U|16S}                              ",
   "                   [frames = nb of Frames] //defaulted to 1               ",
   "                   [samples = {1|3}}       //defaulted to 1(1:Gray,3:RGB) ",
   "                   [patientname = Patient's name]                         ",
   "                   [debug]                                                ",
   "                                                                          ",
   "      debug      : user wants to run the program in 'debug mode'          ",
   FINISH_USAGE
   

   // Initialize Arguments Manager   
   gdcm::ArgMgr *am= new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage") )
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 1;
   }

   char *inputFileName  = am->ArgMgrGetString("filein",(char *)0);
   char *outputFileName = am->ArgMgrGetString("fileout",(char *)0);   
   //char *dirName        = am->ArgMgrGetString("dirin",(char *)0);
   
   char *patientName = am->ArgMgrGetString("patientname",(char *)0);
   
   int nX = am->ArgMgrWantInt("rows", usage);
   int nY = am->ArgMgrWantInt("lines", usage);
   int nZ = am->ArgMgrGetInt("frames", 1);
   int samplesPerPixel = am->ArgMgrGetInt("samples", 1);
   
   
   char *pixelType = am->ArgMgrWantString("pixeltype", usage);
   
   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();

   /* if unused Param we give up */
   if ( am->ArgMgrPrintUnusedLabels() )
   {
      am->ArgMgrUsage(usage);
      delete am;
      return 1;
   } 

   delete am;  // we don't need Argument Manager any longer

   // ----------- End Arguments Manager ---------
   
  
 // Read the Raw file  
   std::ifstream *Fp = new std::ifstream(inputFileName, std::ios::in | std::ios::binary);
   if ( ! *Fp )
   {   
      std::cout << "Cannot open file: " << inputFileName;
      delete Fp;
      Fp = 0;
      return 0;
   }  

   std::string strPixelType(pixelType);
   int pixelSign;
   int pixelSize;
   
   if (strPixelType == "8S")
   {
      pixelSize = 1;
      pixelSign = 0;
   }
   else  if (strPixelType == "8U")
   {
      pixelSize = 1;
      pixelSign = 1;
   }
   else  if (strPixelType == "16S")
   {
      pixelSize = 2;
      pixelSign = 0;
   }   
   else  if (strPixelType == "16U")
   {
      pixelSize = 2;
      pixelSign = 1;
   }      

   int dataSize =  nX*nY*nZ*pixelSize*samplesPerPixel;
   uint8_t *pixels = new uint8_t[dataSize];
   
   Fp->read((char*)pixels, (size_t)dataSize);
  

// Create an empty FileHelper

   gdcm::FileHelper *fileH = gdcm::FileHelper::New();
 
 // Get the (empty) image header.  
   gdcm::File *fileToBuild = fileH->GetFile();
   std::ostringstream str;

   // Set the image size
   str.str("");
   str << nX;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0011); // Columns
   str.str("");
   str << nY;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0010); // Rows
   
   str.str("");
   str << nZ;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0008); // Number of Frames

   // Set the pixel type
   
   str.str("");
   str << pixelSize*8;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0100); // Bits Allocated

   str.str("");
   str << pixelSize*8;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0101); // Bits Stored

   str.str("");
   str << ( pixelSize*8 - 1 );
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0102); // High Bit

   str.str("");
   str << pixelSign;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0103); // Pixel Representation

   str.str("");
   str << samplesPerPixel;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0002); // Samples per Pixel

   if (strlen(patientName) != 0)
      fileToBuild->InsertEntryString(patientName,0x0010,0x0010); // Patient's Name

// Set the image Pixel Data
   fileH->SetImageData(pixels,dataSize);

// Set the writting mode and write the image
   fileH->SetWriteModeToRaw();

 // Write a DICOM Explicit VR file
   fileH->SetWriteTypeToDcmExplVR();

   if( !fileH->Write(outputFileName ) )
   {
      std::cout << "Failed for [" << outputFileName << "]\n"
                << "           File is unwrittable\n";
   }

   fileH->Delete();

   delete[] pixels;
   return 1;
}
