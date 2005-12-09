/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exOverlaysDCM.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/09 10:17:52 $
  Version:   $Revision: 1.1 $
                                                                                
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
#include "gdcmDataEntry.h"

#include <iostream>
#include <stdio.h> // for fseek... FIXME
#include <stdlib.h> // for atoi
 
 // WARNING :
 // unfinished : DO NOT to be used as is !
 

int main(int argc, char *argv[])
{  
   gdcm::File *f;
 
   //gdcm::Debug::DebugOn();

   std::cout << "------------------------------------------------" << std::endl;
   std::cout << "Gets the 'Overlays' from a full gdcm-readable DCM image "
             << "uncompressed image" << std::endl;
   std::cout << "Writes them in a DicomV3 file named 'gdcmOverlay.dcm'"
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

   f->SetLoadMode(gdcm::LD_ALL);
   f->SetFileName( fileName );
   f->AddForceLoadElement(0x6000,0x3000);  // Overlay Data
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
   
   gdcm::DataEntry *e = f->GetDataEntry(0x6000, 0x3000);
   
   if (e == 0)
   {
      std::cout << " Image doesn't contain any Overlay " << std::endl;
      f->Delete();
      return 0;
   }
   
   std::cout << " File is read! " << std::endl;
   
// ============================================================
//   Get usefull info from FileHelper.
// ============================================================ 

   
   uint32_t overlayPixelLength = e->GetLength();
   
/*
   if ( overlayPixelLength != (size_t)nx*ny*sizeof( uint16_t) )
   {
       std::cout << "Sorry, Pixels of" << fileName << "  are not "
                 << "readable. expected length :" << nx*ny*sizeof( uint16_t) 
                 << "  " << "read length : " << overlayPixelLength
                 << std::endl;
       f->Delete();
       delete pixels;  
       return 0;
   }
   else
   {
      std::cout << "Pixels read as expected : length = " << lgt << std::endl;
   } 
*/
   uint32_t nx = f->GetXSize();
   uint32_t ny = f->GetYSize();

   uint8_t *overlayPixelArea = (uint8_t *)(e->GetBinArea());
   
   // should check overlayPixelLength == nx*ny/8 !)
   
   uint8_t *overlayPixelImage = new uint8_t[nx*ny];// uint8 is enough to hold 1 bit !
   
   uint16_t m;
   for (unsigned int j=0; j<overlayPixelLength; j++)
   {
      if (overlayPixelArea[j] != 0)
      { 
         std::cout << "j : " << std::dec << j << " Ox(" << std::hex <<(int)overlayPixelArea[j]
           << ")" << std::endl; 
         for (unsigned int k=0; k<8; k++)
         {
            m = overlayPixelArea[j]<<k;
            //printf("m : %04x ", m);
            m = m & 0x00ff;
            //printf(" : %04x \n", m);
            overlayPixelImage[j*8 +k] =  ( m > 127)?255:0;
    
            printf("%04x ", overlayPixelImage[j*8 +k]);
         }
         std::cout << std::endl;
      }
   }


   if( gdcm::Debug::GetDebugFlag() )
         std::cout << "About to built empty file"  << std::endl;

   gdcm::File *fileToBuild = gdcm::File::New();

   if( gdcm::Debug::GetDebugFlag() )
         std::cout << "Finish to built empty file"  << std::endl;

   std::ostringstream str;
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

   gdcm::FileHelper *fh = gdcm::FileHelper::New(fileToBuild);

   if( gdcm::Debug::GetDebugFlag() )
         std::cout << "-------------Finish to built FileHelper"  << std::endl;

      fh->SetImageData(overlayPixelImage,nx*ny);
      fh->SetWriteTypeToDcmExplVR();

      str.str("");
      str<<"gdcmOverlay" << ".dcm";
      //   Write the current 'overlay' file

      if( !fh->Write(str.str()) )
      {
         std::cout << "Failed\n"
                   << "File in unwrittable\n";
         fh->Delete();
         if (fileToBuild)
            fileToBuild->Delete();
         delete overlayPixelImage;
         
         return 0;
      }
      else
      {
         std::cout << "File written successfully" << std::endl;
      }
  
   if (f)
      fh->Delete();
   if (fileToBuild)
      fileToBuild->Delete();
   f->Delete();
   delete overlayPixelImage;
   

   return 0;
}

