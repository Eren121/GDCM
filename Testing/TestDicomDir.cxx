/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/20 11:09:22 $
  Version:   $Revision: 1.31 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmDocEntry.h"
#include "gdcmDicomDir.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirImage.h"
#include "gdcmTS.h"

#include <iostream>
#include <fstream>

int TestDicomDir(int argc, char* argv[])
{  
   gdcm::DicomDir *e1;
   
   gdcm::DicomDirPatient *pa;
   gdcm::DicomDirStudy *st;
   gdcm::DicomDirSerie *se;
   gdcm::DicomDirImage *im;

   gdcm::TSKey v;
    
   std::string file; 
   if (argc > 1) 
      file = argv[1];    
   else 
   {
      file += GDCM_DATA_ROOT;
      file += "/DICOMDIR";
   }

   e1 = new gdcm::DicomDir(file);
   if (argc > 2) 
   {
      int level = atoi(argv[2]);   
      e1->SetPrintLevel(level);
   }

   // Test if the DicomDir is readable
   if( !e1->IsReadable() )
   {
      std::cout<<"          DicomDir '"<<file
               <<"' is not readable"<<std::endl
               <<"          ...Failed"<<std::endl;

      delete e1;
      return 1;
   }
   else
   {
      std::cout<<"          DicomDir '"<<file
               <<"' is readable"<<std::endl;
   }

   // Test if the DicomDir contains any Patient
   if( !e1->GetFirstPatient() )
   {
      std::cout<<"          DicomDir '"<<file
               <<" has no patient"<<std::endl
               <<"          ...Failed"<<std::endl;

      delete e1;
      return 1;
   }

   // step by step structure full exploitation
   std::cout << std::endl << std::endl  
             << " = PATIENT/STUDY/SERIE/IMAGE List ============================" 
             << std::endl<< std::endl;
  
   pa = e1->GetFirstPatient(); 
   while ( pa ) 
   {  // we process all the PATIENT of this DICOMDIR 
      std::cout << pa->GetEntry(0x0010, 0x0010) << std::endl; // Patient's Name

      st = pa->GetFirstStudy();
      while ( st ) 
      { // we process all the STUDY of this patient
         std::cout << "--- "<< st->GetEntry(0x0008, 0x1030) << std::endl;    // Study Description
         std::cout << " Stud.ID:["          << st->GetEntry(0x0020, 0x0010); // Study ID

         se = st->GetFirstSerie();
         while ( se ) 
         { // we process all the SERIES of this study
            std::cout << "--- --- "<< se->GetEntry(0x0008, 0x103e) << std::endl;      // Serie Description
            std::cout << " Ser.nb:["         <<  se->GetEntry(0x0020, 0x0011);        // Series number
            std::cout << "] Mod.:["          <<  se->GetEntry(0x0008, 0x0060) << "]"; // Modality

            im = se->GetFirstImage();
            while ( im ) { // we process all the IMAGE of this serie
               std::cout << "--- --- --- "<< im->GetEntry(0x0004, 0x1500) << std::endl; // File name
               im = se->GetNextImage();   
            }
            se = st->GetNextSerie();   
         }
         st = pa->GetNextStudy();
      }  
      pa = e1->GetNextPatient();
   }  

   std::cout << std::endl << std::endl  
             << " = DICOMDIR full content ====================================" 
             << std::endl<< std::endl;
   e1->Print();
   
   std::cout<<std::flush;
   delete e1;

   return 0;
}
