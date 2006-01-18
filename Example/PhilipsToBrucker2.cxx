/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PhilipsToBrucker2.cxx,v $
  Language:  C++
  Date:      $Date: 2006/01/18 15:45:21 $
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
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmDirList.h"
#include "gdcmDebug.h"
#include "gdcmArgMgr.h"
#include "gdcmUtil.h"
#include "gdcmSerieHelper.h"

#include <iostream>

/**
  * \brief   
  *          - explores recursively the given directory
  *          - keeps the requested series
  *          - orders the gdcm-readable found Files
  *            according to their Patient/Study/Serie/Image characteristics
  *          - fills a single level Directory with *all* the files,
  *            converted into a Brucker-like Dicom, Intags compliant
  *          
  */  

typedef std::map<std::string, gdcm::File*> SortedFiles;

int main(int argc, char *argv[]) 
{
   START_USAGE(usage)
   " \n PhilipsToBrucker :\n                                                  ",
   " - explores recursively the given directory,                              ",
   " - keeps the requested series/ drops the unrequested series               ",
   " - orders the gdcm-readable found Files according to their                ",
   "           (0x0010, 0x0010) Patient's Name                                ",
   "           (0x0020, 0x000e) Series Instance UID                           ",
   "           (0x0020, 0x0032) Image Position (Patient)                      ",
   "           (0x0018, 0x1060) Trigger Time                                  ",
   "           (0x0018, 0x1312) In-plane Phase Encoding Direction             ",
   " - fills a single level (*) Directory with *all* the files,               ",
   "           converted into a Brucker-like Dicom, InTags compliant          ",
   "   (*) actually : creates as many directories as Patients                 ",
   "                  -that shouldn't appear, but being carefull is better ! -",
   " or                                                                       ",
   " - fills a tree-like structure of directories as :                        ",
   "        - Patient                                                         ",
   "        -- Serie                                                          ",
   "        --- Position                                                      ",
   "            Images are (sorted by Trigger Time /                          ",
   "                     Encoding Direction (Row, Column)                     ",
   "      use :                                                               ",
   "           0x0021, 0x1020 : 'SLICE INDEX'                                 ",
   "           0x0021, 0x1040 : 'FRAME INDEX'                                 ",
   "           0x0020, 0x0012 : 'SESSION INDEX'  (Acquisition Number)         ",
   " usage:                                                                   ",
   " PhilipsToBrucker dirin=rootDirectoryName                                 ",
   "                  dirout=outputDirectoryName                              ",
   "                  {  [keep= list of seriesNumber to process]              ",
   "                   | [drop= list of seriesNumber to ignore] }             ",
   "                  [extent=image suffix (.IMA, .NEMA, .DCM, ...)]          ",
   "                  [listonly] [split]                                      ",
   "                  [noshadowseq][noshadow][noseq] [verbose] [debug]        ",
   "                                                                          ",
   " dirout : will be created if doesn't exist                                ",
   " keep : if user wants to process a limited number of series               ",
   "            he gives the list of 'SeriesNumber' (tag 0020|0011)           ",
   " drop : if user wants to ignore a limited number of series                ",
   "            he gives the list of 'SeriesNumber' (tag 0020|0011)           ",   
   "        SeriesNumber are short enough to be human readable                ",
   "        e.g : 1030,1035,1043                                              ",
   " extent : DO NOT forget the leading '.' !                                 ",
   " split: creates a tree-like structure of directories as :                 ",
   "        - Patient                                                         ",
   "        -- Serie                                                          ",
   "        --- Position                                                      ",
   "            Images are (sorted by Trigger Time /                          ",
   "                     Encoding Direction (Row, Column)                     ",
   " noshadowseq: user doesn't want to load Private Sequences                 ",
   " noshadow : user doesn't want to load Private groups (odd number)         ",
   " noseq    : user doesn't want to load Sequences                           ",
   " verbose  : user wants to run the program in 'verbose mode'               ",
   " debug    : *developer*  wants to run the program in 'debug mode'         ",
   FINISH_USAGE

   // ----- Initialize Arguments Manager ------
      
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage")) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }

   char *dirNamein;   
   dirNamein  = am->ArgMgrGetString("dirin",(char *)"."); 

   char *dirNameout;   
   dirNameout  = am->ArgMgrGetString("dirout",(char *)".");  
   
   int loadMode = gdcm::LD_ALL;
   if ( am->ArgMgrDefined("noshadowseq") )
      loadMode |= gdcm::LD_NOSHADOWSEQ;
   else 
   {
   if ( am->ArgMgrDefined("noshadow") )
         loadMode |= gdcm::LD_NOSHADOW;
      if ( am->ArgMgrDefined("noseq") )
         loadMode |= gdcm::LD_NOSEQ;
   }

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();
      
   bool verbose  = am->ArgMgrDefined("verbose");
   bool split    = am->ArgMgrDefined("split");
   bool listonly = am->ArgMgrDefined("listonly");
         
   int nbSeriesToKeep;
   int *seriesToKeep = am->ArgMgrGetListOfInt("keep", &nbSeriesToKeep);
   int nbSeriesToDrop;
   int *seriesToDrop = am->ArgMgrGetListOfInt("drop", &nbSeriesToDrop);
 
   if ( nbSeriesToKeep!=0 && nbSeriesToDrop!=0)
   {
      std::cout << "KEEP and DROP are mutually exclusive !" << std::endl;
      delete am;
      return 0;         
   }
   
   char *extent  = am->ArgMgrGetString("extent",".DCM");
        
   // if unused Param we give up
   if ( am->ArgMgrPrintUnusedLabels() )
   { 
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }
   delete am;  // we don't need Argument Manager any longer

   // ----- Begin Processing -----
   
   if ( ! gdcm::DirList::IsDirectory(dirNamein) )
   {
      std::cout << "KO : [" << dirNamein << "] is not a Directory." << std::endl;
      exit(0);
   }
   else
   {
      std::cout << "OK : [" << dirNamein << "] is a Directory." << std::endl;
   }

   std::string systemCommand;
   
   std::cout << "Check for output directory :[" << dirNameout << "]."
             <<std::endl;
   if ( ! gdcm::DirList::IsDirectory(dirNameout) )    // dirout not found
   {
      std::string strDirNameout(dirNameout);          // to please gcc 4
      systemCommand = "mkdir " +strDirNameout;        // create it!
      if (verbose)
         std::cout << systemCommand << std::endl;
      system (systemCommand.c_str());
      if ( ! gdcm::DirList::IsDirectory(dirNameout) ) // be sure it worked
      {
          std::cout << "KO : not a dir : [" << dirNameout << "] (creation failure ?)" << std::endl;
          exit(0);
      }
      else
      {
        std::cout << "Directory [" << dirNameout << "] created." << std::endl;
      }
   }
   else
   {
       std::cout << "Output Directory [" << dirNameout << "] already exists; Used as is." << std::endl;
   }
    
   std::string strDirNamein(dirNamein);
   gdcm::DirList dirList(strDirNamein, true); // get recursively the list of files
   
   if (listonly)
   {
      std::cout << "------------List of found files ------------" << std::endl;
      dirList.Print();
   }

   gdcm::DirListType fileNames;
   fileNames = dirList.GetFilenames();
   gdcm::SerieHelper *s;              // Needed only to may use SerieHelper::AddSeriesDetail()
   s = gdcm::SerieHelper::New();

/*       
   std::cout << "---------------Print Serie--------------" << std::endl; 
   s->SetDirectory(dirNamein, true); // true : recursive exploration 
   s->SetUseSeriesDetails(true);  
   s->AddSeriesDetail(0x0018, 0x1312);   
   s->Print();
*/
  
   gdcm::File *f;
   gdcm::FileHelper *fh;
/*   
   std::cout << "---------------Print Unique Series identifiers---------"  
             << std::endl;     
   std::string uniqueSeriesIdentifier;
 
   for (gdcm::DirListType::iterator it = fileNames.begin();  
                                    it != fileNames.end();
                                  ++it)
   {
      std::cout << "File Name : " << *it << std::endl;
      f = gdcm::File::New();
      f->SetLoadMode(gdcm::LD_ALL);
      f->SetFileName( *it );
      f->Load();
        
      uniqueSeriesIdentifier=s->CreateUniqueSeriesIdentifier(f);
      std::cout << "                           [" <<
               uniqueSeriesIdentifier  << "]" << std::endl;       
      f->Delete();
   }
*/
   
   if (verbose)
      std::cout << "------------------Print Break levels-----------------" << std::endl;

   std::string userFileIdentifier; 
   SortedFiles sf;

   s->AddSeriesDetail(0x0010, 0x0010, false); // Patient's Name
   s->AddSeriesDetail(0x0020, 0x000e, false); // Series Instance UID
   s->AddSeriesDetail(0x0020, 0x0032, false); // Image Position (Patient)     
   s->AddSeriesDetail(0x0018, 0x1060, true);  // Trigger Time (true: convert to keep numerical order)
   s->AddSeriesDetail(0x0018, 0x1312, false); // In-plane Phase Encoding Direction 
      
   for (gdcm::DirListType::iterator it = fileNames.begin();  
                                    it != fileNames.end();
                                  ++it)
   {
      f = gdcm::File::New();
      f->SetLoadMode(loadMode);
      f->SetFileName( *it );
      f->Load();
      
      // keep only requested Series
      std::string strSeriesNumber;
      int seriesNumber;
      int j;
      
      bool keep = false;
      if (nbSeriesToKeep != 0)
      {     
         strSeriesNumber = f->GetEntryString(0x0020, 0x0011 );
         seriesNumber = atoi( strSeriesNumber.c_str() );
         for (j=0;j<nbSeriesToKeep; j++)
         {
            if(seriesNumber == seriesToKeep[j])
            {
               keep = true;
               break;
            }
         }
         if ( !keep)
         {
            f->Delete();
            continue;
         } 
      }
      // drop all unrequested Series
      bool drop = false;
      if (nbSeriesToDrop != 0)
      {     
         strSeriesNumber = f->GetEntryString(0x0020, 0x0011 );
         seriesNumber = atoi( strSeriesNumber.c_str() );
         for (j=0;j<nbSeriesToDrop; j++)
         {
            if(seriesNumber == seriesToDrop[j])
            { 
               drop = true;
               break;
            }
        }
        if (drop)
        {
           f->Delete();
           continue;
        }
      }      

      userFileIdentifier=s->CreateUserDefinedFileIdentifier(f); 
     // userFileIdentifier += "_";
      //userFileIdentifier += *it;       
      std::cout << "                           [" <<
              userFileIdentifier  << "]" << std::endl;
      
      // storing in a map ensures automatic sorting !      
      sf[userFileIdentifier] = f;
   }
      
   std::vector<std::string> tokens;
   std::string fullFilename, lastFilename;
   std::string previousPatientName, currentPatientName;
   std::string previousSerieInstanceUID, currentSerieInstanceUID;
   std::string previousImagePosition, currentImagePosition;
   std::string previousPhaseEncodingDirection, currentPhaseEncodingDirection;
   std::string previousTriggerTime, currentTriggerTime;
      
   std::string writeDir, currentWriteDir;
   std::string currentPatientWriteDir, currentSerieWriteDir, 
               currentPositionWriteDir, currentPhaseEncodingDirectionWriteDir;

   std::string fullWriteFilename;
   std::string strExtent(extent); 
           
   writeDir = gdcm::Util::NormalizePath(dirNameout);     
   SortedFiles::iterator it2;
 
   previousPatientName            = "";
   previousSerieInstanceUID       = "";   
   previousImagePosition          = "";
   previousPhaseEncodingDirection = "";
   previousTriggerTime            = "";
   
   int sliceIndex = 1;
   int frameIndex = 1;
   int flag       = 0;
       
   gdcm::File *currentFile;
     
   for (it2 = sf.begin() ; it2 != sf.end(); ++it2)
   {  
      currentFile = it2->second;
       
      fullFilename =  currentFile->GetFileName();
      lastFilename =  gdcm::Util::GetName( fullFilename ); 
      std::cout << "Try to write" <<lastFilename << std::endl;
     
      tokens.clear();
      gdcm::Util::Tokenize (it2->first, tokens, "_");
      
      currentPatientName            = tokens[0];
      currentSerieInstanceUID       = tokens[1];
      currentImagePosition          = tokens[2];
      currentTriggerTime            = tokens[3];
      currentPhaseEncodingDirection = tokens[4];           
      
      if ( currentImagePosition[0] == '-')
          currentImagePosition[0] = 'M';
      if ( currentImagePosition[0] == '+')
          currentImagePosition[0] = 'P'; 
      
      if (previousPatientName != currentPatientName)
      {
         previousPatientName = currentPatientName;
         if (verbose)   
            std::cout << "==== new Patient  [" << currentPatientName  << "]" << std::endl;
    
         previousPatientName            = currentPatientName;
         previousSerieInstanceUID       = ""; //currentSerieInstanceUID;
         previousImagePosition          = ""; //currentImagePosition;
         previousTriggerTime            = "";
         previousPhaseEncodingDirection = ""; //currentPhaseEncodingDirection;
  
         currentPatientWriteDir = writeDir + currentPatientName;
         //if ( ! gdcm::DirList::IsDirectory(currentPatientWriteDir) )
           {
              systemCommand   = "mkdir " + currentPatientWriteDir;
              if (verbose)
                 std::cout << systemCommand << std::endl;
              system ( systemCommand.c_str() );
         }
      }

      if (previousSerieInstanceUID != currentSerieInstanceUID)
      {        
         if (verbose)   
            std::cout << "==== === new Serie [" << currentSerieInstanceUID << "]"
                      << std::endl;
         if (split)
         {
             currentSerieWriteDir  = currentPatientWriteDir + gdcm::GDCM_FILESEPARATOR
                             + currentSerieInstanceUID;
             systemCommand   = "mkdir " + currentSerieWriteDir;  
             system (systemCommand.c_str());
         }
         previousSerieInstanceUID       = currentSerieInstanceUID;
         previousImagePosition          = ""; //currentImagePosition;
         previousPhaseEncodingDirection = ""; //currentPhaseEncodingDirection;
      }

      if (previousImagePosition != currentImagePosition)
      {
         frameIndex = 1;
         flag = 0;        
         if (verbose)   
            std::cout << "=== === === new Position [" << currentImagePosition  << "]"
                      << std::endl;
         if (split)
         {
             currentPositionWriteDir  = currentSerieWriteDir + gdcm::GDCM_FILESEPARATOR
                             + currentImagePosition;
             systemCommand   = "mkdir " + currentPositionWriteDir;     
             system (systemCommand.c_str()); 
         }
         previousImagePosition          = currentImagePosition;
         previousPhaseEncodingDirection = ""; //currentPhaseEncodingDirection;
         sliceIndex += 1;
      }      

// We don't split on Row/Column!
/*
      if (previousPhaseEncodingDirection != currentPhaseEncodingDirection)
      {        
         if (verbose)   
            std::cout << "==== === === === new PhaseEncodingDirection [" 
                      << currentPhaseEncodingDirection  << "]" << std::endl;
      
         if (split)
         {
             currentPhaseEncodingDirectionWriteDir  = currentPositionWriteDir 
                             + gdcm::GDCM_FILESEPARATOR
                             + currentPhaseEncodingDirection;
             systemCommand   = "mkdir " + currentPhaseEncodingDirectionWriteDir;     
             system (systemCommand.c_str());     
         }      
    
         previousPhaseEncodingDirection = currentPhaseEncodingDirection;
      } 
*/    
      
      if (verbose)
         std::cout << "--- --- --- --- --- " << (it2->second)->GetFileName() 
                   << std::endl;
   
      if ( gdcm::Debug::GetDebugFlag())
         std::cout << "--- --- --- --- --- " << it2->first << "  " 
                   << (it2->second)->GetFileName() << " " 
                   << gdcm::Util::GetName( fullFilename ) << std::endl;           
      
      // Transform the image to be 'Brucker-Like'
      // ----------------------------------------   
    
      // Deal with 0x0019, 0x1000 : 'FOV'
      int nX = currentFile->GetXSize();
      int nY = currentFile->GetYSize();
      float pxSzX = currentFile->GetXSpacing();
      float pxSzY = currentFile->GetYSpacing();
      char fov[64];
      sprintf(fov, "%f\\%f",nX*pxSzX, nY*pxSzY);
      currentFile->InsertEntryString(fov, 0x0019, 0x1000, "DS");
     
      // Deal with 0x0020, 0x0012 : 'SESSION INDEX'  (Acquisition Number)
      std::string chSessionIndex;
      if (currentPhaseEncodingDirection == "ROW")
         chSessionIndex = "1";
      else
         chSessionIndex = "2"; // suppose it's "COLUMN" !
      currentFile->InsertEntryString(chSessionIndex, 0x0020, 0x0012, "IS");
   
      // Deal with  0x0021, 0x1020 : 'SLICE INDEX'
      char chSliceIndex[5];
      sprintf(chSliceIndex, "%04d", sliceIndex);
      std::string strChSliceIndex(chSliceIndex);
      currentFile->InsertEntryString(strChSliceIndex, 0x0021, 0x1020, "IS");
       
      // Deal with  0x0021, 0x1040 : 'FRAME INDEX' 
      char chFrameIndex[5];
      sprintf(chFrameIndex, "%04d", frameIndex);
      currentFile->InsertEntryString(chFrameIndex, 0x0021, 0x1040, "IS"); 
      
      if (flag == 0)
      {       
         flag = 1;
      }
      else
      {
         frameIndex++;
         flag = 0;
      }
                    
      if (split)
      
         //fullWriteFilename = currentPhaseEncodingDirectionWriteDir + gdcm::GDCM_FILESEPARATOR 
         //                                + lastFilename + strExtent;      
         fullWriteFilename = currentPositionWriteDir + gdcm::GDCM_FILESEPARATOR 
                                         + lastFilename + strExtent; 
      else
         fullWriteFilename = currentPatientWriteDir + gdcm::GDCM_FILESEPARATOR 
                                         + lastFilename + strExtent; 
      
      /*           
      systemCommand  = "cp " + fullFilename + " " + fullWriteFilename;
      std::cout << systemCommand << std::endl;
      system (  systemCommand.c_str() );
      */
            
      // Load the pixels in RAM.    
      
      fh = gdcm::FileHelper::New(currentFile);     
      fh->GetImageDataRaw(); // Don't convert (Gray Pixels + LUT) into (RGB pixels) ?!?
      fh->SetWriteTypeToDcmExplVR();
      if (!fh->Write(fullWriteFilename))
      {
         std::cout << "Fail to write :[" << fullWriteFilename << "]"
                   << std::endl;
      } 
      fh->Delete();                
   }
 }
