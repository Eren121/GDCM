/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: WriteDicom.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/24 16:10:49 $
  Version:   $Revision: 1.13 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"

// Writting of a DICOM file based on a correct dicom header
// and data pixel of another image

int main(int argc, char* argv[])
{
 
   if (argc < 3) 
     {
     std::cerr << "Usage :" << std::endl << argv[0] << 
       " HeaderFileName DataFileName" << std::endl;
     return 0;
     }
 
   const char *first = argv[1];
   gdcm::FileHelper *f1 = new gdcm::FileHelper( first );
 
   const char *second = argv[2];
   gdcm::FileHelper *f2 = new gdcm::FileHelper( second );
 
   // We assume that DICOM fields of second file actually exists :
 
   std::string nbFrames = f2->GetFile()->GetEntryValue(0x0028, 0x0008);
   if(nbFrames != "gdcm::Unfound") {
       f1->GetFile()->ReplaceOrCreate( nbFrames, 0x0028, 0x0008);
   }
          
   f1->GetFile()->ReplaceOrCreate(
     f2->GetFile()->GetEntryValue(0x0028, 0x0010), 0x0028, 0x0010); // nbLig
   f1->GetFile()->ReplaceOrCreate( 
     f2->GetFile()->GetEntryValue(0x0028, 0x0011), 0x0028, 0x0011); // nbCol
 
   // Some other tags should be updated:
 
   // TODO : add a default value
   // TODO : a function which take as input a list of tuple (gr, el)
   //        and that does the job
 
   int dataSize    = f2->GetImageDataSize();
   uint8_t* imageData = f2->GetImageData();
 
   std::cout << "dataSize :" << dataSize << std::endl;
 
   // TODO : Shouldn't we merge those two functions ?
   f1->SetImageData( imageData, dataSize);
 
   f1->GetFile()->Print();
 
   std::string s0  = f2->GetFile()->GetEntryValue(0x7fe0, 0x0000);
   std::string s10 = f2->GetFile()->GetEntryValue(0x7fe0, 0x0010);
 
   std::cout << "lgr 7fe0, 0000 " << s0  << std::endl;
   std::cout << "lgr 7fe0, 0010 " << s10 << std::endl;
 
   std::cout << "WriteDCM" << std::endl;
 
   f1->WriteDcmExplVR("WriteDicom.dcm");
 
   return 0;
}
