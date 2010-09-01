/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: Rename.cxx,v $
  Language:  C++
  Date:      $Date: 2010/09/01 11:17:32 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmFile.h"
#include "gdcmDirList.h"
#include "gdcmArgMgr.h"

int main(int argc, char *argv[])
{
   START_USAGE(usage)
   " \n copies and renames the files within a directory as ima-SeriesNumber-ImageNumber.dcm (usefull for unaware users!)\n",
   " usage:  Rename dirin=inputDirectoryName  ",
   "                dirout=outputDirectoryName",
   "                [rec] [fix]                    ",
   "      rec : user wants to parse recursively the directory                 ",
   "      fix : user want fix length (4) for SeriesNumber/ ImageNumber        ",
   "      debug      : user wants to run the program in 'debug mode'          ",
   "      warning    : user wants to be warned about any oddity in the File   ",
   "      verose     : user wants to be warned about the programm processing  ",   
      
   FINISH_USAGE
   
   // Initialize Arguments Manager   
   GDCM_NAME_SPACE::ArgMgr *am= new GDCM_NAME_SPACE::ArgMgr(argc, argv);
      
   if (argc == 1 || am->ArgMgrDefined("usage") )
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 1;
   }
   
   const char *dirNameIn   = am->ArgMgrGetString("dirin");
   const char *dirNameOut  = am->ArgMgrGetString("dirout");   
   
   if (am->ArgMgrDefined("debug"))
      GDCM_NAME_SPACE::Debug::DebugOn();

   if (am->ArgMgrDefined("warning"))
      GDCM_NAME_SPACE::Debug::WarningOn();
      
   bool rec      = ( 0 != am->ArgMgrDefined("rec") );
   bool fix      = ( 0 != am->ArgMgrDefined("fix") );
   bool verbose  = ( 0 != am->ArgMgrDefined("verbose") );
            
   /* if unused Param we give up */
   if ( am->ArgMgrPrintUnusedLabels() )
   {
      am->ArgMgrUsage(usage);
      delete am;
      return 1;
   } 

   delete am;  // we don't need Argument Manager any longer

   // ----------- End Arguments Manager ---------
   
   // =============================== Deal with a Directory =====================

#ifdef _MSC_VER
   std::string copyinstr("copy");
#else
   std::string copyinstr("cp");
#endif
   
   char copy[4096]; // Hope it's enough!
   
/*
   char format[5]; // "%d" or "%04d"

   if (fix)
      sprintf(format, "%s", "%04d");
   else
      sprintf(format, "%s", "%d");   
*/      
  
   std::cout << "dirNameIn [" << dirNameIn << "]" << std::endl;
      
   GDCM_NAME_SPACE::DirList dirList(dirNameIn, rec); // gets recursively (or not) the file list
   GDCM_NAME_SPACE::DirListType fileList = dirList.GetFilenames();
   GDCM_NAME_SPACE::File *f;
   bool res;

   if (fileList.size() == 0)
   {
      std::cout << "No file found in : [" << dirNameIn << "]" << std::endl;
   }

   // Remember :
   //0020|0011 [IS]  [Series Number]
   //0020|0012 [IS]  [Acquisition Number]
   //0020|0013 [IS]  [Instance Number]
    
   std::string  strSeriesNumber;
   std::string  strAcquisitionNumber;
   std::string  strInstanceNumber;
   int SeriesNumber;
   int AcquisitionNumber;
   int InstanceNumber;
      
   for( GDCM_NAME_SPACE::DirListType::iterator it  = fileList.begin();
                                 it != fileList.end();
                                 ++it )
   {
      if (verbose)
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

      int number;
 
      strSeriesNumber = f->GetEntryString(0x0020,0x0011);
      if ( strSeriesNumber != GDCM_NAME_SPACE::GDCM_UNFOUND ) 
      {
       if (verbose) std::cout << " SeriesNumber [" <<  strSeriesNumber << "]" << std::endl;
         SeriesNumber = atoi(strSeriesNumber.c_str());
      }
    
      strAcquisitionNumber = f->GetEntryString(0x0020,0x0012);
      if ( strAcquisitionNumber != GDCM_NAME_SPACE::GDCM_UNFOUND ) 
      {
       if (verbose) std::cout << " AcquisitionNumber [" <<  strAcquisitionNumber << "]" << std::endl;
         AcquisitionNumber = atoi(strAcquisitionNumber.c_str());
         number = AcquisitionNumber;
      }  

      strInstanceNumber = f->GetEntryString(0x0020,0x0013);
      if ( strInstanceNumber != GDCM_NAME_SPACE::GDCM_UNFOUND ) 
      {
       if (verbose) std::cout << " InstanceNumber [" <<  strInstanceNumber << "]" << std::endl;
         InstanceNumber = atoi(strInstanceNumber.c_str());
         number = InstanceNumber;
      }

      if (fix) 
         sprintf (copy, "%s %s %s%cima-%04d-%04d.dcm", 
                      copyinstr.c_str(),
                      it->c_str(),
                      dirNameOut, GDCM_NAME_SPACE::GDCM_FILESEPARATOR, SeriesNumber,  number);
      else
         sprintf (copy, "%s %s %s%cima-%d-%d.dcm", 
                      copyinstr.c_str(),
                      it->c_str(),
                      dirNameOut, GDCM_NAME_SPACE::GDCM_FILESEPARATOR, SeriesNumber,  number);
        
    
       if (verbose) std::cout << copy << std::endl;
       system (copy);      
   }
}      
      
            
