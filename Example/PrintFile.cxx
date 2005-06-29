/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PrintFile.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/29 16:00:13 $
  Version:   $Revision: 1.42 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmBinEntry.h"

#include "gdcmFileHelper.h"
#include "gdcmDebug.h"

#include "gdcmArgMgr.h"

#include <iostream>


void ShowLutData(gdcm::File *e1);

void ShowLutData(gdcm::File *e1)
{
     // Nothing is written yet to get LUT Data user friendly
     // The following is to be moved into a PixelReadConvert method
     // Let here, waiting for a clever idea on the way to do it.
  
      gdcm::SeqEntry *modLutSeq = e1->GetSeqEntry(0x0028,0x3000);
      if ( modLutSeq !=0 )
      {
         gdcm::SQItem *sqi= modLutSeq->GetFirstSQItem();
         if ( sqi != 0 )
         {
            std::string lutDescriptor = sqi->GetEntryValue(0x0028,0x3002);
           if (   /*lutDescriptor   == GDCM_UNFOUND*/ 0 )
           {
              //gdcmWarningMacro( "LUT Descriptor is missing" );
              std::cout << "LUT Descriptor is missing" << std::endl;
              return;
            }
            int length;   // LUT length in Bytes
            int deb;      // Subscript of the first Lut Value
            int nbits;    // Lut item size (in Bits)

            int nbRead;    // nb of items in LUT descriptor (must be = 3)

            nbRead = sscanf( lutDescriptor.c_str(),
                              "%d\\%d\\%d",
                               &length, &deb, &nbits );
           std::cout << "length " << length 
                     << " deb " << deb 
                     << " nbits " << nbits
                     << std::endl;
            if ( nbRead != 3 )
            {
                //gdcmWarningMacro( "Wrong LUT descriptor" );
                std::cout << "Wrong LUT descriptor" << std::endl;
            }
            //LUT Data (CTX dependent)    
            gdcm::BinEntry *b = sqi->GetBinEntry(0x0028,0x3006); 
            if ( b != 0 )
            { 
               int BitsAllocated = e1->GetBitsAllocated();
               if ( BitsAllocated <= 8 )
               { 
                  int mult;
                  if ( ( nbits == 16 ) && ( BitsAllocated == 8 ) )
                  {
                  // when LUT item size is different than pixel size
                     mult = 2; // high byte must be = low byte
                  }
                  else
                  {
                  // See PS 3.3-2003 C.11.1.1.2 p 619
                     mult = 1;
                  }
                  uint8_t *lut = b->GetBinArea();
                  for( int i=0; i < length; ++i )
                  {
                     std::cout << i+deb << " : \t"
                               << (int) (lut[i*mult + 1]) << std::endl;
                  }
               }
               else
               {
                  uint16_t *lut = (uint16_t *)(b->GetBinArea());  
                  for( int i=0; i < length; ++i )
                  {
                     std::cout << i+deb << " : \t"
                               << (int) (((uint16_t *)lut)[i])
                               << std::endl;
                  }             
               }
            }  
            else
               std::cout << "No LUT Data BinEntry (0x0028,0x3006) found?!? " 
                         << std::endl;
         }
         else
            std::cout << "No First SQ Item within (0x0028,0x3000) ?!? " 
                      << std::endl;      
      }
      else
         std::cout << "No LUT Data SeqEntry (0x0028,0x3000) found " 
                   << std::endl;

   }

int main(int argc, char *argv[])
{

   START_USAGE(usage)
   " \n PrintFile : \n",
   " Display the header of a ACR-NEMA/PAPYRUS/DICOM File",
   " usage: PrintFile filein=fileName [level=n] [noshadow] [noseq] [debug] ",
   "        level = 0,1,2 : depending on the amount of details user wants to see",
   "        noshadow : user doesn't want to load Private groups (odd number)",
   "        noseq    : user doesn't want to load Sequences ",
   "        debug    : user wants to run the program in 'debug mode' ",
   "        showlut  : user wants to display the Palette Color (as an int array)",
   FINISH_USAGE

   // Initialize Arguments Manager   
   gdcm::ArgMgr *am= new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1)
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }

   char *fileName = am->ArgMgrWantString("filein",usage);

   int loadMode;
   if ( am->ArgMgrDefined("noshadowseq") )
      loadMode = NO_SHADOWSEQ;
   else if ( am->ArgMgrDefined("noshadow") && am->ArgMgrDefined("noseq") )
      loadMode = NO_SEQ | NO_SHADOW;  
   else if ( am->ArgMgrDefined("noshadow") )
      loadMode = NO_SHADOW;
   else if ( am->ArgMgrDefined("noseq") )
      loadMode = NO_SEQ;
   else
      loadMode = 0;

   int level = am->ArgMgrGetInt("level", 2);

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();

   bool showlut = ( 0 != am->ArgMgrDefined("SHOWLUT") );
 
   /* if unused Param we give up */
   if ( am->ArgMgrPrintUnusedLabels() )
   { 
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   } 

   delete am;  // we don't need Argument Manager any longer

   // ----------- End Arguments Manager ---------
 
   // gdcm::File::IsReadable() is no usable here, because we deal with
   // any kind of gdcm-Parsable *document* 
   // not only gdcm::File (as opposed to gdcm::DicomDir)

   gdcm::File *e1 = new gdcm::File();
   e1->SetLoadMode(loadMode);

   bool res = e1->Load( fileName );
   if ( !res )
   {
      delete e1;
      return 0;
   }

   gdcm::FileHelper *f1 = new gdcm::FileHelper(e1);
   f1->SetPrintLevel( level );

   f1->Print();   

   std::cout << "\n\n" << std::endl; 

   std::cout <<std::endl;
   std::cout <<" dataSize    " << f1->GetImageDataSize()    << std::endl;
   std::cout <<" dataSizeRaw " << f1->GetImageDataRawSize() << std::endl;

   int nX,nY,nZ,sPP,planarConfig;
   std::string pixelType;
   nX=e1->GetXSize();
   nY=e1->GetYSize();
   nZ=e1->GetZSize();
   std::cout << " DIMX=" << nX << " DIMY=" << nY << " DIMZ=" << nZ << std::endl;

   pixelType    = e1->GetPixelType();
   sPP          = e1->GetSamplesPerPixel();
   planarConfig = e1->GetPlanarConfiguration();
   
   std::cout << " pixelType= ["            << pixelType 
             << "] SamplesPerPixel= ["     << sPP
             << "] PlanarConfiguration= [" << planarConfig 
             << "] "<< std::endl 
             << " PhotometricInterpretation= [" 
                                << e1->GetEntryValue(0x0028,0x0004)
             << "] "<< std::endl;

   int numberOfScalarComponents=e1->GetNumberOfScalarComponents();
   std::cout << " NumberOfScalarComponents = " << numberOfScalarComponents 
             <<std::endl
             << " LUT = " << (e1->HasLUT() ? "TRUE" : "FALSE")
             << std::endl;

   if ( e1->GetEntryValue(0x0002,0x0010) == gdcm::GDCM_NOTLOADED ) 
   {
      std::cout << "Transfer Syntax not loaded. " << std::endl
                << "Better you increase MAX_SIZE_LOAD_ELEMENT_VALUE"
                << std::endl;
      return 0;
   }
  
   std::string transferSyntaxName = e1->GetTransferSyntaxName();
   std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" 
             << std::endl;
   std::cout << " SwapCode= " << e1->GetSwapCode() << std::endl;
 
   // Display the LUT as an int array (for debugging purpose)
   if ( e1->HasLUT() && showlut )
   {
      uint8_t* lutrgba = f1->GetLutRGBA();
      if ( lutrgba == 0 )
      {
         std::cout << "Lut RGBA (Palette Color) not built " << std::endl;
 
        // Nothing is written yet to get LUT Data user friendly
        // The following is to be moved into a PixelRedaConvert method
  
         gdcm::SeqEntry *modLutSeq = e1->GetSeqEntry(0x0028,0x3000);
         if ( modLutSeq !=0 )
         {
            gdcm::SQItem *sqi= modLutSeq->GetFirstSQItem();
            if ( !sqi )
            {
               std::string lutDescriptor = sqi->GetEntryValue(0x0028,0x3002);
               int length;   // LUT length in Bytes
               int deb;      // Subscript of the first Lut Value
               int nbits;    // Lut item size (in Bits)
               int nbRead;    // nb of items in LUT descriptor (must be = 3)

               nbRead = sscanf( lutDescriptor.c_str(),
                                 "%d\\%d\\%d",
                                  &length, &deb, &nbits );
               if ( nbRead != 3 )
               {
                   //gdcmWarningMacro( "Wrong LUT descriptor" );
                   std::cout << "Wrong LUT descriptor" << std::endl;
               }                                                  
               gdcm::BinEntry *b = sqi->GetBinEntry(0x0028,0x3006);
               if ( b != 0 )
               {
                  if ( b->GetLength() != 0 )
                  {
                     std::cout << "---------------------------------------"
                               << " We should never reach this point      "
                               << std::endl;
                     //LoadEntryBinArea(b);    //LUT Data (CTX dependent)
                  }   
              }
           }      
         }
         else
             std::cout << "No LUT Data (0x0028,0x3000) found " << std::endl;
     }
      else
      {
         if ( f1->GetLutItemSize() == 8 )
         {
            for (int i=0;i<f1->GetLutItemNumber();i++)
               std::cout << i << " : \t"
                         << (int)(lutrgba[i*4])   << " "
                         << (int)(lutrgba[i*4+1]) << " "
                         << (int)(lutrgba[i*4+2]) << std::endl;
         }
         else // LutItemSize assumed to be = 16
         {
            uint16_t* lutrgba16 = (uint16_t*)lutrgba;
            for (int i=0;i<f1->GetLutItemNumber();i++)
               std::cout << i << " : \t"
                         << (int)(lutrgba16[i*4])   << " "
                         << (int)(lutrgba16[i*4+1]) << " "
                         << (int)(lutrgba16[i*4+2]) << std::endl;
         }
      }
   }
   else if (showlut)
   {
      std::cout << "Try LUT Data "<< std::endl;
      ShowLutData(e1);
   }
     
   if (e1->IsReadable())
      std::cout <<std::endl<<fileName<<" is Readable"<<std::endl;
   else
      std::cout <<std::endl<<fileName<<" is NOT Readable"<<std::endl;
   std::cout<<std::flush;
   delete e1;
   delete f1;
   return 0;   
}
