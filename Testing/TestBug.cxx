/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestBug.cxx,v $
  Language:  C++
  Date:      $Date: 2004/12/03 20:16:56 $
  Version:   $Revision: 1.16 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
// The following crashes on Win32
// We have to be carefull when the code both use cout + printf VC++ gets
// confused, thus make sure we use only one kind of iostream

#include "gdcmHeader.h"

int TestBug(int argc, char* argv[])
{  
  gdcm::Header* e1;

   if (argc > 1)
      e1 = new gdcm::Header( argv[1] );
   else {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/test.acr";
      e1 = new gdcm::Header( filename.c_str() );
   }
   //e1->PrintPubDict();
   //e1->GetPubDict()->GetEntriesByKey();
   e1->GetPubDict()->PrintByKey();
   delete e1;

   return 0;
}
