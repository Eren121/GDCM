/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: makeDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2004/11/16 04:26:18 $
  Version:   $Revision: 1.8 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include <iostream>
#include "gdcm.h"
#include "gdcmDocEntry.h"
#include "gdcmDicomDir.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDirList.h"

// ---
void StartMethod(void *toto) {
  (void)toto;
   std::cout<<"Start parsing"<<std::endl;
}

void EndMethod(void *toto) {
  (void)toto;
   std::cout<<"End parsing"<<std::endl;
}
// ---

/**
  * \ingroup Test
  * \brief   Explores recursively the given directory (or GDCM_DATA_ROOT by default)
  *          orders the gdcm-readable found Files
  *          according their Patient/Study/Serie/Image characteristics
  *          makes the gdcmDicomDir 
  *          and writes a file named NewDICOMDIR..
  */  

int main(int argc, char* argv[]) {
  gdcm::DicomDir *dcmdir;
   std::string dirName;   

   if (argc > 1)
      dirName = argv[1];
   else
      dirName = GDCM_DATA_ROOT;

   dcmdir = new gdcm::DicomDir(dirName, true); // we ask for Directory parsing

   dcmdir->SetStartMethod(StartMethod, (void *) NULL);
   dcmdir->SetEndMethod(EndMethod);
   
   gdcm::ListDicomDirPatient lp = dcmdir->GetDicomDirPatients();
   if (! lp.size() ) 
   {
      std::cout << "makeDicomDir: no patient list present. Exiting."
                << std::endl;
      return 1;
   }
    
   dcmdir->WriteDicomDir("NewDICOMDIR");
   std::cout<<std::flush;

   delete dcmdir;
   return 0;
}
