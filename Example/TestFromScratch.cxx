/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestFromScratch.cxx,v $
  Language:  C++
  Date:      $Date: 2004/12/10 15:50:04 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmHeader.h"
#include "gdcmFile.h"
#include "gdcmDictEntry.h"
#include "gdcmDocEntry.h"
#include "gdcmValEntry.h"
#include "gdcmDebug.h"

// The whole purpose of this example is to create a DICOM from scratch
// well almost the only thing we allow use is to copy the string entry
// From the previous image read.

int main(int argc, char *argv[])
{
   if( argc < 2 )
   {
      std::cerr << "Usage: " << argv[0] << " Image.dcm" << std::endl;
      return 1;
   }

   // Doesn't seems to do anything:
   dbg.SetDebug(-1);
   // Doesn't link properly:
   //gdcm::Debug::GetReference().SetDebug(1);

   std::string filename = argv[1];
   //gdcm::File *f1 = new gdcm::File( "/home/malaterre/Creatis/gdcmData/012345.002.050.dcm" );
   gdcm::File *f1 = new gdcm::File( filename );
   gdcm::Header *h1 = f1->GetHeader();

   int dataSize = f1->GetImageDataSize();
   std::cout << "DataSize:      " << dataSize << std::endl;
   // Since we know the image is 16bits:
   uint8_t* imageData = f1->GetImageData();
 
   // Hopefully default to something
   gdcm::Header *h2 = new gdcm::Header();

   const gdcm::TagDocEntryHT &nameHt = h1->GetTagHT();
   //const gdcm::TagDocEntryHT &nameHt = h2->GetTagHT(); seems to be empty ??
   for (gdcm::TagDocEntryHT::const_iterator tag = nameHt.begin(); tag != nameHt.end(); ++tag)
   {
      //Copy string only:
      if (tag->second->GetVR().find("SQ") == 0)
      {
         // skip DICOM SeQuence, otherwise following cast will crash
         continue;
      }
      const std::string &name = tag->second->GetName();
      const std::string &value = ((gdcm::ValEntry*)(tag->second))->GetValue();
      if( name != "unkn"
       && value.find( "gdcm::NotLoaded" ) != 0
       && value.find( "gdcm::Binary" )    != 0
       && value.find( "gdcm::Loaded" )    != 0 )
      {
        //std::cout << name << "," << value << std::endl;
        gdcm::DictEntry *dictEntry = h2->GetPubDict()->GetDictEntryByName(name);
        h2->ReplaceOrCreateByNumber( value, dictEntry->GetGroup(), dictEntry->GetElement());
      }
   }

   h2->ReplaceOrCreateByNumber( imageData, dataSize, 0x7fe0, 0x0010, "PXL" );
   h2->Print( std::cout );

   gdcm::File *f2 = new gdcm::File( h2 );
 
   // SetImageData does not work because you need to have called
   // somwhow ReplaceOrCreateByNumber before with the proper group/element
   f2->SetImageData(imageData, dataSize);

   f2->WriteDcmExplVR( "output.dcm" );

   delete f1;
   delete f2;
   delete h2;

   return 0;
}
