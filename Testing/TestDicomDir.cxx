/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/13 14:39:02 $
  Version:   $Revision: 1.26 $
                                                                                
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
   
   gdcm::ListDicomDirPatient::const_iterator  itPatient;
   gdcm::ListDicomDirStudy::const_iterator    itStudy;
   gdcm::ListDicomDirSerie::const_iterator    itSerie;
   //gdcm::ListDicomDirImage::const_iterator    itImage;
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

   // Test if the dicomDir is readable
   if( !e1->IsReadable() )
   {
      std::cout<<"          DicomDir '"<<file
               <<"' is not readable"<<std::endl
               <<"          ...Failed"<<std::endl;

      delete e1;
      return 1;
   }

   if(e1->GetDicomDirPatients().begin() == e1->GetDicomDirPatients().end() )
   {
      std::cout<<"          DicomDir '"<<file
               <<" has no patient"<<std::endl
               <<"          ...Failed"<<std::endl;

      delete e1;
      return 1;
   }

// Simple examples of structure exploitation 
// DON'T REMOVE neither the following code, nor the commented out lines

/*
  cout << std::endl << std::endl  
        << " = Liste des PATIENT ==========================================" 
        << std::endl<< std::endl;


   itPatient = e1->GetDicomDirPatients().begin();
   while ( itPatient != e1->GetDicomDirPatients().end() ) {  // on degouline la liste de PATIENT
      std::cout << (*itPatient)->GetEntry(0x0010, 0x0010) << std::endl; // Patient's Name   
      itPatient ++;    
   }
*/

/*        
   cout << std::endl << std::endl  
        << " = Liste des PATIENT/STUDY ==========================================" 
        << std::endl<< std::endl;

   itPatient = e1->GetDicomDirPatients().begin();
   while ( itPatient != e1->GetDicomDirPatients().end() ) {  // on degouline la liste de PATIENT
      std::cout << (*itPatient)->GetEntry(0x0010, 0x0010) << std::endl; // Patient's Name 
      itStudy = ((*itPatient)->GetDicomDirStudies()).begin();
      while (itStudy != (*itPatient)->GetDicomDirStudies().end() ) { // on degouline les STUDY de ce patient
         std::cout << "--- "<< (*itStudy)->GetEntry(0x0008, 0x1030) << std::endl; // Study Description
         ++itStudy;
      }
      itPatient ++;    
   }   
 */
 
   std::cout << std::endl << std::endl  
             << " = Liste des PATIENT/STUDY/SERIE ==========================================" 
             << std::endl<< std::endl;
 
   itPatient = e1->GetDicomDirPatients().begin();
   while ( itPatient != e1->GetDicomDirPatients().end() ) // on degouline la liste de PATIENT
   {  
       // Patient's Name, Patient ID 
      std::cout << "Pat.Name:[" << (*itPatient)->GetEntry(0x0010, 0x0010) <<"]";
      std::cout << " Pat.ID:[";
      std::cout << (*itPatient)->GetEntry(0x0010, 0x0020) << "]" << std::endl;
      itStudy = ((*itPatient)->GetDicomDirStudies()).begin();
      while (itStudy != (*itPatient)->GetDicomDirStudies().end() ) { // on degouline les STUDY de ce patient
         std::cout << "--- Stud.descr:["    << (*itStudy)->GetEntry(0x0008, 0x1030) << "]";// Study Description 
         std::cout << " Stud.ID:["<< (*itStudy)->GetEntry(0x0020, 0x0010);                 // Study ID
         std::cout << "]" << std::endl;
         itSerie = ((*itStudy)->GetDicomDirSeries()).begin();
         while (itSerie != (*itStudy)->GetDicomDirSeries().end() ) // on degouline les SERIES de cette study
         {
            std::cout << "--- --- Ser.Descr:["<< (*itSerie)->GetEntry(0x0008, 0x103e)<< "]";// Serie Description
            std::cout << " Ser.nb:[" <<   (*itSerie)->GetEntry(0x0020, 0x0011);            // Serie number
            std::cout << "] Mod.:["    <<   (*itSerie)->GetEntry(0x0008, 0x0060) << "]";   // Modality
            std::cout << std::endl;
            ++itSerie;   
         }
         ++itStudy;
      }
      itPatient ++;    
   } 
   
    
 /*
   cout << std::endl << std::endl  
        << " = Liste des PATIENT/STUDY/SERIE/IMAGE ===================================" 
        << std::endl<< std::endl;
 
   itPatient = e1->GetDicomDirPatients().begin();
   while ( itPatient != e1->GetDicomDirPatients().end() ) {  // on degouline la liste de PATIENT
      std::cout << (*itPatient)->GetEntry(0x0010, 0x0010) << std::endl; // Patient's Name
      itStudy = ((*itPatient)->GetDicomDirStudies()).begin();
      while (itStudy != (*itPatient)->GetDicomDirStudies().end() ) { // on degouline les STUDY de ce patient
         std::cout << "--- "<< (*itStudy)->GetEntry(0x0008, 0x1030) << std::endl; // Study Description
         itSerie = ((*itStudy)->GetDicomDirSeries()).begin();
         while (itSerie != (*itStudy)->GetDicomDirSeries().end() ) { // on degouline les SERIES de cette study
            std::cout << "--- --- "<< (*itSerie)->GetEntry(0x0008, 0x103e) << std::endl; // Serie Description
            itImage = ((*itSerie)->GetDicomDirImages()).begin();
            while (itImage != (*itSerie)->GetDicomDirImages().end() ) { // on degouline les SERIES de cette study
               std::cout << "--- --- --- "<< (*itImage)->GetEntry(0x0004, 0x1500) << std::endl; // File name
               ++itImage;   
            }
            ++itSerie;   
         }
         ++itStudy;
      }  
      itPatient ++;    
   }   
 */  

   std::cout << std::endl << std::endl  
             << " = Contenu Complet du DICOMDIR ==========================================" 
             << std::endl<< std::endl;
   e1->Print();
   
   std::cout<<std::flush;
   delete e1;

   return 0;
}
