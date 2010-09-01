/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: SingleFrames2Multiframe.cxx,v $
  Language:  C++
  Date:      $Date: 2010/09/01 14:40:00 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmDocument.h"
#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmDataEntry.h"
#include "gdcmUtil.h"

#include "gdcmFileHelper.h"
#include "gdcmDebug.h"
#include "gdcmDirList.h"
#include "gdcmGlobal.h"
#include "gdcmDictSet.h"
#include "gdcmArgMgr.h"
#include "gdcmOrientation.h"
#include <iostream>

#include <algorithm>

int main(int argc, char *argv[])
{

   START_USAGE(usage)
   " \n SingleFrames2Multiframe : \n                                          ",
   "   Converts a directory holding a set of SingleFrames to a Multiframe file",
   " usage: SingleFrames2Multiframe {dirin=inputDirectoryName}                ",
   "                        fileout=nomDuFichierMultiframe                    ",
   
   "                       [debug] [warning]                                  ",
   "  studyUID   : *aware* user wants to add the serie                        ",
   "                                             to an already existing study ",
   "  serieUID   : *aware* user wants to give his own serie UID               ",
   "      debug      : user wants to run the program in 'debug mode'          ",
   "      warning    : user wants to be warned about any oddity in the File   ",
   FINISH_USAGE

   // Initialize Arguments Manager   
   GDCM_NAME_SPACE::ArgMgr *am= new GDCM_NAME_SPACE::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage") )
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 1;
   }

   const char *dirName  = am->ArgMgrGetString("dirin");

   if  (dirName == 0 )
   {
      std::cerr << std::endl
        << "'dirin=' must be present;";
      am->ArgMgrUsage(usage); // Display 'usage'  
      delete am;
      return 1;
   }   

   const char *outputFileName = am->ArgMgrGetString("fileout");
   if  (outputFileName == 0 )
   {
      std::cerr << std::endl
        << "'fileout=' must be present;" ;
      am->ArgMgrUsage(usage); // Display 'usage'  
      delete am;
      return 1;
   }
   
   if (am->ArgMgrDefined("debug"))
      GDCM_NAME_SPACE::Debug::DebugOn();

   if (am->ArgMgrDefined("warning"))
      GDCM_NAME_SPACE::Debug::WarningOn();

   bool userDefinedStudy = ( 0 != am->ArgMgrDefined("studyUID") );
   const char *studyUID;
   if (userDefinedStudy)
      studyUID  = am->ArgMgrGetString("studyUID");  

   // not described *on purpose* in the Usage !
   bool userDefinedSerie = ( 0 != am->ArgMgrDefined("serieUID") );       
   const char *serieUID;
   if(userDefinedSerie)
      serieUID = am->ArgMgrGetString("serieUID");

   /* if unused Param we give up */
   if ( am->ArgMgrPrintUnusedLabels() )
   {
      am->ArgMgrUsage(usage);
      delete am;
      return 1;
   } 

   delete am;  // we don't need Argument Manager any longer

   // ----------- End Arguments Manager ---------


      GDCM_NAME_SPACE::DirList dirList(dirName,false); // gets recursively (or not) the file list
      GDCM_NAME_SPACE::DirListType fileList = dirList.GetFilenames();
      GDCM_NAME_SPACE::File *f;
      
      bool res;

      if (fileList.size() == 0)
      {
         std::cout << "No file found in : [" << dirName << "]" << std::endl;
         exit(0);
      }

      // Order on file name (no pertinent info within header for 'secondary capture storage' images
      //fileList.sort();
      std::sort( fileList.begin(), fileList.end() );

      f = GDCM_NAME_SPACE::File::New();
      f->SetFileName( fileList[0].c_str() );
      res = f->Load();

      // if first file is not gdcm-readable, we give up.
      // (maybe we should check *all* the files ...)

      if (!res)
      {
            std::cout << "Cannot process file [" << fileList[0].c_str() << "]" 
                      << std::endl;
            std::cout << "Either it doesn't exist, or it's read protected " 
                      << std::endl;
            std::cout << "or it's not a Dicom File, or its 'header' is bugged" 
                      << std::endl;
            std::cout << "use 'PrintFile filein=... debug' "
                      << "to try to guess the pb"
                      << std::endl;
            f->Delete();
            exit(0);
      } 

      int nX   = f->GetXSize();
      int nY   = f->GetYSize();
      int nZ   = fileList.size();
      int pxSz = f->GetPixelSize();
      int sPP  = f->GetSamplesPerPixel();
      
      std::string stud= f->GetEntryString(0x0020,0x000d );
      std::cout << "------------------------------------------------ Study UID " << stud << std::endl;
      
      
     std::cout << "First file nX " << nX << " nY " << nY << " nZ " << nZ << " pxSz " << pxSz << " sPP " << sPP << std::endl;
     std::cout << "Image size " << nX*nY*pxSz*sPP << std::endl;
     
      char *imageBuffer; //   = new char[nX*nY*pxSz];
      char *globalBuffer = new char[nX*nY*nZ*pxSz*sPP];
      
      int i = 0;  // image counter           
      for( GDCM_NAME_SPACE::DirListType::iterator it  = fileList.begin();
                                 it != fileList.end();
                                 ++it )
      {
         std::cout << std::endl<<" Start processing :[" << it->c_str() << "]"
                   << std::endl;
         f = GDCM_NAME_SPACE::File::New();
         f->SetFileName( it->c_str() );
         res = f->Load();
 
         if ( !res )
         {
            std::cout << "Cannot process file [" << it->c_str() << "]" 
                      << std::endl;
            std::cout << "Either it doesn't exist, or it's read protected " 
                      << std::endl;
            std::cout << "or it's not a Dicom File, or its 'header' is bugged" 
                      << std::endl;
            std::cout << "use 'PrintFile filein=... debug' "
                      << "to try to guess the pb"
                      << std::endl;
            f->Delete();
            continue;
         }
 
         GDCM_NAME_SPACE::FileHelper *fh = GDCM_NAME_SPACE::FileHelper::New(f);
         imageBuffer = (char *)fh->GetImageData();
         int lgr     = fh->GetImageDataSize();

      //  we check all the files are consistent with the first one  
         if ( lgr != nX*nY*pxSz*sPP)
         {
            std::cout << "File : [" << it->c_str() << "] inconsistent with first one (lgr " <<  lgr  << " vs " << nX*nY*pxSz*sPP<< "); ignored!" << std::endl;
            continue;
         }
         else
 
         {
            memcpy(globalBuffer+i*nX*nY*pxSz*sPP, imageBuffer, lgr );
            i++;
         }
         fh->Delete();
      }

      GDCM_NAME_SPACE::FileHelper *fh2 = GDCM_NAME_SPACE::FileHelper::New();

   std::string strStudyUID;
   std::string strSerieUID;

   if (userDefinedStudy)
      strSerieUID = studyUID;
   else
   {
      //strStudyUID = GDCM_NAME_SPACE::Util::CreateUniqueUID();
      
      strStudyUID = stud;
   } 
   
   if (userDefinedSerie)
     strSerieUID = serieUID;
   else
      strSerieUID = GDCM_NAME_SPACE::Util::CreateUniqueUID();
                 
      char temp[64];
      
      sprintf(temp, "%d", sPP);
      fh2->InsertEntryString(temp,0x0028,0x0002, "US"); // Samples per Pixel
           
      sprintf(temp, "%d", nY);
      fh2->InsertEntryString(temp,0x0028,0x0010, "US"); // Number of Rows
   
      sprintf(temp, "%d", nX);
      fh2->InsertEntryString(temp,0x0028,0x0011, "US"); // Number of Columns      
      
      sprintf(temp, "%d", nZ);
      fh2->InsertEntryString(temp,0x0028,0x0008, "IS"); // Number of Frames
      
      sprintf(temp, "%d", pxSz*8);
      fh2->InsertEntryString(temp,0x0028,0x0100, "US"); // Bits Allocated



   // 'Study Instance UID'
   // The user is allowed to create his own Study, 
   //          keeping the same 'Study Instance UID' for various images
   // The user may add images to a 'Manufacturer Study',
   //          adding new Series to an already existing Study

   fh2->InsertEntryString(strStudyUID,0x0020,0x000d,"UI");  //  Study UID   

   // 'Serie Instance UID'
   // The user is allowed to create his own Series, 
   // keeping the same 'Serie Instance UID' for various images
   // The user shouldn't add any image to a 'Manufacturer Serie'
   // but there is no way no to prevent him for doing that
   
   fh2->InsertEntryString(strSerieUID,0x0020,0x000e,"UI");  //  Serie UID
   
      // Set the image Pixel Data
      fh2->SetUserData((uint8_t *)globalBuffer, nX*nY*pxSz*sPP*nZ);
      
      res = fh2->Write(outputFileName);
      
      if(!res)
         std::cout <<"Fail to write [" << outputFileName << "]" <<std::endl;    

      f->Delete();
      fh2->Delete();

      return 0;
}
