/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestBug.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/21 11:40:54 $
  Version:   $Revision: 1.18 $
                                                                                
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

#include "gdcmFile.h"

int TestBug(int argc, char* argv[])
{  
  gdcm::File* e1;

   if (argc > 1)
      e1 = new gdcm::File( argv[1] );
   else {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/test.acr";
      e1 = new gdcm::File( filename.c_str() );
   }
   //e1->PrintPubDict();
   //e1->GetPubDict()->GetEntriesByKey();
   e1->GetPubDict()->Print();
   delete e1;

   return 0;
}
