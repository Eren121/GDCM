/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestValidate.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/03 14:17:24 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmValidator.h"


int main(int argc, char *argv[])
{
  if( argc < 2 )
  {
    std::cerr << "ouh les cornes / shame on you" << std::endl;
    return 1;
  }

   const char *filename = argv[1];

   gdcm::File *input =  gdcm::File::New( );
   input->SetFileName(filename);
   input->Load();
   gdcm::Validator *v = new gdcm::Validator();
   v->SetInput( input );

   return 0;
}
