#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmCommon.h"
#include "gdcmDebug.h"
#include "gdcmDataEntry.h"

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
   " \n exOverlaysDCM :\n                                                     ",
   " Extract an overlay image from a DICOM image                              ",
   "          Warning : probably segfaults if no overlay                      ",
   " usage: exOverlaysDCM filein=inputFileName fileout=outputFileName [debug] ",
   "        debug    : developper wants to run the program in 'debug mode'    ",
   FINISH_USAGE

   // ----- Initialize Arguments Manager ------
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);

   if (argc == 1 || am->ArgMgrDefined("usage"))
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }
   
/*
   char *fileName = am->ArgMgrWantString("filein",usage);
   if ( fileName == NULL )
   {
      delete am;
      return 0;
   }
*/


   const char *fileName = am->ArgMgrGetString("filein");
   const char *dirName  = am->ArgMgrGetString("dirin");

   if ( (fileName == 0 && dirName == 0) ||
        (fileName != 0 && dirName != 0) )
   {
      std::cerr << std::endl
        << "Either 'filein=' or 'dirin=' must be present;" 
        << std::endl << "Not both" << std::endl;
      am->ArgMgrUsage(usage); // Display 'usage'  
      delete am;
      return 1;
   }


   char *outputFileName = am->ArgMgrWantString("fileout",usage);
   if ( outputFileName == NULL )
   {
      delete am;
      return 0;
   }
   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();
      
   if (am->ArgMgrDefined("warning"))
      gdcm::Debug::WarningOn();
      
   // if unused Param we give up
   if ( am->ArgMgrPrintUnusedLabels() )
   {
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }

   delete am;  // we don't need Argument Manager any longer

   // ============================================================
   //   Read the input file.
   // ============================================================

   gdcm::File *f;

   f = gdcm::File::New(  );
   f->SetLoadMode( gdcm::LD_ALL );
   f->SetFileName( fileName );
   f->AddForceLoadElement(0x6000,0x3000);  // Overlay Data
   int res = f->Load();

   if ( !res )
   {
       std::cerr << "Sorry, " << fileName <<"  not a gdcm-readable "
                 << "DICOM / ACR File" <<std::endl;
       f->Delete();
       return 0;
   }
   std::cout << " ... is readable " << std::endl;

   // ============================================================
   //   Load the Overlays in memory (the first one)
   // ============================================================

   gdcm::DataEntry *e = f->GetDataEntry(0x6000, 0x3000);  
   if (e == 0)
   {
      std::cout << " Image doesn't contain any Overlay " << std::endl;
      f->Delete();
      return 0;
   }   
   std::cout << " File is read! " << std::endl;

   uint8_t *overlay = (uint8_t *)(e->GetBinArea());
   if ( overlay == 0 )
   {
       std::cerr << "Sorry, Overlays of" << fileName <<"  are not "
                 << " gdcm-readable."    << std::endl;
       f->Delete();
       return 0;
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
   gdcm::File *f2;
   f2 = gdcm::File::New(  );
   
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


   // feel free to add any field (Dicom Data Entry) you like, here.
   // ...
   gdcm::FileHelper *fh = gdcm::FileHelper::New(f2);
       
   fh->SetImageData(outputData,dimXY);
   fh->WriteDcmExplVR(outputFileName);
   std::cout <<"End WriteOverlayImage" << std::endl;

   delete outputData;
   f->Delete();
   f2->Delete();  
   fh->Delete();
   
   return 0;
}

