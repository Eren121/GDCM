/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: MakeDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/03 15:40:53 $
  Version:   $Revision: 1.3 $
                                                                                
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

#include <iostream>

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

int main(int argc, char *argv[]) 
{
  // gdcm::Debug::DebugOn();
   std::string dirName;   

   if (argc > 1)
   {
      dirName = argv[1];
   }
   else
   {
      dirName = GDCM_DATA_ROOT;
   }

   gdcm::DicomDir *dcmdir;
    // we ask for Directory parsing
 
    // Old style (still available) :
    // dcmdir = new gdcm::DicomDir(dirName, true);

   // new style (user is allowed no to load Sequences an/or Shadow Groups)
   dcmdir = new gdcm::DicomDir( );
   dcmdir->SetParseDir(true);
// some images have a wrong length for element 0x0000 of private groups
//   dcmdir->SetLoadMode(NO_SEQ | NO_SHADOW);

   dcmdir->SetLoadMode(NO_SEQ);
   dcmdir->Load(dirName);

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
