/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: ToInTag.cxx,v $
  Language:  C++
  Date:      $Date: 2007/02/26 08:47:29 $
  Version:   $Revision: 1.10 $
                                                                                
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
  */  

typedef std::map<std::string, gdcm::File*> SortedFiles;

int main(int argc, char *argv[]) 
{
   START_USAGE(usage)
   " \n ToInTag :\n                                                           ",
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
   " ToInTag          dirin=rootDirectoryName                                 ",
   "                  dirout=outputDirectoryName                              ",
   "                  {  [keep= list of seriesNumber to process]              ",
   "                   | [drop= list of seriesNumber to ignore] }             ",
   "                  [taggrid] [skel]                                        ",
   "                  [input = {ACR|DCM}]                                     ", 
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
   " taggrid : user knows all the images are 'grid' -ie : not 'col', not 'raw'",
   " extent : DO NOT forget the leading '.' !                                 ",
   " skel: name skeleton eg : patName_1.nema -> skel=patName_                 ",
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

   std::cout << "inside ToInTag" << std::endl;
   
   // ----- Initialize Arguments Manager ------
      
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage")) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }

   const char *dirNamein;   
   dirNamein  = am->ArgMgrGetString("dirin","."); 

   const char *dirNameout;   
   dirNameout  = am->ArgMgrGetString("dirout",".");  
   
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
      
   int verbose  = am->ArgMgrDefined("verbose");
   int split    = am->ArgMgrDefined("split");
   int listonly = am->ArgMgrDefined("listonly");
         
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
   
   int taggrid = am->ArgMgrDefined("taggrid");
   
   int hasSkel = am->ArgMgrDefined("skel");
   const char *skel;
   if (hasSkel)
      skel = am->ArgMgrGetString("skel");   
      
   const char *extent  = am->ArgMgrGetString("extent",".DCM");
   
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
      return 0;

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
      return 0;

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
      std::cout << std::endl;
   }
   
   gdcm::DirListType fileNames;
   fileNames = dirList.GetFilenames();
   gdcm::SerieHelper *s;              // Needed only to may use SerieHelper::AddSeriesDetail()
   s = gdcm::SerieHelper::New();

   std::string token = "%%%"; // Hope it's enough!
/*       
   std::cout << "---------------Print Serie--------------" << std::endl; 
   s->SetDirectory(dirNamein, true); // true : recursive exploration 
   s->SetUseSeriesDetails(true);  
   s->AddSeriesDetail(0x0018, 0x1312);   
   s->Print();
*/
  
   gdcm::File *f;
   gdcm::FileHelper *fh;
   std::vector<std::string> tokens;
   std::vector<std::string> tokensForFileName;
   
   // For Siemens pb, we need Station Name
   
   gdcm::DirListType::iterator it1 = fileNames.begin();
   f = gdcm::File::New();
   f->SetLoadMode(gdcm::LD_ALL);
   f->SetFileName( *it1 );
   f->Load();
   std::string stationName = f->GetEntryString(0x0008,0x1010);
   f->Delete();   
   
/*   
   std::cout << "---------------Print Unique Series identifiers---------"  
             << std::endl;     
   std::string uniqueSeriesIdentifier;
 
   for (gdcm::DirListType::iterator it) = fileNames.begin();  
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
   if ( stationName != "MRC35150" ) // for Siemens MRC35150, don't deal with 'Series Instance UID'
      s->AddSeriesDetail(0x0020, 0x000e, false); // Series Instance UID
   else
      s->AddSeriesDetail(0x9999, 0x9999, false); // dirty trick to ignore 'Series Instance UID'
   s->AddSeriesDetail(0x0020, 0x0032, false); // Image Position (Patient)     
   s->AddSeriesDetail(0x0018, 0x1060, true);  // Trigger Time (true: convert to keep numerical order)
   s->AddSeriesDetail(0x0018, 0x1312, false); // In-plane Phase Encoding Direction 
   s->AddSeriesDetail(0x0008, 0x103e, false); // Series Description (special Siemens ...)  

   //uint8_t *imageData; // Useless : pixels will not be loaded 
                         //          (images are overwritten)
         
   for (gdcm::DirListType::iterator it = fileNames.begin();  
                                    it != fileNames.end();
                                  ++it)
   {
      f = gdcm::File::New();
      f->SetLoadMode(loadMode);
      f->SetFileName( *it );
      f->Load();

      std::string strSeriesNumber;
      int seriesNumber;
      int j;

      // keep only requested Series      
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
      tokens.clear();
      gdcm::Util::Tokenize (userFileIdentifier, tokens, token); 
   
      int imageNum; // Within FileName
      char newName[1024];
      
      if ( tokens[3] == "gdcmUnfound")  // sometimes Trigger Time is not found. CreateUserDefinedFileIdentifier is not aware of the pb.
      {
         ///this is a trick to build up a lexicographical compliant name :
         ///     eg : fich001.ima vs fich100.ima as opposed to fich1.ima vs fich100.ima
         std::string name = gdcm::Util::GetName( *it );
         if (hasSkel)
         {
            gdcm::Util::Tokenize (name, tokensForFileName, skel);
            imageNum = atoi ( tokensForFileName[0].c_str() );
            // probabely we could write something much more complicated using C++ !
            sprintf (newName, "%s%06d%s", skel, imageNum, extent);
            tokens[3] = newName;
            tokensForFileName.clear();    
         }
         else
            tokens[3] = name;
 
 
         userFileIdentifier = tokens[0] + token + tokens[1] + token + tokens[2] + token 
                    + tokens[3] + token + tokens[4] + token + tokens[5] + token;
      }   
      if (verbose) 
         std::cout << "[" << userFileIdentifier  << "] : " << *it << std::endl;
               
      // storing in a map ensures automatic sorting !      
      sf[userFileIdentifier] = f;
   }
   
   if (verbose)
      std::cout << "  " << std::endl;
      
   std::string fullFilename, lastFilename;
   std::string previousPatientName, currentPatientName;
   std::string previousSerieInstanceUID, currentSerieInstanceUID;
   std::string previousImagePosition, currentImagePosition;
   std::string previousPhaseEncodingDirection, currentPhaseEncodingDirection;
   std::string previousTriggerTime, currentTriggerTime;
   
   std::string currentStudyUID;
   std::string seriesDescription;  
      
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
   
   int sliceIndex = 0; // Is incremented *at the beginning* of processing
   int frameIndex;
   if (taggrid)
       frameIndex = 0;
   else
       frameIndex = 1;
              
   int flag       = 0;
       
   gdcm::File *currentFile;

   std::string defaultStudyUID =  gdcm::Util::CreateUniqueUID();
   std::string defaultSerieUID;
     
   for (it2 = sf.begin() ; it2 != sf.end(); ++it2)
   {  
      currentFile = it2->second;
       
      fullFilename =  currentFile->GetFileName();
      lastFilename =  gdcm::Util::GetName( fullFilename ); 
      std::cout << " --------------------------------------------------"
                << " Rewrite [" <<lastFilename << "]" << std::endl;
     
      tokens.clear();
      gdcm::Util::Tokenize (it2->first, tokens, token);
      
      currentPatientName            = tokens[0];
      currentSerieInstanceUID       = tokens[1];
      currentImagePosition          = tokens[2];
      currentTriggerTime            = tokens[3];
      currentPhaseEncodingDirection = tokens[4];
      seriesDescription             = tokens[5];  // For Siemens pb

      if ( currentImagePosition[0] == '-')
          currentImagePosition[0] = 'M';
      if ( currentImagePosition[0] == '+')
          currentImagePosition[0] = 'P'; 

      // Add a default ImagePositionPatient to avoid confusion at post processing time
      if ( currentFile->GetEntryString(0x0020,0x0032) == gdcm::GDCM_UNFOUND && 
           currentFile->GetEntryString(0x0020,0x0030) == gdcm::GDCM_UNFOUND )
      {
         currentFile->InsertEntryString("0.\\0.\\0.",0x0020, 0x0032, "DS" );
      }

      // Add a default ImagePositionPatient to avoid confusion at post processing time
      if ( currentFile->GetEntryString(0x0020,0x0037) == gdcm::GDCM_UNFOUND && 
           currentFile->GetEntryString(0x0020,0x0035) == gdcm::GDCM_UNFOUND )
      {
         currentFile->InsertEntryString("1.\\0.\\0.\\0.\\1.\\0.",0x0020, 0x0037, "DS" );
      }
      
      if (previousPatientName != currentPatientName)
      {      
         if ( currentFile->GetEntryString(0x0020,0x000d) == gdcm::GDCM_UNFOUND)
         {
            if (verbose)   
               std::cout << "--- new  Study UID created" << std::endl;
            defaultStudyUID =  gdcm::Util::CreateUniqueUID();
            currentFile->InsertEntryString(defaultStudyUID, 0x0020, 0x000d, "UI" );
         }
  
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

      if ( stationName != "MRC35150" ) // for Siemens MRC35150, don't deal with 'Series Instance UID'

      if (previousSerieInstanceUID != currentSerieInstanceUID)
      {        
         if (verbose)   
            std::cout << "==== === new Serie [" << currentSerieInstanceUID << "]"
                      << std::endl;
      
         if ( currentFile->GetEntryString(0x0020,0x000e) == gdcm::GDCM_UNFOUND)
         {
            if (verbose)   
               std::cout << "--- --- new  Serie UID created" << std::endl;
            defaultSerieUID =  gdcm::Util::CreateUniqueUID();
            currentFile->InsertEntryString(defaultSerieUID, 0x0020, 0x000e, "UI" );
         }       
      
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
      // end of stationName != "MRC35150"
      
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
         if (split)
            sliceIndex = 1; // only *one* slice in a given directory
         else
            sliceIndex += 1;
      }
      if (verbose)
         std::cout << "Slice Index : " << sliceIndex << std::endl;      

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
      if (currentFile->IsVRCoherent(0x0019) == 1 )
         currentFile->InsertEntryString(fov, 0x0019, 0x1000, "  ");
      else     
         currentFile->InsertEntryString(fov, 0x0019, 0x1000, "DS");

     
      // Deal with 0x0020, 0x0012 : 'SESSION INDEX'  (Acquisition Number)
      std::string chSessionIndex;
      // CLEANME


      if (taggrid)
      { 
         chSessionIndex = "1";
      }
      else
      {
      /* for SIEMENS MRI :
        D 0008|1010 [SH]  [Station Name ] [MRC35150]
        we have to deal with :
     
        D 0008|103e [LO]  [Series Description ] [fl2d9_line PA 15 90deg]
        D 0008|103e [LO]  [Series Description ] [fl2d9_line PA 15 0deg ]
        (everything is flagged as 'ROW')
      */      
         if ( stationName == "MRC35150" )
         {
            if ( memcmp(seriesDescription.c_str(), "fl2d9linePA1590deg", 18) )
              chSessionIndex = "1";
            else if ( memcmp(seriesDescription.c_str(), "fl2d9linePA150deg", 18) )
              chSessionIndex = "2"; 
            else
            {
               std::cout << "====================== seriesDescription "
                         << " neither fl2d9_line PA 15 90deg nor fl2d9_line PA 15 0deg (?!?) : ["
                         << seriesDescription << "]" << std::endl;
               chSessionIndex = "1";
            }    
         } 
         else  // for all other 'normal' cases
         {
            if (currentPhaseEncodingDirection == "COL" || currentPhaseEncodingDirection == "COL " || currentPhaseEncodingDirection == " COL")
               chSessionIndex = "1";
            else if (currentPhaseEncodingDirection == "ROW" || currentPhaseEncodingDirection == "ROW "|| currentPhaseEncodingDirection == " ROW")
               chSessionIndex = "2"; 
            else
            {
               std::cout << "====================== PhaseEncodingDirection "
                         << " neither COL nor ROW (?!?) : [ "
                         << currentPhaseEncodingDirection << "]" << std::endl;
               chSessionIndex = "1";
            }
         }
      }
      if (currentFile->IsVRCoherent(0x0020) == 1 )     
         currentFile->InsertEntryString(chSessionIndex, 0x0020, 0x0012, "  ");
      else
         currentFile->InsertEntryString(chSessionIndex, 0x0020, 0x0012, "IS");
 
      // Deal with  0x0021, 0x1020 : 'SLICE INDEX'
      char chSliceIndex[5];
      sprintf(chSliceIndex, "%04d", sliceIndex);
      std::string strChSliceIndex(chSliceIndex);
       
      // Deal with  0x0021, 0x1040 : 'FRAME INDEX' 
      char chFrameIndex[5];
      sprintf(chFrameIndex, "%04d", frameIndex);

      std::string stringVR;       
      if (currentFile->IsVRCoherent(0x0021) == 1 )
         stringVR = "  ";
      else
        stringVR = "IS";
  
      currentFile->InsertEntryString(strChSliceIndex, 0x0021, 0x1020, stringVR);
      currentFile->InsertEntryString(chFrameIndex,    0x0021, 0x1040, stringVR); 
 
 
      std::string strImagePositionPatient    = currentFile->GetEntryString(0x0020, 0x0032 );
      if (strImagePositionPatient == gdcm::GDCM_UNFOUND)
      {
         if (verbose)
            std::cout << "Duplicate ImagePosition into ImagePositionPatient" << std::endl;
         currentFile->InsertEntryString(currentFile->GetEntryString(0x0020, 0x0030), 0x0020, 0x0032, "DS" );
      }  
      
      std::string strImageOrientationPatient = f->GetEntryString(0x0020, 0x0037 );
      if (strImageOrientationPatient == gdcm::GDCM_UNFOUND)
      {
         if (verbose)
            std::cout << "Duplicate ImageOrientation into ImageOrientationPatient" << std::endl;      
         currentFile->InsertEntryString(currentFile->GetEntryString(0x0020, 0x0035), 0x0020, 0x0037, "DS" );       
      }
                
      if (taggrid)
         frameIndex++;
      else     
      {     
         if (flag == 0)
         {       
            flag = 1;
         }
         else
         {
            frameIndex++;
            flag = 0;
         }
      } 
      if (verbose)
         std::cout << "Frame Index : " << frameIndex << std::endl; 
                 
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
      uint8_t *imageData = fh->GetImageDataRaw(); // Don't convert (Gray Pixels + LUT) into (RGB pixels) ?!?
      fh->SetWriteTypeToDcmExplVR();
      // We didn't modify pixels -> keep unchanged the following :
      // 'Media Storage SOP Class UID' (0x0002,0x0002)
      // 'SOP Class UID'               (0x0008,0x0016)
      // 'Image Type'                  (0x0008,0x0008)
      // 'Conversion Type'             (0x0008,0x0064)
      
      // Put to Black the burnt-in number.
      nX = currentFile->GetXSize();
      nY = currentFile->GetYSize();
      for(int y=nY-15; y<nY; y++)
         for(int x=nX/3; x<nX/2+50; x++)
           imageData[ y*nX*2 + x ] = 0;   
        
      fh->SetContentType(gdcm::UNMODIFIED_PIXELS_IMAGE);
      if (!fh->Write(fullWriteFilename))
      {
         std::cout << "Fail to write :[" << fullWriteFilename << "]"
                   << std::endl;
      } 
      fh->Delete();                
   }
   return 0;
 }
