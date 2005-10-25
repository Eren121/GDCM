/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exOverlaysACR.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/25 14:52:28 $
  Version:   $Revision: 1.9 $
                                                                                
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
#include "gdcmDocEntry.h"

#include <iostream>
#include <stdio.h> // for fseek... FIXME
#include <stdlib.h> // for atoi
 
 // WARNING :
 // unfinished : DO NOT to be used as is !
 
 /*
 // Example (sorry, we've got no more than this one ...)
 
V 0028|0010[US] [Rows] [256] x(100)
V 0028|0011[US] [Columns] [256] x(100)
V 0028|0030[DS] [Pixel Spacing] [01.56\1.56]
V 0028|0100[US] [Bits Allocated] [16] x(10)
V 0028|0101[US] [Bits Stored] [12] x(c)
V 0028|0102[US] [High Bit] [11] x(b)
V 0028|0103[US] [Pixel Representation] [0] x(0)
 
V 6000|0000[UL] [Group Length] [96] x(60)
V 6000|0010[US] [Rows] [256] x(100)
V 6000|0011[US] [Columns] [256] x(100)
V 6000|0040[CS] [Overlay Type] [R ]
V 6000|0050[SS] [Overlay Origin] [23601\8241] x(5c31)
V 6000|0100[US] [Overlay Bits Allocated] [16] x(10)
V 6000|0102[US] [Overlay Bit Position] [12] x(c)
...
...
V 6006|0000[UL] [Group Length] [96] x(60)
V 6006|0010[US] [Rows] [256] x(100)
V 6006|0011[US] [Columns] [256] x(100)
V 6006|0040[CS] [Overlay Type] [R ]
V 6006|0050[SS] [Overlay Origin] [23601\8241] x(5c31)
V 6006|0100[US] [Overlay Bits Allocated] [16] x(10)
V 6006|0102[US] [Overlay Bit Position] [15] x(f)
 */
 
int main(int argc, char *argv[])
{  
   gdcm::File *f;
 
   //gdcm::Debug::DebugOn();

   std::cout << "------------------------------------------------" << std::endl;
   std::cout << "Gets the 'Overlays' from a full gdcm-readable ACR-NEMA "
             << "uncompressed image" << std::endl;
   std::cout << "Writes them in DicomV3 files named 'gdcmOverlay-xxx.dcm'"
             << std::endl;
   std::cout << "(Note :  we just have ONE image : "
             << "SIEMENS_GBS_III-16-ACR_NEMA_1.acr)"
             << std::endl;
   std::cout << "------------------------------------------------" << std::endl;

   std::string fileName;
   if( argc > 1 )
      fileName = argv[1];
   else
      fileName = "SIEMENS_GBS_III-16-ACR_NEMA_1.acr";  

   std::cout << fileName << std::endl;
// ============================================================
//   Read the input image.
// ============================================================

   //std::cout << argv[1] << std::endl;

   f = gdcm::File::New( );

   f->SetLoadMode(gdcm::LD_NOSEQ | gdcm::LD_NOSHADOW);
   f->SetFileName( fileName );
   bool res = f->Load();  

   if( gdcm::Debug::GetDebugFlag() )
   {
      std::cout << "---------------------------------------------" << std::endl;
      f->Print();
      std::cout << "---------------------------------------------" << std::endl;
   }
   if (!res) {
       std::cout << "Sorry, " << fileName <<"  not a gdcm-readable "
           << "DICOM / ACR File"
           <<std::endl;
      f->Delete();
      return 0;
   }
   std::cout << " ... is readable " << std::endl;

// ============================================================
//   Check whether image contains Overlays ACR-NEMA style.
// ============================================================

   int bitsAllocated = f->GetBitsAllocated();
   if ( bitsAllocated <= 8 )
   {
      std::cout << " 8 bits pixel image cannot contain Overlays " << std::endl;
      f->Delete();
      return 0;
   }
   std::string s1 = f->GetEntryString(0x6000, 0x0102);
   if (s1 == gdcm::GDCM_UNFOUND)
   {
      std::cout << " Image doesn't contain any Overlay " << std::endl;
      f->Delete();
      return 0;
   }
   std::cout << " File is read! " << std::endl;

   
// ============================================================
//   Load the pixels in memory.
// ============================================================

   // We don't use a gdcm::FileHelper, since it rubs out 
   // the 'non image' bits of the pixels...

   int nx = f->GetXSize();
   int ny = f->GetYSize();
 
   std::cout << "Dimensions " << ny << "  " <<ny << std::endl;

   gdcm::DocEntry *p = f->GetDocEntry(f->GetGrPixel(), f->GetNumPixel());
   if (p == 0)
      std::cout << "Pixels element  not found" << std::endl;
   else
      std::cout << "Pixels element FOUND" << std::endl;

   int offset = (int)(p->GetOffset());

   std::cout << "Offset " << offset << std::endl;

   FILE *fp = fopen(fileName.c_str(), "r");

   if (fp == 0)
   {
      std::cout << "Unable to open File" << std::endl;
      f->Delete();
      return 0;
   }
   else
      std::cout << "File open successfully" << std::endl; 
  
   fseek(fp, (long) offset,SEEK_SET);      
   uint16_t *pixels = new uint16_t[nx*ny];
   size_t lgt = fread(pixels, 1,  nx*ny*sizeof( uint16_t) , fp );   

   if ( lgt != (size_t)nx*ny*sizeof( uint16_t) )
   {
       std::cout << "Sorry, Pixels of" << fileName << "  are not "
                 << "readable. expected length :" << nx*ny 
                 << "  " << "read length : " << lgt
                 << std::endl;
       f->Delete();
       delete pixels;  
       return 0;
   }
   else
   {
      std::cout << "Pixels read as expected : length = " << lgt << std::endl;
   } 


// ============================================================
//   Get each overlay Bit into an image
// ============================================================
                                         
   uint8_t *tabPixels = new uint8_t[nx*ny]; // uint8 is enought to hold 1 bit !
   
   uint16_t currentOvlGroup = 0x6000;
   std::string strOvlBitPosition;
   int ovlBitPosition;
   uint16_t mask;
   int i = 0;
   uint16_t overlayLocation;
   std::ostringstream str;
   std::string strOverlayLocation;
   gdcm::File *fileToBuild = 0;
   gdcm::FileHelper *fh = 0;

      
   while ( (strOvlBitPosition = f->GetEntryString(currentOvlGroup, 0x0102)) 
            != gdcm::GDCM_UNFOUND )
   {

      strOverlayLocation = f->GetEntryString(currentOvlGroup, 0x0200);
      if ( strOverlayLocation != gdcm::GDCM_UNFOUND )
      {
         overlayLocation = atoi(strOverlayLocation.c_str());
         if ( overlayLocation != f->GetGrPixel() )
         {
            std::cout << "Big Trouble : Overlays are NOT in the Pixels Group "
                      << std::hex << "(" << overlayLocation << " vs " 
                      << f->GetGrPixel() << std::endl;
            // Actually, here, we should (try to) read the overlay location
            // and go on the job.
            continue;
         }
      }
      ovlBitPosition = atoi(strOvlBitPosition.c_str());
      mask = 1 << ovlBitPosition; 
      std::cout << "Mask :[" <<std::hex << mask << "]" << std::endl;          
      for (int j=0; j<nx*ny ; j++)
      {
         if( gdcm::Debug::GetDebugFlag() )
            if (pixels[j] >= 0x1000)// if it contains at least one overlay bit
               printf("%d : %04x\n",j, pixels[j]);

         if ( (pixels[j] & mask) == 0 )
            tabPixels[j] = 0;
         else
            tabPixels[j] = 128;
      }
      if( gdcm::Debug::GetDebugFlag() )
         std::cout << "About to built empty file"  << std::endl;

      fileToBuild = gdcm::File::New();

      if( gdcm::Debug::GetDebugFlag() )
         std::cout << "Finish to built empty file"  << std::endl;

      str.str("");
      str << nx;
      fileToBuild->InsertEntryString(str.str(),0x0028,0x0011); // Columns
      str.str("");
      str << ny;
      fileToBuild->InsertEntryString(str.str(),0x0028,0x0010); // Rows

      fileToBuild->InsertEntryString("8",0x0028,0x0100); // Bits Allocated
      fileToBuild->InsertEntryString("8",0x0028,0x0101); // Bits Stored
      fileToBuild->InsertEntryString("7",0x0028,0x0102); // High Bit
      fileToBuild->InsertEntryString("0",0x0028,0x0103); // Pixel Representation
      fileToBuild->InsertEntryString("1",0x0028,0x0002); // Samples per Pixel

      fileToBuild->InsertEntryString("MONOCHROME2 ",0x0028,0x0004);
      // Other mandatory fields will be set automatically,
      // just before Write(), by FileHelper::CheckMandatoryElements()

      if( gdcm::Debug::GetDebugFlag() )
         std::cout << "-------------About to built FileHelper"  << std::endl;

      fh = gdcm::FileHelper::New(fileToBuild);

      if( gdcm::Debug::GetDebugFlag() )
         std::cout << "-------------Finish to built FileHelper"  << std::endl;

      fh->SetImageData(tabPixels,nx*ny);
      fh->SetWriteTypeToDcmExplVR();

      str.str("");
      str<<"gdcmOverlay-"<<i << ".dcm";
      //   Write the current 'overlay' file

      if( !fh->Write(str.str()) )
      {
         std::cout << "Failed\n"
                   << "File in unwrittable\n";
         fh->Delete();
         if (fileToBuild)
            fileToBuild->Delete();
         delete pixels;
         delete tabPixels;
         return 0;
      }
      else
      {
         std::cout << "File written successfully" << std::endl;
      }
      currentOvlGroup += 2;
      i++;
   }
    
   if (f)
      fh->Delete();
   if (fileToBuild)
      fileToBuild->Delete();
   f->Delete();
   delete pixels;
   delete tabPixels;

   return 0;
}

