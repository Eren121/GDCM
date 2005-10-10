/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exExtractTag.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/10 21:57:57 $
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
#include "gdcmBinEntry.h"

#include <iomanip>

int main(int argc, char *argv[])
{  
   gdcm::File *f;
 
   if( argc < 5 )
     {
     std::cerr << "Usage :" << argv[0] << " input.dcm  group element outputfile" << std::endl;
     std::cerr << "  Ex: " << argv[0] << " /tmp/bla.dcm 0029 2110 /tmp/out.raw" << std::endl;
     return 1;
     }
   std::string fileName = argv[1];

   std::cout << fileName << std::endl;
// ============================================================
//   Read the input image.
// ============================================================

   f = new gdcm::File( );

   //f->SetLoadMode(gdcm::LD_NOSEQ | gdcm::LD_NOSHADOW);
   f->SetFileName( fileName );
   bool res = f->Load();  

   if( gdcm::Debug::GetDebugFlag() )
   {
      std::cout << "---------------------------------------------" << std::endl;
      f->Print();
      std::cout << "---------------------------------------------" << std::endl;
   }
   if (!res) {
       std::cerr << "Sorry, " << fileName << " not a gdcm-readable "
           << "DICOM / ACR File"
           << std::endl;
      delete f;
      return 1;
   }
   std::cout << " ... is readable " << std::endl;

   // Find the dicom tag, and extract the string
   uint16_t group, elem;
   std::istringstream convert;
   convert.str( argv[2] );
   convert >> std::hex >> group;
   convert.clear(); //important
   convert.str( argv[3] );
   convert >> std::hex >> elem;
   std::cout << "Extracting tag: (0x" << std::hex << std::setw(4) << std::setfill('0')
     << group << ",0x" << std::setw(4) << std::setfill('0') << elem << ")" << std::endl;
   std::string dicom_tag_value = f->GetEntryValue(group, elem);
   if (dicom_tag_value == gdcm::GDCM_UNFOUND)
   {
     gdcm::DictEntry *dictEntry = f->GetPubDict()->GetEntry( group, elem);
     std::cerr << "Image doesn't contain any tag: " << dictEntry->GetName() << std::endl;
     delete f;
     return 1;
   }

   gdcm::DocEntry *dicom_tag_doc = f->GetDocEntry(group, elem);
   gdcm::BinEntry *dicom_tag = dynamic_cast<gdcm::BinEntry*>(dicom_tag_doc);
   if( !dicom_tag )
     {
     std::cerr << "Sorry BinEntry only please" << std::endl; //TODO support ValEntry
     delete f;
     return 1;
     }

   // Write out the data as a file:
   std::ofstream o(argv[4]);
   if( !o )
     {
     std::cerr << "Problem opening file: " << argv[4] << std::endl;
     delete f;
     return 1;
     }
   o.write((char*)dicom_tag->GetBinArea(), dicom_tag->GetLength());
   o.close();

   delete f;
   return 0;
}


