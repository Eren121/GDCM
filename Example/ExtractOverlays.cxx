/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: ExtractOverlays.cxx,v $
  Language:  C++
  Date:      $Date: 2007/05/23 14:18:04 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/


#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmCommon.h"
#include "gdcmDebug.h"
#include "gdcmDataEntry.h"
#include "gdcmDirList.h"

#include "gdcmArgMgr.h"
#include <iostream>

// Each BIT of Overlay Data (0x6000,0x3000) corresponds 
// to a BYTE of overlay image.
void explodeByte(unsigned char byte, unsigned char* result) 
{
   unsigned char mask = 1;
   for (int i=0;i<8;i++) 
   {
      if ((byte & mask)==0) 
         result[i]=0;
      else 
         result[i]=1;
      mask<<=1;
   }
   return;
}


int main(int argc, char *argv[])
{
   START_USAGE(usage)
   " \n ExtractOverlays :\n                                                   ",
   " Extract overlay images from all DICOM image within a directory           ",
   "          Warning : probably segfaults if no overlay                      ",
   " usage: ExtractOverlays dirin=inputDirectoryName  [debug]                 ",
   "        debug    : developper wants to run the program in 'debug mode'    ",
   FINISH_USAGE

   // ----- Initialize Arguments Manager ------
   
   GDCM_NAME_SPACE::ArgMgr *am = new GDCM_NAME_SPACE::ArgMgr(argc, argv);

   if (argc == 1 || am->ArgMgrDefined("usage"))
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }
   
   const char *dirIn  = am->ArgMgrWantString("dirin", usage);

   if (am->ArgMgrDefined("debug"))
      GDCM_NAME_SPACE::Debug::DebugOn();
      
   if (am->ArgMgrDefined("warning"))
      GDCM_NAME_SPACE::Debug::WarningOn();
      
   // if unused Param we give up
   if ( am->ArgMgrPrintUnusedLabels() )
   {
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }

   delete am;  // we don't need Argument Manager any longer


   // ========================== Now, we can do the job! ================ 


   // ======================== more checking on the params ==============

   if ( ! GDCM_NAME_SPACE::DirList::IsDirectory(dirIn) )
   {
      std::cout << "KO : [" << dirIn << "] is not a Directory." << std::endl;
      return 0;

   }
 
   char outputFileName[1024]; // Hope it's enough for a file name!
   
   GDCM_NAME_SPACE::File *f;
        
   GDCM_NAME_SPACE::DirList dirList(dirIn,true); // gets (recursively) the file list
   GDCM_NAME_SPACE::DirListType fileList = dirList.GetFilenames();
   for( GDCM_NAME_SPACE::DirListType::iterator it  = fileList.begin();
                                 it != fileList.end();
                                 ++it )
   {
   //   Just to see *all* the file names:
   //   std::cout << "file [" << it->c_str() << "]" << std::endl;     
   
   //   Read the input file.

   f = GDCM_NAME_SPACE::File::New(  );
   f->SetLoadMode( GDCM_NAME_SPACE::LD_ALL );
   f->SetFileName( it->c_str() );
   
   f->AddForceLoadElement(0x6000,0x3000);  // Overlay Data
   f->AddForceLoadElement(0x6002,0x3000); 
   f->AddForceLoadElement(0x6004,0x3000); 
   f->AddForceLoadElement(0x6006,0x3000);    
   f->AddForceLoadElement(0x6008,0x3000);    
   f->AddForceLoadElement(0x600a,0x3000); 
   f->AddForceLoadElement(0x600c,0x3000); 
   f->AddForceLoadElement(0x600e,0x3000);
             
   int res = f->Load();

   if ( !res )
   {
       std::cerr << "Sorry, " << it->c_str() <<"  not a gdcm-readable "
                 << "DICOM / ACR File" <<std::endl;
       f->Delete();
       continue;
   }
   std::cout << " ... is readable " << std::endl;

   // ============================================================
   //   Load the Overlays in memory (the first one)
   // ============================================================

/// \todo : deal with *each* overlay Data Element (not only the first one!)

   uint16_t ovlyGroup = 0x6000;

   GDCM_NAME_SPACE::DataEntry *e = f->GetDataEntry(ovlyGroup, 0x3000);  
   if (e == 0)
   {
      std::cout << " Image doesn't contain any Overlay " << std::endl;
      f->Delete();
      continue;
   }   
   std::cout << " File is read! " << std::endl;

   uint8_t *overlay = (uint8_t *)(e->GetBinArea());
   if ( overlay == 0 )
   {
       std::cerr << "Sorry, Overlays of" << it->c_str() <<"  are not "
                 << " gdcm-readable."    << std::endl;
       f->Delete();
       continue;
   }

   // ============================================================
   //  Image data generation.
   // ============================================================   

   unsigned int dimX= f->GetXSize();
   unsigned int dimY= f->GetYSize();

   unsigned int dimXY=dimX*dimY;
   std::cout << "DimX : "<< dimX <<" DimY : " << dimY 
             << " DimXY : " <<dimXY << std::endl;
   unsigned char *outputData = new unsigned char[dimXY];

   unsigned char *result=outputData;
   for (unsigned int i=0;i<(dimXY/8);i++) 
   {
      explodeByte(overlay[i], result);
      result+=8;
   }
   
   // ============================================================
   //   Write a new file
   // ============================================================
   GDCM_NAME_SPACE::File *f2;
   f2 = GDCM_NAME_SPACE::File::New(  );
   
   char temp[256];
   
   sprintf(temp,"%d ",dimX);
   f2->InsertEntryString(temp,0x0028,0x0011, "US"); // Columns
   sprintf(temp,"%d ",dimY);
   f2->InsertEntryString(temp,0x0028,0x0010, "US"); // Rows

   f2->InsertEntryString("8",0x0028,0x0100, "US"); // Bits Allocated
   f2->InsertEntryString("8",0x0028,0x0101, "US"); // Bits Stored
   f2->InsertEntryString("7",0x0028,0x0102, "US"); // High Bit
   f2->InsertEntryString("0",0x0028,0x0103, "US"); // Pixel Representation
   f2->InsertEntryString("1",0x0028,0x0002, "US"); // Samples per Pixel
   f2->InsertEntryString("MONOCHROME2 ",0x0028,0x0004, "LO");  
   
   sprintf(outputFileName, "%s.ovly.dcm",it->c_str() );

   // feel free to add any field (Dicom Data Entry) you like, here.
   // ...
   GDCM_NAME_SPACE::FileHelper *fh = GDCM_NAME_SPACE::FileHelper::New(f2);
       
   fh->SetImageData(outputData,dimXY);
   fh->WriteDcmExplVR(outputFileName);
   std::cout <<"End WriteOverlayImage" << std::endl;

   delete outputData;
   f->Delete();
   f2->Delete();  
   fh->Delete();
   
}  // end of loop on files
 
   return 0;
}

