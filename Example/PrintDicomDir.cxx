/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PrintDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/13 14:20:18 $
  Version:   $Revision: 1.12 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmDocument.h"
#include "gdcmDicomDir.h"
#include "gdcmValEntry.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirImage.h"
#include "gdcmTS.h"
#include "gdcmDebug.h"

#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{  
   gdcm::DicomDir *e1;
   
   gdcm::ListDicomDirPatient::const_iterator  itPatient;
   gdcm::ListDicomDirStudy::const_iterator    itStudy;
   gdcm::ListDicomDirSerie::const_iterator    itSerie;
   gdcm::ListDicomDirImage::const_iterator    itImage;
   gdcm::TSKey v;
    
   std::string fileName; 
   if (argc > 1) 
      fileName = argv[1];    
   else 
   {
      fileName = GDCM_DATA_ROOT;
      fileName += "/DICOMDIR";
   }

   if (argc > 3)
      gdcm::Debug::SetDebugOn();

   e1 = new gdcm::DicomDir( fileName );

   e1->SetPrintLevel(2);
   int detailLevel;
   if (argc > 2)
      detailLevel = atoi(argv[2]);   
   else
      detailLevel = 3;


   if(e1->GetDicomDirPatients().begin() == e1->GetDicomDirPatients().end() )
   {
      std::cout<<"Empty list"<<std::endl;
      return(1);
   }

// Structure use Examples 

   switch (detailLevel)
  { 
  case 1:
     std::cout << std::endl << std::endl  
       << " =  PATIENT List ==========================================" 
       << std::endl<< std::endl;

      itPatient = e1->GetDicomDirPatients().begin();
      while ( itPatient != e1->GetDicomDirPatients().end() ) // on degouline la liste de PATIENT
      {
         std::cout << (*itPatient)->GetEntry(0x0010, 0x0010) << std::endl; // Patient's Name   
         itPatient ++;    
      }
      break;

   case 2:    
      std::cout << std::endl << std::endl  
        << " = PATIENT/STUDY List =======================================" 
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
      break;

   case 3: 
      std::cout << std::endl << std::endl  
        << " =  PATIENT/STUDY/SERIE List ==================================" 
        << std::endl<< std::endl;
 
      itPatient = e1->GetDicomDirPatients().begin();
      while ( itPatient != e1->GetDicomDirPatients().end() )   // on degouline la liste de PATIENT
      {
       // Patient's Name, Patient ID 
         std::cout << "Pat.Name:[" << (*itPatient)->GetEntry(0x0010, 0x0010) <<"]"; // Patient's Name
         std::cout << " Pat.ID:[";
         std::cout << (*itPatient)->GetEntry(0x0010, 0x0020) << "]" << std::endl; // Patient ID
         itStudy = ((*itPatient)->GetDicomDirStudies()).begin();
         while (itStudy != (*itPatient)->GetDicomDirStudies().end() ) { // on degouline les STUDY de ce patient
            std::cout << "--- Stud.descr:["    << (*itStudy)->GetEntry(0x0008, 0x1030) << "]";// Study Description 
            std::cout << " Stud.ID:["          << (*itStudy)->GetEntry(0x0020, 0x0010);       // Study ID
            std::cout << "]" << std::endl;
            itSerie = ((*itStudy)->GetDicomDirSeries()).begin();
            while (itSerie != (*itStudy)->GetDicomDirSeries().end() ) { // on degouline les SERIES de cette study
               std::cout << "--- --- Ser.Descr:["<< (*itSerie)->GetEntry(0x0008, 0x103e)<< "]";  // Series Description
               std::cout << " Ser.nb:["         <<  (*itSerie)->GetEntry(0x0020, 0x0011);        // Series number
               std::cout << "] Mod.:["          <<  (*itSerie)->GetEntry(0x0008, 0x0060) << "]"; // Modality
              std::cout << std::endl;    
               ++itSerie;   
            }
            ++itStudy;
         }
         itPatient ++;    
      } 
      break;

   case 4:  
      std::cout << std::endl << std::endl  
           << " = PATIENT/STUDY/SERIE/IMAGE List ============================" 
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
      break;

   case 5:
      std::cout << std::endl << std::endl  
           << " = DICOMDIR full content ==========================================" 
           << std::endl<< std::endl;
      e1->Print();
      break;

   }  // end switch



   if(e1->IsReadable())
      std::cout <<std::endl<<fileName<<" is Readable"<<std::endl;
   else
      std::cout <<std::endl<<fileName<<" is NOT Readable"<<std::endl;
   std::cout<<std::flush;
   delete e1;

   return(0);
}
