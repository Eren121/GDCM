/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestMakeIcon.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/11 12:14:26 $
  Version:   $Revision: 1.1 $
                                                                                
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

// 0088 0200 SQ 1 Icon Image Sequence 

int TestMakeIcon (int , char )
{

   //ard coded small image name
   std::string input = "LIBIDO-8-ACR_NEMA-Lena_128_128.acr";
   std::string output = "test.dcm";
/*
   if ( argc == 3 )
   {
      input  = argv[1];
      output = argv[2];
   }
   else if ( argc < 3  )
   {
      std::cout << "   Usage: " << argv[0]
                << " input filename.dcm output Filename.dcm" << std::endl;
      return 1;
   }
*/

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
 
   sqi->InsertValEntry( "MONOCHROME2", 0x0028,0x0004);
   sqi->InsertValEntry( "128", 0x0028,0x0010);
   sqi->InsertValEntry( "8", 0x0028,0x0100);
   sqi->InsertValEntry( "8", 0x0028,0x0101);
   sqi->InsertValEntry( "7", 0x0028,0x0102);
   sqi->InsertValEntry( "0", 0x0028,0x0103);
   sqi->InsertBinEntry(  pixels, lgth, 0x7fe0,0x0010);
   // just to see if it's stored a the right place
   sqi->InsertValEntry( "128", 0x0028,0x0011);
    
   fh1->WriteDcmExplVR(output);

   delete f1;

   f1 = new gdcm::File(output);
   f1->Print();

   icon = f1->GetSeqEntry(0x0088, 0x0200);
   sqi = icon->GetFirstSQItem();

   if ( (sqi->GetValEntry(0x0028,0x0010))->GetValue() != "128" )
      return 1;

   if ( (sqi->GetValEntry(0x0028,0x0011))->GetValue() != "128" )
      return 1;

   if ( (sqi->GetValEntry(0x0028,0x0100))->GetValue() != "8" )
      return 1;

   if ( (sqi->GetValEntry(0x0028,0x0101))->GetValue() != "8" )
      return 1;

   if ( (sqi->GetValEntry(0x0028,0x0102))->GetValue() != "7" )
      return 1;

   if ( (sqi->GetValEntry(0x0028,0x0103))->GetValue() != "0" )
      return 1;

   delete fh1;
   delete F1;

   return 0;
}
