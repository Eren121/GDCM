#include <iostream>
#include "gdcm.h"
#include "gdcmDocEntry.h"
#include "gdcmDicomDir.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDirList.h"
#include "gdcmDebug.h"

#include <fstream>
#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif

#define  ostringstream ostrstream

#include <sys/types.h>
#include <errno.h>

// for Directory pb
#ifdef _MSC_VER 
   #include <windows.h> 
   #include <direct.h>
#else
   #include <dirent.h>   
   #include <unistd.h>
#endif

#include <vector>
#include <algorithm>

// ===============================================================
/**
  * \ingroup Test
  * \brief   Builds up ex-nihilo a DICOMDIR file 
  *          adding Patient, Study, Serie, Image descriptions
  *          to an empty gdcmDicomDir occurence
  *          and writes a file named NewDICOMDIR. 
  */
  
int main(int argc, char* argv[])
{
   if (argc) {
      std::cerr << "Usage: " << argv[0] << " dummy ";
   }

   gdcmDicomDir *dcmdir;
   std::string dirName;  

   printf( "BuildUpDicomDir: entering  BuildUpDicomDir\n");

   dcmdir=new gdcmDicomDir();
   printf( "BuildUpDicomDir: exiting new DicomDir\n");

   printf( "\n------- BuildUpDicomDir: Test Print Meta only -----\n");
   ((gdcmDocument *)dcmdir)->Print();

   gdcmDicomDirPatient *p1=dcmdir->NewPatient();
   p1->SetEntryByNumber("patientONE",0x0010, 0x0010);

     
   gdcmDicomDirPatient *p2=dcmdir->NewPatient();
   p2->SetEntryByNumber("patientTWO",0x0010, 0x0010);     
   gdcmDicomDirStudy *s21=p2->NewStudy();  
       s21->SetEntryByNumber("StudyDescrTwo.One",0x0008, 0x1030);        
   gdcmDicomDirSerie *s211=s21->NewSerie();   
   gdcmDicomDirImage *s2111=s211->NewImage();
   (void)s2111; //not used

   gdcmDicomDirStudy *s11=p1->NewStudy();  
       s11->SetEntryByNumber("StudyDescrOne.One",0x0008, 0x1030);
       // Name of the physician reading study
       // Header Entry to be created
       s11->SetEntryByNumber("Dr Mabuse",0x0008, 0x1060);

   gdcmDicomDirPatient *p3=dcmdir->NewPatient();
   p3->SetEntryByNumber("patientTHREE",0x0010, 0x0010);

   printf( "\n------- BuildUpDicomDir: Test Print of Patient ONE -----\n");
   p1->Print();
   printf( "\n------- BuildUpDicomDir: Test Print of Patient THREE -----\n");
   p3->Print();
   printf( "\n------- BuildUpDicomDir: Test Print of Patient TWO -------\n");
   p2->Print();
   printf( "\n------- BuildUpDicomDir: Test Full Print-------------------\n");  
   dcmdir->SetPrintLevel(-1);
   dcmdir->Print();

   dcmdir->Write("NewDICOMDIR");
   std::cout<<std::flush;

   delete dcmdir;

   return 0;
}
