/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exBuildUpDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/03 15:44:50 $
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
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirImage.h"

// ===============================================================
/**
  * \ingroup Test
  * \brief   Builds up ex-nihilo a DICOMDIR file 
  *          adding Patient, Study, Serie, Image descriptions
  *          to an empty gdcmDicomDir occurence
  *          and writes a file named NewDICOMDIR. 
  */
  
int main(int argc, char *argv[])
{
   std::string dirName = "NewDICOMDIR";

   if ( argc > 2 )
      {
      std::cerr << "Usage: " << argv[0] << " [dicomdirname] ";
      dirName = argv[1];
      }

   gdcm::DicomDir *dcmdir;
   dcmdir = new gdcm::DicomDir();

   std::cout << "\n------- BuildUpDicomDir: Test Print Meta only -----" << std::endl;
   ((gdcm::Document *)dcmdir)->Print();

   gdcm::DicomDirPatient *p1=dcmdir->NewPatient();
   p1->SetValEntry("patientONE",0x0010, 0x0010);

     
   gdcm::DicomDirPatient *p2=dcmdir->NewPatient();
   p2->SetValEntry("patientTWO",0x0010, 0x0010);     
   gdcm::DicomDirStudy *s21=p2->NewStudy();  
       s21->SetValEntry("StudyDescrTwo.One",0x0008, 0x1030);        
   gdcm::DicomDirSerie *s211=s21->NewSerie();   
   gdcm::DicomDirImage *s2111=s211->NewImage();
   (void)s2111; //not used

   gdcm::DicomDirStudy *s11=p1->NewStudy();  
   s11->SetValEntry("StudyDescrOne.One",0x0008, 0x1030);
   // Name of the physician reading study
   // Header Entry to be created
   s11->SetValEntry("Dr Mabuse",0x0008, 0x1060);

   gdcm::DicomDirPatient *p3 = dcmdir->NewPatient();
   p3->SetValEntry("patientTHREE",0x0010, 0x0010);

   std::cout << "\n------- BuildUpDicomDir: Test Print of Patient ONE -----\n";
   p1->Print();
   std::cout << "\n------- BuildUpDicomDir: Test Print of Patient THREE -----\n";
   p3->Print();
   std::cout << "\n------- BuildUpDicomDir: Test Print of Patient TWO -------\n";
   p2->Print();
   std::cout << "\n------- BuildUpDicomDir: Test Full Print-------------------\n";
   dcmdir->SetPrintLevel(-1);
   dcmdir->Print();

   dcmdir->WriteDicomDir( dirName );
   std::cout << std::endl;

   delete dcmdir;

   return 0;
}
