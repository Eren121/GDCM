/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestChangeHeader.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/21 11:40:52 $
  Version:   $Revision: 1.8 $
                                                                                
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

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
       std::cerr << "usage :" << std::endl <<
         argv[0] << " nomFichierPourEntete nomFichierPourDonnées" << 
         std::endl;
       return 1;
    }

    gdcm::File *h1 = new gdcm::File( argv[1] );
    gdcm::FileHelper  *f1 = new gdcm::FileHelper( h1 );
    gdcm::FileHelper  *f2 = new gdcm::FileHelper( argv[2] );

    // 0018 1310 US ACQ Acquisition Matrix
    gdcm::DictEntry *dictEntry =
       f2->GetFile()->GetPubDict()->GetDictEntry( 0x0018, 1310 );
    std::cerr << std::hex << dictEntry->GetGroup() << "," << dictEntry->GetElement() << std::endl;

    std::string matrix = f2->GetFile()->GetEntry(0x0018, 0x1310);
    if(matrix != "gdcm::Unfound")
    {
       std::cerr << "Aquisition Matrix:" << matrix << std::endl;
       f1->GetFile()->ReplaceOrCreate( matrix, 0x0018, 0x1310);
    }

    f1->GetImageData();
    
    h1->Print();
    
    f1->WriteDcmExplVR("output-matrix.dcm");

    return 0;
}


