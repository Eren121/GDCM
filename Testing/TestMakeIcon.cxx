/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestMakeIcon.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/11 13:12:05 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmDebug.h"
#include "gdcmGlobal.h"
#include "gdcmCommon.h"
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"

// 0088 0200 SQ 1 Icon Image Sequence 

int TestMakeIcon (int argc, char *argv[])
{
   // hard coded small image name
   std::string input = "LIBIDO-8-ACR_NEMA-Lena_128_128.acr";
   std::string output = "test.dcm";

   if ( argc == 3 )
   {
      input  = argv[1];
      output = argv[2];
   }
   else if ( argc < 3  )
   {
      std::cout << "   Usage: " << argv[0]
                << " input filename.dcm output Filename.dcm" << std::endl;
   }

   gdcm::File *f1 = new gdcm::File(input);

   if (f1 == 0)
   {
      std::cout << " failed to open file" << std::endl;
      return 1;
   }  
   gdcm::FileHelper *fh1 = new gdcm::FileHelper(f1); 
   uint8_t *pixels = fh1->GetImageData();
   uint32_t lgth = fh1->GetImageDataSize();

   gdcm::SeqEntry *icon = f1->InsertSeqEntry(0x0088, 0x0200);
   gdcm::SQItem *sqi = new gdcm::SQItem(1);
   icon->AddSQItem(sqi, 1);

   // icone is just define like the image
   // The purpose is NOT to imagine an icon, 
   // just check the stuff works
   uint16_t binVal[3]={0x52f7,0xf358,0xad9b};
 
   sqi->InsertValEntry( "MONOCHROME2", 0x0028,0x0004);
   sqi->InsertValEntry( "128", 0x0028,0x0010);
   sqi->InsertValEntry( "8",   0x0028,0x0100);
   sqi->InsertValEntry( "8",   0x0028,0x0101);
   sqi->InsertValEntry( "7",   0x0028,0x0102);
   sqi->InsertValEntry( "0",   0x0028,0x0103);
   sqi->InsertBinEntry(  (uint8_t *)binVal, 3*2, 0x0005,0x0010,"OW");
   sqi->InsertBinEntry(  pixels, lgth, 0x7fe0,0x0010);
   // just to see if it's stored a the right place
   sqi->InsertValEntry( "128", 0x0028,0x0011);
    
   fh1->WriteDcmExplVR(output);

   delete f1;

   f1 = new gdcm::File(output);
   f1->Print();

   icon = f1->GetSeqEntry(0x0088, 0x0200);
   sqi = icon->GetFirstSQItem();

   if ( !sqi )
   {
      std::cout << "Sequence 0088|0200 not found" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }

   // Test for entry 0028|0010
   if ( !sqi->GetValEntry(0x0028,0x0010) )
   {
      std::cout << "ValEntry 0028|0010 not found" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }
   if ( sqi->GetValEntry(0x0028,0x0010)->GetValue() != "128" )
   {
      std::cout << "Value 0028|0010 don't match" << std::endl
                << "Read : " << sqi->GetValEntry(0x0028,0x0010)->GetValue()
                << " - Expected : 128" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }

   // Test for entry 0028|0011
   if ( !sqi->GetValEntry(0x0028,0x0011) )
   {
      std::cout << "ValEntry 0028|0011 not found" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }
   if ( sqi->GetValEntry(0x0028,0x0011)->GetValue() != "128" )
   {
      std::cout << "Value 0028|0011 don't match" << std::endl
                << "Read : " << sqi->GetValEntry(0x0028,0x0011)->GetValue()
                << " - Expected : 128" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }

   // Test for entry 0028|0100
   if ( !sqi->GetValEntry(0x0028,0x0100) )
   {
      std::cout << "ValEntry 0028|0100 not found" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }
   if ( sqi->GetValEntry(0x0028,0x0100)->GetValue() != "8" )
   {
      std::cout << "Value 0028|0100 don't match" << std::endl
                << "Read : " << sqi->GetValEntry(0x0028,0x0100)->GetValue()
                << " - Expected : 8" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }

   // Test for entry 0028|0101
   if ( !sqi->GetValEntry(0x0028,0x0101) )
   {
      std::cout << "ValEntry 0028|0101 not found" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }
   if ( sqi->GetValEntry(0x0028,0x0101)->GetValue() != "8" )
   {
      std::cout << "Value 0028|0101 don't match" << std::endl
                << "Read : " << sqi->GetValEntry(0x0028,0x0101)->GetValue()
                << " - Expected : 8" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }

   // Test for entry 0028|0102
   if ( !sqi->GetValEntry(0x0028,0x0102) )
   {
      std::cout << "ValEntry 0028|0102 not found" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }
   if ( sqi->GetValEntry(0x0028,0x0102)->GetValue() != "7" )
   {
      std::cout << "Value 0028|0102 don't match" << std::endl
                << "Read : " << sqi->GetValEntry(0x0028,0x0102)->GetValue()
                << " - Expected : 7" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }

   // Test for entry 0028|0103
   if ( !sqi->GetValEntry(0x0028,0x0103) )
   {
      std::cout << "ValEntry 0028|0010 not found" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }
   if ( sqi->GetValEntry(0x0028,0x0103)->GetValue() != "0" )
   {
      std::cout << "Value 0028|0103 don't match" << std::endl
                << "Read : " << sqi->GetValEntry(0x0028,0x0103)->GetValue()
                << " - Expected : 0" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }

   // Test for entry 0005|0010
   if ( !sqi->GetValEntry(0x0028,0x0010) )
   {
      std::cout << "BinEntry 0005|0010 not found" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }
   if( sqi->GetBinEntry(0x0005,0x0010)->GetLength() != 6 )
   {
      std::cout << "BinEntry size 0005|0010 don't match" << std::endl
                << "Read : " << sqi->GetValEntry(0x0005,0x0010)->GetLength()
                << " - Expected : 6" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }

   if( memcmp(sqi->GetBinEntry(0x0005,0x0010)->GetBinArea(),binVal,6)!=0 )
   {
      std::cout << "Value 0005|0010 don't match (BinEntry)" << std::endl
                << "   ... Failed" << std::endl;
      delete fh1;
      delete f1;
      return 1;
   }

   delete fh1;
   delete f1;
   std::cout << "   ... OK" << std::endl;

   return 0;
}
