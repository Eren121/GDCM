#include <iostream>
#include "gdcm.h"
#include "gdcmDocEntry.h"
#include "gdcmDicomDir.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDirList.h"

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

// ---
void StartMethod(void *toto) {
  (void)toto;
   std::cout<<"Start parsing"<<std::endl;
}

void EndMethod(void *toto) {
  (void)toto;
   std::cout<<"End parsing"<<std::endl;
}
// ---

/**
  * \ingroup Test
  * \brief   Explores recursively the given directory (or GDCM_DATA_ROOT by default)
  *          orders the gdcm-readable found Files
  *          according their Patient/Study/Serie/Image characteristics
  *          makes the gdcmDicomDir 
  *          and writes a file named NewDICOMDIR..
  */  

int main(int argc, char* argv[]) {
  gdcm::DicomDir *dcmdir;
   std::string dirName;   

   if (argc > 1)
      dirName = argv[1];
   else
      dirName = GDCM_DATA_ROOT;

   dcmdir = new gdcm::DicomDir(dirName, true); // we ask for Directory parsing

   dcmdir->SetStartMethod(StartMethod, (void *) NULL);
   dcmdir->SetEndMethod(EndMethod);
   
   gdcm::ListDicomDirPatient lp = dcmdir->GetDicomDirPatients();
   if (! lp.size() ) 
   {
      std::cout << "makeDicomDir: no patient list present. Exiting."
                << std::endl;
      return 1;
   }
    
   dcmdir->WriteDicomDir("NewDICOMDIR");
   std::cout<<std::flush;

   delete dcmdir;
   return 0;
}
