/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exExtractDicomTags.cxx,v $
  Language:  C++
  Date:      $Date: 2005/09/29 08:49:27 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmDocument.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"
#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmDebug.h"
#include "gdcmUtil.h"

#include "gdcmArgMgr.h"
#include <iostream>

//#include <fstream>

#ifndef _WIN32
#include <unistd.h> //for access, unlink
#else
#include <io.h> //for _access
#endif


int main(int argc, char *argv[])
{

   START_USAGE(usage)
   " \n exExtractDicomTags :\n",
   "  ",
   "         ",
   "",
   " usage: exExtractDicomTags filein=inputDicomFileName ", 
   "                      [debug]  ", 
   "        debug    : user wants to run the program in 'debug mode'        ",
   FINISH_USAGE


   // ----- Initialize Arguments Manager ------
   
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);
  
   if (am->ArgMgrDefined("usage")) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }
   char *fileName = am->ArgMgrWantString("filein",usage);
   if ( fileName == NULL )
   {
      delete am;
      return 0;
   }

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();
 
   // if unused Params we give up
   if ( am->ArgMgrPrintUnusedLabels() )
   { 
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }

   delete am;  // we don't need Argument Manager any longer

   // ----------- End Arguments Manager ---------


   int loadMode = 0x0; // load everything
   gdcm::File *f = new gdcm::File();
   f->SetLoadMode( loadMode );
   f->SetFileName( fileName );
   bool res = f->Load();  

   if ( !res )
   {
      delete f;
      return 0;
   }

   // 
   if (!f->IsReadable())
   {
      std::cout << "NOT a Dicom File : " << fileName <<std::endl;
      delete f;
      return 1;
   }

   std::string MediaStSOPinstUID;      
   std::string TransferSyntax;
   std::string StudyDate;
   std::string StudyTime;
   std::string Modality;
   std::string PatientName;
   std::string PatientID;
   std::string PatientSex;

   //std::string MediaStSOPinstUID;
   std::string SOPInstanceUID; // == image ID
   std::string StudyInstanceUID;
   std::string SeriesInstanceUID;
   std::string AcquistionDate;
   std::string AcquisitionTime;
   std::string AcquisitionDateTime;
   std::string InstitutionName;
   std::string InstitutionalDepartmentName; // always empty
   std::string ProtocolName;


// ------------> Region (Organe) : aucun champ DICOM n'est prévu 
//                        pour contenir cette information

// Get informations on the file : 
//  Modality, Transfer Syntax, Study Date, Study Time
// Patient Name, Media Storage SOP Instance UID, etc

   MediaStSOPinstUID   = f->GetEntryValue(0x0002,0x0002);
   TransferSyntax      = f->GetEntryValue(0x0002,0x0010);
   StudyDate           = f->GetEntryValue(0x0008,0x0020);
   StudyTime           = f->GetEntryValue(0x0008,0x0030);
   PatientName         = f->GetEntryValue(0x0010,0x0010);
   PatientID           = f->GetEntryValue(0x0010,0x0020);  //patientid
   PatientSex          = f->GetEntryValue(0x0010,0x0040);  //sexe
   SOPInstanceUID      = f->GetEntryValue(0x0008,0x0018);  //imageid = SOPinsUID
   StudyInstanceUID    = f->GetEntryValue(0x0020,0x000d);  //STUInsUID                                              [Study Instance UID] [1.2.840.113680.1.103.56887.1017329008.714317]
   SeriesInstanceUID   = f->GetEntryValue(0x0020,0x000e);  //SerInsUID
   AcquistionDate      = f->GetEntryValue(0x0008,0x0022);
   AcquisitionTime     = f->GetEntryValue(0x0008,0x0032);
   AcquisitionDateTime = f->GetEntryValue(0x0008,0x002a);


   Modality            = f->GetEntryValue(0x0008,0x0060);  //modality
   InstitutionName     = f->GetEntryValue(0x0008,0x0080);  //hospital
   // always empty :-(
   InstitutionalDepartmentName     
                     = f->GetEntryValue(0x0008,0x1040);  //departement

   // Radiologist :
   // 0008|0090  [Referring Physician's Name]
   // 0008|1050  [Performing Physician's Name]
   // 0008|1060  [Name of Physician(s) Reading Study] 
   // 0008|1048  [Physician(s) of Record] 
   // 0032|1032  [Requesting Physician] 

   // --> I'll have to ask people working on PACS which one they use. JPRx

   ProtocolName = f->GetEntryValue(0x0018,0x1030); 
  
   // --> Big trouble with nz (mb of planes) and nt (number of 'times')
   // --> that belong to LibIDO, not to DICOM.
   // --> DICOM has 'Number of Frames' (0028|0008), 
   //     that's more or less number of 'times'
   // Volumes are generaly stored in a 'Serie' 
   //  (hope so ... a single Serie be xti-slice xti-times)


   std::string Rows;
   std::string Columns;
   std::string Planes;

   std::string SamplesPerPixel;
   std::string BitsAllocated;
   std::string BitsStored;
   std::string HighBit;
   std::string PixelRepresentation;
   std::string PixelType;
   
   SamplesPerPixel     = f->GetEntryValue(0x0028,0x0002);  // 3 -> RGB
   Rows                = f->GetEntryValue(0x0028,0x0010);  //ny
   Columns             = f->GetEntryValue(0x0028,0x0011);  //nx
   Planes              = f->GetEntryValue(0x0028,0x0012);  //nz

   BitsAllocated       = f->GetEntryValue(0x0028,0x0100);
   BitsStored          = f->GetEntryValue(0x0028,0x0101);
   HighBit             = f->GetEntryValue(0x0028,0x0102);
   PixelRepresentation = f->GetEntryValue(0x0028,0x0103);

   PixelType           = f->GetPixelType();                //type 
/*
   int iRows            = (uint32_t) atoi( Rows.c_str() );    //ny
   int iColumns         = (uint32_t) atoi( Columns.c_str() ); //nz
   int iPlanes          = (uint32_t) atoi( Planes.c_str() );  //nz
   int iSamplesPerPixel = (uint32_t) atoi( SamplesPerPixel.c_str() );
   int iBitsAllocated   = (uint32_t) atoi( BitsAllocated.c_str() );
*/

  

   float sx =  f->GetXSpacing();
   float sy =  f->GetYSpacing();
   float sz =  f->GetZSpacing(); // meaningless for DICOM 
                                 // (cannot be extracted from a single image)

   std::cout << "Rows = ["            << Rows << "]" << std::endl;
   std::cout << "Columns = ["         << Columns << "]" << std::endl;
   std::cout << "Planes = ["          << Planes << "]" << std::endl;
   std::cout << "SamplesPerPixel = [" << SamplesPerPixel << "]" << std::endl;
   std::cout << "BitsAllocated = ["   << BitsAllocated << "]" << std::endl;
   std::cout << "BitsStored = ["      << BitsStored << "]" << std::endl;
   std::cout << "HighBit = ["         << HighBit << "]" << std::endl;
   std::cout << "PixelRepresentation = [" << PixelRepresentation << "]" << std::endl;
   std::cout << "PixelType = ["       << PixelType << "]" << std::endl;

   std::cout << "TransferSyntax = [" << TransferSyntax << "]" << std::endl;
   std::cout << "StudyDate = ["      << StudyDate << "]" << std::endl;
   std::cout << "StudyTime = ["      << StudyTime << "]" << std::endl;
   std::cout << "Modality = ["       << Modality << "]" << std::endl;
   std::cout << "PatientName = ["    << PatientName << "]" << std::endl;
   std::cout << "PatientID = ["      << PatientID << "]" << std::endl;
   std::cout << "PatientSex = ["     << PatientSex << "]" << std::endl;
   std::cout << "SOPInstanceUID = ["              << SOPInstanceUID << "]" 
          << std::endl; 
   std::cout << "StudyInstanceUID = ["            << StudyInstanceUID 
          << "]" << std::endl;
   std::cout << "SeriesInstanceUID = ["           << SeriesInstanceUID 
          << "]" << std::endl;
   std::cout << "AcquistionDate = ["              << AcquistionDate 
          << "]" << std::endl;
   std::cout << "AcquisitionTime = ["             << AcquisitionTime 
          << "]" << std::endl;
   std::cout << "AcquisitionDateTime = ["         << AcquisitionDateTime << "]" 
             << std::endl;
   std::cout << "InstitutionName = ["             << InstitutionName 
          << "]" << std::endl;
   std::cout << "InstitutionalDepartmentName = [" 
                                        << InstitutionalDepartmentName << "]"
                               << std::endl;
   std::cout << "ProtocolName = ["                << ProtocolName << "]" << std::endl;

   std::cout << "GetXSpacing = ["            << sx << "]" << std::endl;
   std::cout << "GetYSpacing = ["            << sy << "]" << std::endl;
   std::cout << "GetZSpacing = ["            << sz << "]" << std::endl;

   return 0;
}
