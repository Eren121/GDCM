#include <iostream>
#include "gdcm.h"
#include "gdcmHeaderEntry.h"
#include "gdcmDicomDir.h"

#include <fstream>
#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif
#include <string>
#include <iostream>

#define  ostringstream ostrstream
//extern "C"

using namespace std;

int main(int argc, char* argv[])
{  
   gdcmDicomDir *e1;
   ListTag::iterator deb , fin;
   ListPatient::iterator  itPatient;
   ListStudy::iterator itStudy;
   ListSerie::iterator itSerie;
   ListImage::iterator itImage;
   TSKey v;
    
   std::string file; 
   if (argc > 1) 
      file = argv[1];    
   else	
      file = "../gdcmData/DICOMDIR";
      
   e1 = new gdcmDicomDir(file.c_str());
	if (argc > 2) {
	   int level = atoi(argv[2]);   
	   e1->SetPrintLevel(level);
	}

   if(e1->GetPatients().begin() == e1->GetPatients().end() )
   {
      std::cout<<"Empty list"<<std::endl;
      return(1);
   }

// Exemples exploitation de la structure

// DON'T REMOVE neither the folowong code, nor the commented out lines


  cout << std::endl << std::endl  
	     << " = Liste des PATIENT ==========================================" 
	     << std::endl<< std::endl;	


   itPatient = e1->GetPatients().begin();	
   while ( itPatient != e1->GetPatients().end() ) {  // on degouline la liste de PATIENT
      std::cout << (*itPatient)->GetEntryByNumber(0x0010, 0x0010) << std::endl; // Patient's Name   
      itPatient ++;    
   }

/*   
     
   cout << std::endl << std::endl  
        << " = Liste des PATIENT/STUDY ==========================================" 
        << std::endl<< std::endl;	

   itPatient = e1->GetPatients().begin();	
   while ( itPatient != e1->GetPatients().end() ) {  // on degouline la liste de PATIENT
      std::cout << (*itPatient)->GetEntryByNumber(0x0010, 0x0010) << std::endl; // Patient's Name 
      itStudy = ((*itPatient)->GetStudies()).begin();	
      while (itStudy != (*itPatient)->GetStudies().end() ) { // on degouline les STUDY de ce patient	
         std::cout << "--- "<< (*itStudy)->GetEntryByNumber(0x0008, 0x1030) << std::endl; // Stdy Description
         ++itStudy;		
      }
      itPatient ++;    
   }
   
 */
 
   cout << std::endl << std::endl  
	     << " = Liste des PATIENT/STUDY/SERIE ==========================================" 
	     << std::endl<< std::endl;	
 
   itPatient = e1->GetPatients().begin();	
   while ( itPatient != e1->GetPatients().end() ) {  // on degouline la liste de PATIENT
      std::cout << (*itPatient)->GetEntryByNumber(0x0010, 0x0010) << std::endl; // Patient's Name 
      itStudy = ((*itPatient)->GetStudies()).begin();	
      while (itStudy != (*itPatient)->GetStudies().end() ) { // on degouline les STUDY de ce patient	
         std::cout << "--- "<< (*itStudy)->GetEntryByNumber(0x0008, 0x1030) << std::endl; // Study Description 
         itSerie = ((*itStudy)->GetSeries()).begin();
         while (itSerie != (*itStudy)->GetSeries().end() ) { // on degouline les SERIES de cette study	   
            std::cout << "--- ---  "<< (*itSerie)->GetEntryByNumber(0x0008, 0x103e) << std::endl; // Serie Description
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
 
   itPatient = e1->GetPatients().begin();	
   while ( itPatient != e1->GetPatients().end() ) {  // on degouline la liste de PATIENT
      std::cout << (*itPatient)->GetEntryByNumber(0x0010, 0x0010) << std::endl; // Patient's Name
      itStudy = ((*itPatient)->GetStudies()).begin();	
      while (itStudy != (*itPatient)->GetStudies().end() ) { // on degouline les STUDY de ce patient	
         std::cout << "--- "<< (*itStudy)->GetEntryByNumber(0x0008, 0x1030) << std::endl; // Study Description
         itSerie = ((*itStudy)->GetSeries()).begin();
         while (itSerie != (*itStudy)->GetSeries().end() ) { // on degouline les SERIES de cette study	   
            std::cout << "--- --- "<< (*itSerie)->GetEntryByNumber(0x0008, 0x103e) << std::endl; // Serie Description
            itImage = ((*itSerie)->GetImages()).begin();
            while (itImage != (*itSerie)->GetImages().end() ) { // on degouline les SERIES de cette study	   
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

// DICOM DIR

//What about the META informations?

   cout << std::endl << std::endl  
        << " = Contenu Complet du DICOMDIR ==========================================" 
        << std::endl<< std::endl;	
   e1->Print();
   
   std::cout<<std::flush;
   delete e1;

   return(0);
}
