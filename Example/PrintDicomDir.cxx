#include <iostream>
#include "gdcm.h"
#include "gdcmDocument.h"
#include "gdcmDicomDir.h"
#include "gdcmValEntry.h"

#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{  
   gdcmDicomDir *e1;
   
   ListDicomDirPatient::iterator  itPatient;
   ListDicomDirStudy::iterator    itStudy;
   ListDicomDirSerie::iterator    itSerie;
   ListDicomDirImage::iterator    itImage;
   TSKey v;
    
   std::string file; 
   if (argc > 1) 
      file = argv[1];    
   else {
      file += GDCM_DATA_ROOT;
      file += "/DICOMDIR";
   }

   e1 = new gdcmDicomDir( file );

   if (argc > 2) {
      int level = atoi(argv[2]);   
      e1->SetPrintLevel(level);
   }

   if(e1->GetDicomDirPatients().begin() == e1->GetDicomDirPatients().end() )
   {
      std::cout<<"Empty list"<<std::endl;
      return(1);
   }

// Structure use Examples 
// DON'T REMOVE neither the following code, nor the commented out lines

/*
  cout << std::endl << std::endl  
       << " =  PATIENT List ==========================================" 
       << std::endl<< std::endl;

   itPatient = e1->GetDicomDirPatients().begin();
   while ( itPatient != e1->GetDicomDirPatients().end() ) {  // on degouline la liste de PATIENT
      std::cout << (*itPatient)->GetEntryByNumber(0x0010, 0x0010) << std::endl; // Patient's Name   
      itPatient ++;    
   }
*/

/*       
   cout << std::endl << std::endl  
        << " = PATIENT/STUDY List =======================================" 
        << std::endl<< std::endl;

   itPatient = e1->GetDicomDirPatients().begin();
   while ( itPatient != e1->GetDicomDirPatients().end() ) {  // on degouline la liste de PATIENT
      std::cout << (*itPatient)->GetEntryByNumber(0x0010, 0x0010) << std::endl; // Patient's Name 
      itStudy = ((*itPatient)->GetDicomDirStudies()).begin();
      while (itStudy != (*itPatient)->GetDicomDirStudies().end() ) { // on degouline les STUDY de ce patient
         std::cout << "--- "<< (*itStudy)->GetEntryByNumber(0x0008, 0x1030) << std::endl; // Study Description
         ++itStudy;
      }
      itPatient ++;    
   }   

*/

 
   cout << std::endl << std::endl  
        << " =  PATIENT/STUDY/SERIE List ==================================" 
        << std::endl<< std::endl;
 
   itPatient = e1->GetDicomDirPatients().begin();
   while ( itPatient != e1->GetDicomDirPatients().end() ) {  // on degouline la liste de PATIENT
       // Patient's Name, Patient ID 
      std::cout << "Pat.Name:[" << (*itPatient)->GetEntryByNumber(0x0010, 0x0010) <<"]"; // Patient's Name
      std::cout << " Pat.ID:[";
      std::cout << (*itPatient)->GetEntryByNumber(0x0010, 0x0020) << "]" << std::endl; // Patient ID
      itStudy = ((*itPatient)->GetDicomDirStudies()).begin();
      while (itStudy != (*itPatient)->GetDicomDirStudies().end() ) { // on degouline les STUDY de ce patient
         std::cout << "--- Stud.descr:["    << (*itStudy)->GetEntryByNumber(0x0008, 0x1030) << "]";// Study Description 
         std::cout << " Stud.ID:["          << (*itStudy)->GetEntryByNumber(0x0020, 0x0010);       // Study ID
         std::cout << "]" << std::endl;
         itSerie = ((*itStudy)->GetDicomDirSeries()).begin();
         while (itSerie != (*itStudy)->GetDicomDirSeries().end() ) { // on degouline les SERIES de cette study
            std::cout << "--- --- Ser.Descr:["<< (*itSerie)->GetEntryByNumber(0x0008, 0x103e)<< "]";  // Series Description
            std::cout << " Ser.nb:["         <<  (*itSerie)->GetEntryByNumber(0x0020, 0x0011);        // Series number
            std::cout << "] Mod.:["          <<  (*itSerie)->GetEntryByNumber(0x0008, 0x0060) << "]"; // Modality
            std::cout << std::endl;    
            ++itSerie;   
         }
         ++itStudy;
      }
      itPatient ++;    
   } 
  
    
 /*
   cout << std::endl << std::endl  
        << " = PATIENT/STUDY/SERIE/IMAGE List ============================" 
        << std::endl<< std::endl;
 
   itPatient = e1->GetDicomDirPatients().begin();
   while ( itPatient != e1->GetDicomDirPatients().end() ) {  // on degouline la liste de PATIENT
      std::cout << (*itPatient)->GetEntryByNumber(0x0010, 0x0010) << std::endl; // Patient's Name
      itStudy = ((*itPatient)->GetDicomDirStudies()).begin();
      while (itStudy != (*itPatient)->GetDicomDirStudies().end() ) { // on degouline les STUDY de ce patient
         std::cout << "--- "<< (*itStudy)->GetEntryByNumber(0x0008, 0x1030) << std::endl; // Study Description
         itSerie = ((*itStudy)->GetDicomDirSeries()).begin();
         while (itSerie != (*itStudy)->GetDicomDirSeries().end() ) { // on degouline les SERIES de cette study
            std::cout << "--- --- "<< (*itSerie)->GetEntryByNumber(0x0008, 0x103e) << std::endl; // Serie Description
            itImage = ((*itSerie)->GetDicomDirImages()).begin();
            while (itImage != (*itSerie)->GetDicomDirImages().end() ) { // on degouline les SERIES de cette study
               std::cout << "--- --- --- "<< (*itImage)->GetEntryByNumber(0x0004, 0x1500) << std::endl; // File name
               ++itImage;   
            }
            ++itSerie;   
         }
         ++itStudy;
      }  
      itPatient ++;    
   }   
 */  

/*
   cout << std::endl << std::endl  
        << " = DICOMDIR full content ==========================================" 
        << std::endl<< std::endl;
   e1->Print();
*/   
   std::cout<<std::flush;
   delete e1;

   return(0);
}
