/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestBuildUpDicomDir.cxx,v $
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
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirImage.h"
#include "gdcmDirList.h"
#include "gdcmDebug.h"
#include "gdcmUtil.h"

// ===============================================================

/**
  * \brief   Builds up ex-nihilo a DICOMDIR file 
  *          adding Patient, Study, Serie, Image descriptions
  *          to an empty gdcmDicomDir occurence
  *          and writes a file named NewDICOMDIR. 
  */  
int TestBuildUpDicomDir(int argc, char *argv[])
{
   if (argc) 
   {
      std::cerr << "Usage: " << argv[0] << " dummy ";
   }

   gdcm::DicomDir *dcmdir;
   std::string dirName;  

   dcmdir = new gdcm::DicomDir();

   gdcm::DicomDirPatient *p1;
   // --- Forget these 4 lines :
   // just to improve test coverage.
   p1=dcmdir->GetFirstPatient();
   if (!p1)
      std::cout << "BEFORE any Patient creation, a DicomDir has no Patient. Pffff"
                << std::endl;
   // --- end forget

   // Create patient ONE
   // ------------------
   p1 = dcmdir->NewPatient();
   p1->SetValEntry("patientONE",0x0010, 0x0010);
   // fill here other patient characteristics

   gdcm::DicomDirStudy *s11;
   // --- Forget these 4 lines :
   // just to improve test coverage.
   s11=p1->GetFirstStudy();
   if (!s11)
      std::cout << "BEFORE any Study creation, a Patient has no Study. Pffff"
                << std::endl;
   // --- end forget

   // Let's create and add a Study for this Patient
   s11 = p1->NewStudy();  
   s11->SetValEntry("StudyDescrOne.One_",0x0008, 0x1030);
   // we know entry (0008,1060) is not yet created
   s11->InsertValEntry("Dr Mabuse",     0x0008, 0x1060);
   // fill here other Study characteristics

   gdcm::DicomDirStudy *s12 = p1->NewStudy();    
   s12->SetValEntry("StudyDescrOne.Two",0x0008, 0x1030);
   s12->InsertValEntry("Dr Zorglub",    0x0008, 0x1060);
   // fill here other Study characteristics

   gdcm::DicomDirStudy *s13 = p1->NewStudy();  
   s13->SetValEntry("StudyDescrOne.Tree",0x0008, 0x1030);
   s13->InsertValEntry("Dr Follamour",   0x0008, 0x1060);
   // fill here other Study characteristics
 
   gdcm::DicomDirSerie *s111;
   // --- Forget these 4 lines :
   // just to improve test coverage.
   s111=s11->GetFirstSerie();
   if (!s111)
      std::cout << "BEFORE any Serie creation, a Study has no Serie. Pffff"
                << std::endl;
   // --- end forget

   // Let's create and add a Serie for this Study
   s111 = s11->NewSerie();
   s111->SetValEntry("01-01-111", 0x0008, 0x0021);
   // fill here other Serie characteristics

   gdcm::DicomDirImage *s1111;

   // --- Forget these 4 lines :
   // just to improve test coverage.
   s1111=s111->GetFirstImage();
   if (!s1111)
      std::cout << "BEFORE any Image creation, a Serie has no Image. Pffff"
                << std::endl;
   // --- end forget

   // Let's create and add a Image for this Serie
   s1111 = s111->NewImage();
   s1111->SetValEntry("imageFileName1111",0x0004,0x1500);

   gdcm::DicomDirImage *s1112 = s111->NewImage();
   s1112->SetValEntry("imageFileName1112",0x0004,0x1500);

   // Create patient TWO
   // ------------------
   gdcm::DicomDirPatient *p2 = dcmdir->NewPatient();
   p2->SetValEntry("patientTWO",0x0010, 0x0010); 
   // fill here other patient characteristics
    
   gdcm::DicomDirStudy *s21 = p2->NewStudy();  
   s21->SetValEntry("StudyDescrTwo.One",0x0008, 0x1030);        
   // fill here other Study characteristics

   gdcm::DicomDirSerie *s211 = s21->NewSerie();
   s111->SetValEntry("01-01-211", 0x0008, 0x0021);
   // fill here other Serie characteristics

   gdcm::DicomDirImage *s2111 = s211->NewImage();
   s2111->SetValEntry("imageFileName2111",0x0004,0x1500);
   // fill here other Image characteristics

   gdcm::DicomDirImage *s2112 = s211->NewImage();
   s2112->SetValEntry("imageFileName1122",0x0004,0x1500);
   // fill here other Image characteristics

   // Create patient TREE
   // -------------------
   gdcm::DicomDirPatient *p3 = dcmdir->NewPatient();
   p3->SetValEntry("patientTHREE",0x0010, 0x0010);
   // fill here other Patient characteristics

   // Add a new Serie/Image for a Patient's Study created a long time ago
   // -------------------------------------------------------------------
   gdcm::DicomDirSerie *s131 = s13->NewSerie();
   s111->SetValEntry("01-01-131", 0x0008, 0x0021);
   // fill here other Serie characteristics

   gdcm::DicomDirImage *s1311 = s131->NewImage();
   s1311->SetValEntry("imageFileName1311",0x0004,0x1500);
   // fill here other Image characteristics
     
   // Print
   std::cout << "Test/BuildUpDicomDir: Test Print of patients\n";
   p1->Print();
   std::cout << "Test/BuildUpDicomDir: -------------------\n";
   p2->Print();
   std::cout << "Test/BuildUpDicomDir: -------------------\n";
   p3->Print();
   std::cout << "Test/BuildUpDicomDir: Test Print of patients ended\n";
  
   if( !dcmdir->IsReadable() )
   {
      std::cout<<"          Created DicomDir "
               <<" is not readable"<<std::endl
               <<"          ...Failed"<<std::endl;

      delete dcmdir;
      return 1;
   }


   // Print the 'in memory' built up DicomDir
   std::cout << "Test/BuildUpDicomDir: Print all of the DicomDir" << std::endl;
   dcmdir->SetPrintLevel(-1);
   dcmdir->Print();

   // Write it on disc
   dcmdir->WriteDicomDir("NewDICOMDIR");
   delete dcmdir;

   // Read the newly written DicomDir
   gdcm::DicomDir *newDicomDir = new gdcm::DicomDir("NewDICOMDIR");
   if( !newDicomDir->IsReadable() )
   {
      std::cout<<"          Written DicomDir 'NewDICOMDIR'"
               <<" is not readable"<<std::endl
               <<"          ...Failed"<<std::endl;

      delete newDicomDir;
      return 1;
   }
   // Check some value we are sure
   p1 = newDicomDir->GetFirstPatient();
   p2 = newDicomDir->GetNextPatient();
   p3 = newDicomDir->GetNextPatient();

   if (!p1 || !p2 || !p3)
   {
      std::cout << "A patient is missing in written DicomDir"
          << std::endl;
      delete newDicomDir;
      return 1;
   }

   std::cout <<std::endl
             << "----------Final Check ---------------------" 
             <<std::endl;
 
   bool errorFound = false; 
   std::string valueStuff;  
   for (;;) // exit on 'break'
   {
      if ( p1->GetEntryValue(0x0010, 0x0010) != "patientONE" )
      {
         std::cout << "0x0010,0x0010 [" 
                   << p1->GetEntryValue(0x0010, 0x0010)
                   << "]" << std::endl;
         errorFound = true;
         break;
      }
      std::cout << "Patient : [" 
                << p1->GetEntryValue(0x0010, 0x0010)
                << "]" << std::endl;

      if ( !(s11 = p1->GetFirstStudy()) )
      {
         std::cout << "missing first Study Patient One" << std::endl;  
         errorFound = true;
         break;
      }
      valueStuff = s11->GetEntryValue(0x0008, 0x1030);
      if ( valueStuff.find("StudyDescrOne.One_") >= valueStuff.length() )
      {
         std::cout << "1 : 0x0008,0x1030 [" 
                   << valueStuff
                   << "]" << std::endl;
         errorFound = true;
         break;
      }
      std::cout << "Study : [" 
                << valueStuff
                << "]" << std::endl;
 
      valueStuff = s11->GetEntryValue(0x0008, 0x1060);
      if (!gdcm::Util::DicomStringEqual(valueStuff, "Dr Mabuse") )
      {
         std::cout << "2 : 0x0008,0x1060 [" 
                   << s11->GetEntryValue(0x0008,0x1060)
                   << "]" << std::endl;
         errorFound = true;
         break;
      }
      std::cout << "Pysician : [" 
                << valueStuff
                << "]" << std::endl;
      if ( !(s12 = p1->GetNextStudy()) )
      {
         errorFound = true;
         break;
      }
      if ( gdcm::Util::DicomStringEqual(s12->GetEntryValue(0x0008,
                                           0x1030),"StudyDescrOne.Two " ))
      {
         std::cout << "3 0x0008,0x1030 [" 
                   << s12->GetEntryValue(0x0008,0x1030)
                   << "]" << std::endl;
         errorFound = true;
         break;
      }
      std::cout << "Study Descr : [" 
                << s12->GetEntryValue(0x0008,0x1030)
                << "]" << std::endl;

      if ( gdcm::Util::DicomStringEqual(s12->GetEntryValue(0x0008,
                                           0x1060),"Dr Zorglub " ))
      {
         std::cout << "4 0x0008,0x1060 [" 
                   << s12->GetEntryValue(0x0008,0x1060)
                   << "]" << std::endl;
         errorFound = true;
         break;
      }
      std::cout << "Pysician : [" 
                << s12->GetEntryValue(0x0008,0x1060)
                << "]" << std::endl;

      if ( !(s13 = p1->GetNextStudy()) )
      {
         std::cout << "Study StudyDescrOne.Tree missing" << std::endl;
         break;
      }         
      if ( s13->GetEntryValue(0x0008, 0x1030) != "StudyDescrOne.Tree" )
      {
         errorFound = true;
         break;
      }
      std::cout << "Study : [" 
                << valueStuff
                << "]" << std::endl;

      valueStuff = s13->GetEntryValue(0x0008, 0x1060);
      if (!gdcm::Util::DicomStringEqual(valueStuff, "Dr Follamour") )
      {
         std::cout << "5 0x0008,0x1060 [" 
                   << valueStuff
                   << "]" << std::endl;
         errorFound = true;
         break;
      }
      std::cout << "Pysician : [" 
                << valueStuff
                << "]" << std::endl;

      if (!(s111 = s11->GetFirstSerie()) )
      {
         std::cout << "Serie 01-01-111 missing" << std::endl;
         errorFound = true;
         break;
      }

      valueStuff = s111->GetEntryValue(0x0008, 0x0021);
      if (!gdcm::Util::DicomStringEqual(valueStuff, "01-01-131") )
      {
         std::cout << "6 0x0008,0x0021 [" 
                   << valueStuff
                   << "]" << std::endl;
         errorFound = true;
         break;
      }
      std::cout << "Serie : [" 
                << valueStuff
                << "]" << std::endl;

      if ( !(s1111 = s111->GetFirstImage()) )
      {
         std::cout << "missing image S1111" << std::endl;
         errorFound = true;
         break;
      } 
/*
      if ( s1111->GetEntryValue(0x0004,0x1500) != "imageFileName1111 " )
      {
         errorFound = true;
         break;
      }
*/
      if ( !(s1112 = s111->GetNextImage()) )
      {
         std::cout << "missing image S1112" << std::endl;
         errorFound = true;
         break;
      }
/*
      if ( s1112->GetEntryValue(0x0004,0x1500) != "imageFileName1112 " )
      {
         errorFound = true;
         break;
      }
  */
     break; // No error found. Stop looping
   }

   delete newDicomDir;
   if ( errorFound )
   {
      std::cout << "MissWritting / MissReading " << std::endl;
      std::cout<<std::flush;
      return(1);
   }

   std::cout<<std::flush;
   return 0;
}