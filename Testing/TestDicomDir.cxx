#include <iostream>
#include "gdcm.h"
#include "gdcmHeaderEntry.h"
#include "gdcmDICOMDIR.h"

#include <fstream>
#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif

#define  ostringstream ostrstream
//extern "C"


void aff (ListTag::iterator deb, ListTag::iterator fin);

// ===============================================================================

void aff (ListTag::iterator deb, ListTag::iterator fin, int nbFFFE) {
   int count =0;
   for (ListTag::iterator i= deb; i!= fin; i++) {  
         if( (*i)->GetGroup() == 0xfffe) {
	    count ++;
	    if (count == nbFFFE) break;
	 }
   	(*i)->SetPrintLevel(0);
	(*i)->Print();
   } 
}


int main(int argc, char* argv[])
{  
   gdcmDICOMDIR* e1;
   unsigned short int g, e;
   int o;
   TSKey v;
    
   string a; 
   if (argc > 1) 
      a = argv[1];    
   else	
      a = "DICOMDIR";
      
 e1 = new gdcmDICOMDIR(a);

  if(e1->GetPatients().begin() == e1->GetPatients().end() )
        std::cout << "Liste Vide" << std::endl;

// Exemples exploitation de la structure

ListTag::iterator deb , fin;
lPatient::iterator  itPatient;

   cout << std::endl << std::endl  
	<< " = Liste des PATIENT ==========================================" 
	<< std::endl<< std::endl;	


    itPatient = e1->GetPatients().begin();	
    while ( itPatient != e1->GetPatients().end() ) {  // on degouline la liste de PATIENT
	//cout << " = PATIENT ==========================================" << std::endl;	
        //deb = (*itPatient)->beginObj;	
	//fin =  (*((*itPatient)->GetStudies()).begin())->beginObj;
	//e1->SetPrintLevel(2);
	//aff(deb,fin,1); 
	std::cout << (*itPatient)->GetEntryByNumber(0x0010, 0x0010) << std::endl; // Patient's Name   
	itPatient ++;    
    }    

   cout << std::endl << std::endl  
        << " = Liste des PATIENT/STUDY ==========================================" 
	<< std::endl<< std::endl;	
 
    itPatient = e1->GetPatients().begin();	
    while ( itPatient != e1->GetPatients().end() ) {  // on degouline la liste de PATIENT
	std::cout << (*itPatient)->GetEntryByNumber(0x0010, 0x0010) << std::endl; // Patient's Name 
	lStudy::iterator itStudy = ((*itPatient)->GetStudies()).begin();	
	while (itStudy != (*itPatient)->GetStudies().end() ) { // on degouline les STUDY de ce patient	
	   std::cout << "--- "<< (*itStudy)->GetEntryByNumber(0x0008, 0x1030) << std::endl; // Stdy Description 
		
	   
	   ++itStudy;		
	}  
	itPatient ++;    
    }    

   cout << std::endl << std::endl  
	<< " = Liste des PATIENT/STUDY/SERIE ==========================================" 
	<< std::endl<< std::endl;	
 
    itPatient = e1->GetPatients().begin();	
    while ( itPatient != e1->GetPatients().end() ) {  // on degouline la liste de PATIENT
	std::cout << (*itPatient)->GetEntryByNumber(0x0010, 0x0010) << std::endl; // Patient's Name 
	lStudy::iterator itStudy = ((*itPatient)->GetStudies()).begin();	
	while (itStudy != (*itPatient)->GetStudies().end() ) { // on degouline les STUDY de ce patient	
	   std::cout << "--- "<< (*itStudy)->GetEntryByNumber(0x0008, 0x1030) << std::endl; // Study Description 
	   lSerie::iterator itSerie = ((*itStudy)->GetSeries()).begin();
	   while (itSerie != (*itStudy)->GetSeries().end() ) { // on degouline les SERIES de cette study	   
	   std::cout << "--- ---  "<< (*itSerie)->GetEntryByNumber(0x0008, 0x103e) << std::endl; // Serie Description 	   	
	   ++itSerie;   
	   }			   
	   ++itStudy;		
	}  
	itPatient ++;    
    }    
 
   cout << std::endl << std::endl  
	<< " = Contenu Complet du DICOMDIR ==========================================" 
	<< std::endl<< std::endl;	
   
    	
    itPatient = e1->GetPatients().begin();	
    while ( itPatient != e1->GetPatients().end() ) {  // on degouline la liste de PATIENT
	cout << " = PATIENT ==========================================" << std::endl;	
        deb = (*itPatient)->beginObj;	
	fin =  (*((*itPatient)->GetStudies()).begin())->beginObj;
	e1->SetPrintLevel(2);
	aff(deb,fin,1);
	
	lStudy::iterator itStudy = ((*itPatient)->GetStudies()).begin();	
	while (itStudy != (*itPatient)->GetStudies().end() ) { // on degouline les STUDY de ce patient	
           std::cout << " ==== STUDY ==========================================" <<std::endl;		   
           deb = (*itStudy)->beginObj;	
	   fin = (*((*itStudy)->GetSeries()).begin())->beginObj;
	   //e1->SetPrintLevel(2);
	   aff(deb,fin,1);		   		   	
	
	   lSerie::iterator itSerie = ((*itStudy)->GetSeries()).begin();
	   while (itSerie != (*itStudy)->GetSeries().end() ) { // on degouline les SERIES de cette study	   
	      cout << " ======= SERIES ==========================================" << std::endl;	      
              deb = (*itSerie)->beginObj;	
	      fin = (*((*itSerie)->GetImages()).begin())->beginObj;
	      //e1->SetPrintLevel(2);
	      aff(deb,fin,1);	      
	      		      
	      lImage::iterator itImage = ((*itSerie)->GetImages()).begin();
	      while (itImage != (*itSerie)->GetImages().end() ) {  // on degouline les IMAGE de cette serie 
	      
	         cout << " ========== IMAGE ==========================================" << std::endl;	
		 std::cout << "--- --- --- File Name: "<< (*itImage)->GetEntryByNumber(0x0004, 0x1500) << std::endl; // Referenced File 
	                  
	      ++itImage;
	      }	  	      	      
	      ++itSerie;   
	   }	
	   ++itStudy;	
	 }		     	   
	itPatient ++;    
    }
    
    delete e1;
}
