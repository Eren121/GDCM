/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PrintDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/18 07:55:16 $
  Version:   $Revision: 1.16 $
                                                                                
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
   gdcm::TSKey v;

   gdcm::DicomDirPatient *pa;
   gdcm::DicomDirStudy *st;
   gdcm::DicomDirSerie *se;
   gdcm::DicomDirImage *im;
  
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

   // Test if the DicomDir is readable
   if( !e1->IsReadable() )
   {
      std::cout<<"          DicomDir '"<<fileName
               <<"' is not readable"<<std::endl
               <<"          ...Failed"<<std::endl;

      delete e1;
      return 1;
   }

   // Test if the DicomDir contains any Patient
   pa = e1->GetFirstEntry();
   if ( pa  == 0)
   {
      std::cout<<"          DicomDir '"<<fileName
               <<" has no patient"<<std::endl
               <<"          ...Failed"<<std::endl;

      delete e1;
      return 1;
   }

// Structure use Examples 

   switch (detailLevel)
  { 
  case 1:
     std::cout << std::endl << std::endl  
       << " =  PATIENT List ==========================================" 
       << std::endl<< std::endl;

      pa = e1->GetFirstEntry();
      while (pa) 
      {
         std::cout << pa->GetEntry(0x0010, 0x0010) << std::endl; // Patient's Name   
         pa = e1->GetNextEntry();    
      }
      break;

   case 2:    
      std::cout << std::endl << std::endl  
        << " = PATIENT/STUDY List =======================================" 
        << std::endl<< std::endl;

      pa = e1->GetFirstEntry();
      while ( pa ) // on degouline les PATIENT de ce DICOMDIR
      {  
         std::cout << pa->GetEntry(0x0010, 0x0010) << std::endl; // Patient's Name 
         st = pa->GetFirstEntry();
         while ( st ) { // on degouline les STUDY de ce patient
            std::cout << "--- "<< st->GetEntry(0x0008, 0x1030) << std::endl; // Study Description
            st = pa->GetNextEntry();
         }
         pa = e1->GetNextEntry();    
      }   
      break;

   case 3: 
      std::cout << std::endl << std::endl  
        << " =  PATIENT/STUDY/SERIE List ==================================" 
        << std::endl<< std::endl;

      pa = e1->GetFirstEntry(); 
      while ( pa )   // on degouline les PATIENT de ce DICOMDIR
      {
       // Patient's Name, Patient ID 
         std::cout << "Pat.Name:[" << pa->GetEntry(0x0010, 0x0010) <<"]"; // Patient's Name
         std::cout << " Pat.ID:[";
         std::cout << pa->GetEntry(0x0010, 0x0020) << "]" << std::endl; // Patient ID
         st = pa->GetFirstEntry();
         while ( st ) { // on degouline les STUDY de ce patient
            std::cout << "--- Stud.descr:["    << st->GetEntry(0x0008, 0x1030) << "]";// Study Description 
            std::cout << " Stud.ID:["          << st->GetEntry(0x0020, 0x0010);       // Study ID
            std::cout << "]" << std::endl;
            se = st->GetFirstEntry();
            while ( se ) { // on degouline les SERIES de cette study
               std::cout << "--- --- Ser.Descr:["<< se->GetEntry(0x0008, 0x103e)<< "]";  // Series Description
               std::cout << " Ser.nb:["         <<  se->GetEntry(0x0020, 0x0011);        // Series number
               std::cout << "] Mod.:["          <<  se->GetEntry(0x0008, 0x0060) << "]"; // Modality
               std::cout << std::endl;    
               se = st->GetNextEntry();   
            }
            st = pa->GetNextEntry();
         }
         pa = e1->GetNextEntry();    
      } 
      break;

   case 4:  
      std::cout << std::endl << std::endl  
           << " = PATIENT/STUDY/SERIE/IMAGE List ============================" 
           << std::endl<< std::endl;
 
      pa = e1->GetFirstEntry(); 
      while ( pa ) {  // les PATIENT de ce DICOMDIR
         std::cout << pa->GetEntry(0x0010, 0x0010) << std::endl; // Patient's Name
         st = pa->GetFirstEntry();
         while ( st ) { // on degouline les STUDY de ce patient
            std::cout << "--- "<< st->GetEntry(0x0008, 0x1030) << std::endl;    // Study Description
            std::cout << " Stud.ID:["          << st->GetEntry(0x0020, 0x0010); // Study ID
            se = st->GetFirstEntry();
            while ( se ) { // on degouline les SERIES de cette study
               std::cout << "--- --- "<< se->GetEntry(0x0008, 0x103e) << std::endl;      // Serie Description
               std::cout << " Ser.nb:["         <<  se->GetEntry(0x0020, 0x0011);        // Series number
               std::cout << "] Mod.:["          <<  se->GetEntry(0x0008, 0x0060) << "]"; // Modality
               im = se->GetFirstEntry();
               while ( im ) { // on degouline les Images de cette serie
                  std::cout << "--- --- --- "<< im->GetEntry(0x0004, 0x1500) << std::endl; // File name
                  im = se->GetNextEntry();   
               }
               se = st->GetNextEntry();   
           }
            st = pa->GetNextEntry();
        }     
        pa = e1->GetNextEntry();    
      }
      break;

   case 5:
      std::cout << std::endl << std::endl  
           << " = DICOMDIR full content ==========================================" 
           << std::endl<< std::endl;
      e1->Print();
      break;

   }  // end switch


 /*
   // Previous code.
   // Kept as an example. Please don't remove
 
   gdcm::ListDicomDirPatient::const_iterator  itPatient;
   gdcm::ListDicomDirStudy::const_iterator    itStudy;
   gdcm::ListDicomDirSerie::const_iterator    itSerie;
   gdcm::ListDicomDirImage::const_iterator    itImage;
   cout << std::endl << std::endl
        << " = Liste des PATIENT/STUDY/SERIE/IMAGE ===================================" 
        << std::endl<< std::endl;
 
   itPatient = e1->GetDicomDirPatients().begin();
   while ( itPatient != e1->GetDicomDirPatients().end() ) {  // on degouline les PATIENT de ce DICOMDIR
      std::cout << (*itPatient)->GetEntry(0x0010, 0x0010) << std::endl; // Patient's Name
      itStudy = ((*itPatient)->GetDicomDirStudies()).begin();
      while (itStudy != (*itPatient)->GetDicomDirStudies().end() ) { // on degouline les STUDY de ce patient
         std::cout << "--- "<< (*itStudy)->GetEntry(0x0008, 0x1030) << std::endl; // Study Description
         itSerie = ((*itStudy)->GetDicomDirSeries()).begin();
         while (itSerie != (*itStudy)->GetDicomDirSeries().end() ) { // on degouline les SERIES de cette study
            std::cout << "--- --- "<< (*itSerie)->GetEntry(0x0008, 0x103e) << std::endl; // Serie Description
            itImage = ((*itSerie)->GetDicomDirImages()).begin();
            while (itImage != (*itSerie)->GetDicomDirImages().end() ) { // on degouline les IMAGES de cette serie
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


   if(e1->IsReadable())
      std::cout <<std::endl<<fileName<<" is Readable"<<std::endl;
   else
      std::cout <<std::endl<<fileName<<" is NOT Readable"<<std::endl;
   std::cout<<std::flush;
   delete e1;

   return(0);
}
