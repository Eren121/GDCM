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

// DICOM DIR
   cout << std::endl << std::endl  
        << " = Contenu Complet du DICOMDIR ==========================================" 
        << std::endl<< std::endl;	
   e1->Print();
   
   std::cout<<std::flush;
   delete e1;

   return(0);
}
