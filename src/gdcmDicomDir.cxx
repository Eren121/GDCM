// gdcmDicomDir.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDir.h"
#include "gdcmPatient.h"
#include "gdcmStudy.h"
#include "gdcmSerie.h"
#include "gdcmImage.h"

#include <string>

gdcmDicomDir::gdcmDicomDir(std::string & FileName,
                           bool exception_on_error):
   gdcmParser(FileName.c_str(),exception_on_error, true )  {


   gdcmPatient *patCur;
   gdcmStudy   *studCur;
   gdcmSerie   *serCur;
   gdcmImage   *imaCur; 
     
   ListTag::iterator i, j;
   
   
     if ( GetListEntry().begin() ==   GetListEntry().end() ) {
        cout << "ListEntry vide " << endl;
     }     
 
     i = GetListEntry().begin();              
     while ( i != GetListEntry().end() ) {

       // std::cout << std::hex << (*i)->GetGroup() << 
       //                  " " << (*i)->GetElement() << endl;

	std::string v = (*i)->GetValue();	   	
        if (v == "PATIENT ") {
	   patCur=new gdcmPatient();
	   //cout << "PATIENT" << endl,
	   patCur->beginObj =i; 
	   GetPatients().push_back(patCur);	     
	}	

        if (v == "STUDY ") {
	    //cout << "STUDY" << endl,
           studCur=new gdcmStudy();
	   studCur->beginObj = patCur->endObj = i;		   
           lPatient::iterator aa = GetPatients().end();
           --aa;
	   (*aa)->GetStudies().push_back(studCur);
	}
           studCur=new gdcmStudy();
	 
        if (v == "SERIES") {
	   //cout << "SERIES" << endl,
           serCur=new gdcmSerie();

	   serCur->beginObj  = studCur->endObj= i; 		
	   lPatient::iterator aa = GetPatients().end();
           --aa;
	   lStudy::iterator bb = (*aa)->GetStudies().end();
	   --bb;
	   (*bb)->GetSeries().push_back(serCur);
	}
		
        if (v == "IMAGE ") {
           //cout << "IMAGE" << endl;
           imaCur=new gdcmImage();
	   imaCur->beginObj  = serCur->endObj= i; 		

	   lPatient::iterator aa = GetPatients().end();
           --aa;
	   lStudy::iterator bb = (*aa)->GetStudies().end();
	   --bb;
	   lSerie::iterator cc = (*bb)->GetSeries().end();
	   --cc; 
	   (*cc)->GetImages().push_back(imaCur);
	   
	   
	   /* ---
	   // ce n'est pas sur une nouvelle IMAGE, qu'il faut intervenir
	   // mais lorsqu'on rencontre un 'non IMAGE' apres des 'IMAGE'
	   lImage::iterator dd = (*cc)->GetImages().end();

	   if ( (*cc)->GetImages().begin() != dd ) {
	      --dd;
	      (*dd)->endObj = i;	   
	   }
	 --- */  	   	   
	}				  
	++i; 
      }      
}


gdcmDicomDir::~gdcmDicomDir() {
   lPatient::iterator cc = GetPatients().begin();
   while  (cc != GetPatients().end() ) {
      //cout << "delete PATIENT" << endl;
      delete *cc;
      ++cc;
   }
}
