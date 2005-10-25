/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestChangeHeader.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/25 14:52:27 $
  Version:   $Revision: 1.18 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"

// This examples read two images (could be the same). Try to modify
// Acquisition Matrix and then write the image again

int main(int argc, char *argv[])
{
   if (argc < 3)
   {
      std::cerr << "usage :" << std::endl <<
      argv[0] << " fileNameForHeader fileNameForData" << 
      std::endl;
      return 1;
   }

   gdcm::File *h1 = gdcm::File::New( );
   h1->SetFileName ( argv[1] );
   h1->Load( );
   gdcm::FileHelper  *f1 = gdcm::FileHelper::New( h1 );
   
   gdcm::File *h2 = gdcm::File::New( );
   h2->SetFileName ( argv[2] );
   h2->Load( );
   gdcm::FileHelper  *f2 = gdcm::FileHelper::New( h2 );   
   

   // 0018 1310 US ACQ Acquisition Matrix
   gdcm::DictEntry *dictEntry =
                        f2->GetFile()->GetPubDict()->GetEntry( 0x0018, 1310 );
   std::cerr << std::hex << dictEntry->GetGroup() << "," 
            << dictEntry->GetElement() << std::endl;

   std::string matrix = f2->GetFile()->GetEntryString(0x0018, 0x1310);
   if(matrix != "gdcm::Unfound")
   {
      std::cerr << "Aquisition Matrix:" << matrix << std::endl;
      f1->GetFile()->InsertEntryString( matrix, 0x0018, 0x1310);
   }

   f1->GetImageData();
   
   h1->Print();
   
   f1->WriteDcmExplVR("output-matrix.dcm");
   
   f1->Delete();
   f2->Delete();
   h1->Delete();
   h2->Delete();

   return 0;
}
