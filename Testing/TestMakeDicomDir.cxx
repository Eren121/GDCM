/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestMakeDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/02 10:41:10 $
  Version:   $Revision: 1.1 $
                                                                                
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
#include "gdcmDirList.h"
#include "gdcmDebug.h"

// ---
void StartMethod(void *startMethod) 
{
  (void)startMethod;
   std::cout<<"Start parsing"<<std::endl;
}

void EndMethod(void *endMethod) 
{
  (void)endMethod;
   std::cout<<"End parsing"<<std::endl;
}
// ---

/**
  * \brief   - Explores recursively the given directory 
  *            (or GDCM_DATA_ROOT by default)
  *          - Orders the gdcm-readable found Files
  *             according their Patient/Study/Serie/Image characteristics
  *          - Makes the gdcm::DicomDir. 
  *          - Writes a file named "NewDICOMDIR".
  *          - Reads "NewDICOMDIR" file.
  */  

int TestMakeDicomDir(int argc, char *argv[])
{
//   gdcm::Debug::DebugOn();
   std::string dirName;   

   if (argc > 1)
   {
      dirName = argv[1];
   }
   else
   {
      dirName = GDCM_DATA_ROOT;
   }

    // we ask for Directory parsing
    gdcm::DicomDir *dcmdir = new gdcm::DicomDir(dirName, true);

   dcmdir->SetStartMethod(StartMethod, (void *) NULL);
   dcmdir->SetEndMethod(EndMethod);
   
   if ( !dcmdir->GetFirstPatient() ) 
   {
      std::cout << "makeDicomDir: no patient found. Exiting."
                << std::endl;

      delete dcmdir;
      return 1;
   }
    
   // Create the corresponding DicomDir
   dcmdir->WriteDicomDir("NewDICOMDIR");
   delete dcmdir;

   // Read from disc the just written DicomDir
   gdcm::DicomDir *newDicomDir = new gdcm::DicomDir("NewDICOMDIR");
   if( !newDicomDir->IsReadable() )
   {
      std::cout<<"          Written DicomDir 'NewDICOMDIR'"
               <<" is not readable"<<std::endl
               <<"          ...Failed"<<std::endl;

      delete newDicomDir;
      return 1;
   }

   if( !newDicomDir->GetFirstPatient() )
   {
      std::cout<<"          Written DicomDir 'NewDICOMDIR'"
               <<" has no patient"<<std::endl
               <<"          ...Failed"<<std::endl;

      delete newDicomDir;
      return(1);
   }

   std::cout<<std::flush;

   delete newDicomDir;
   return 0;
}