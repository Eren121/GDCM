 
 /*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exExplorePresentationState.cxx,v $
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

#include "gdcmArgMgr.h"

#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmDocEntrySet.h"
//#include "gdcmSerieHelper.h"
#include "gdcmDirList.h"
#include "gdcmUtil.h"

typedef struct {
       // 0020|0013 [IS]  [Instance Number]
       std::string referencedInstanceNumber;
       // 0004|1500 [CS] [Referenced File ID]
       std::string referencedFileName;
} FILEINFO;


  GDCM_NAME_SPACE::SeqEntry *CheckIfSequenceExists(GDCM_NAME_SPACE::File *fPS,  uint16_t gr, uint16_t el);
  GDCM_NAME_SPACE::SeqEntry *CheckIfSequenceExists(GDCM_NAME_SPACE::SQItem *si, uint16_t gr, uint16_t el); 
  bool dealWithCurrentFile(const char *PSName);
  bool dealWithTopLevelItem(GDCM_NAME_SPACE::SQItem* currentItem);
  bool dealWithEndLevelItem(GDCM_NAME_SPACE::SQItem* currentItem);
  void displaySeekResult(GDCM_NAME_SPACE::SeqEntry* currentItem, uint16_t g, uint16_t e);
  bool dealWithSequence(GDCM_NAME_SPACE::SeqEntry* se);
  std::string dealWithReferencedFile(GDCM_NAME_SPACE::SQItem *si);
  std::map<std::string, FILEINFO> dealWithDicomDir(const char *dicomdirname);
    
bool verbose;
bool PSNameAlreadyShown;
std::map<std::string, FILEINFO> m;
std::string currentPSFileName;

   
int main(int argc, char *argv[])
{
   START_USAGE(usage)
   "\n exExplorePresentationState :\n    ",
   FINISH_USAGE
   

   // ----- Initialize Arguments Manager ------
  
   GDCM_NAME_SPACE::ArgMgr *am = new GDCM_NAME_SPACE::ArgMgr(argc, argv);
  
   if (am->ArgMgrDefined("usage") || argc == 1) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }
   
   if (am->ArgMgrDefined("debug"))
      GDCM_NAME_SPACE::Debug::DebugOn();

   /*bool */verbose = am->ArgMgrDefined("verbose");
   bool rec  = ( 0 != am->ArgMgrDefined("rec") );
      
   const char *dirName  = am->ArgMgrGetString("dirin");
   const char *dicomdirname = am->ArgMgrGetString("dicomdir");
      
   /* if unused Param we give up */
   if ( am->ArgMgrPrintUnusedLabels() )
   {
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }  
   delete am;  // ------ we don't need Arguments Manager any longer ------
   
     // ===========================================================================
     // ========================= Deal with the relevant DICOMDIR =================
     // ===========================================================================
   
   
      /*std::map<std::string, std::string> */ m = dealWithDicomDir(dicomdirname);   

     // ===========================================================================
     // ========================= Deal with the Directory holding DICOM images   ==
     // ===========================================================================

      std::cout << " Image Directory Name [" << dirName << "]\n\n" << std::endl;
      
      GDCM_NAME_SPACE::DirList dirList(dirName,rec); // gets recursively (or not) the file list
      GDCM_NAME_SPACE::DirListType fileList = dirList.GetFilenames();
      GDCM_NAME_SPACE::File *f;
      bool res;

      if (fileList.size() == 0)
      {
         std::cout << "No file found in : [" << dirName << "]" << std::endl;
      }
      
      for( GDCM_NAME_SPACE::DirListType::iterator it  = fileList.begin();
                                 it != fileList.end();
                                 ++it )
      {
      
         currentPSFileName = it->c_str();
         if (verbose)
            std::cout << std::endl<<" Start processing :[" << it->c_str() << "]"
                      << std::endl;
         f = GDCM_NAME_SPACE::File::New();
         //f->SetLoadMode(loadMode);
         f->SetFileName( it->c_str() );
 
         res = f->GDCM_NAME_SPACE::Document::Load();

         if ( !res )
         {
            if (verbose)
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
            }
            f->Delete();
            continue;
         }
 
         bool resFile = dealWithCurrentFile(it->c_str());
         if (verbose)
           std::cout << "\n\n"  <<std::endl;
         //std::cout << "=====================================================[" <<  it->c_str() << "]==" << resFile << std::endl;
         //return resFile;
 
         continue;
 
      }
}



//----------------------------------------------------------------------------------------------------
bool dealWithCurrentFile(const char *PSName)
{

   GDCM_NAME_SPACE::File *fPS = GDCM_NAME_SPACE::File::New( );
   fPS->SetFileName( PSName );
   fPS->SetMaxSizeLoadEntry(0xffff);
   bool res2 = fPS->Load();
           
   if (!res2) {
      if (verbose)
          std::cout << "Sorry, " << PSName << " not a gdcm-readable "
                    << "DICOM / ACR File"
                    << std::endl;
      fPS->Delete();
      return false;
   }
   
   GDCM_NAME_SPACE::SeqEntry *se;
   
   se = CheckIfSequenceExists( fPS, 0x0070, 0x0001);
   //displaySeekResult(se, 0x0070, 0x0001);
   if (!se)
   {
         if (verbose)
            std::cout << "[" << PSName << "] : Hopeless (" << std::hex <<  0x0070 << "|" << 0x0001 << std::dec << " doesn't exist...)" <<std::endl;
         return false;      
   }
   
   if (verbose){   
      std::cout << "\n\n =========================================================================================" <<std::endl;       
      std::cout << "[" << PSName << "] is a gdcm-readable PresentationState file, "
                << "that (probabely?) holds one or more 'ROI' within [0070|0001] (Graphic Annotation Sequence)"   <<std::endl; 
      std::cout << " =========================================================================================\n\n" <<std::endl;
   }
   GDCM_NAME_SPACE::SQItem* currentItem = se->GetFirstSQItem(); // Get the first 'ROI'
   if (currentItem == NULL)
   {
      if (verbose)
         std::cout << "======== Deal With NOTHING! (Sequence 0070|0001 [Graphic Annotation Sequence] has NO item ?!?) within " 
                 << PSName << "]" << std::endl;
      return false;
   }
   int i =0;
   bool res3;
   bool finalRes = false;
   while (currentItem != NULL)
   {
      if (verbose)
        std::cout << "======== Deal With 'ROI' n." << i << std::endl;

        // do what you want with the current 'ROI'
         res3 = dealWithTopLevelItem(currentItem);
       if (res3)
         finalRes = true;
        //...

        currentItem = se->GetNextSQItem(); // Get the next 'ROI'
        i++;
   }

   if (finalRes)
      std::cout << "=============================(within PS File :[" <<  PSName << "]\n\n" << std::endl;   
   fPS->Delete();
   
   if(verbose)
      std::cout << "\n\n"  <<std::endl;
   return true;
}
 
//----------------------------------------------------------------------------------------------------
 
bool dealWithTopLevelItem(GDCM_NAME_SPACE::SQItem* currentItem)
{
  // probably this list should be cleaned up.
  // (I didn't find the exact architecture of Presentation State)
   int tabElement[] = {         0x0008, 0x0009, 0x005a, 0x0060, 0x0086, 
                        0x0308, 0x0309, 0x030A, 0x030d, 0x0311, 0x0314, 
                        0x0318, 0x031c, 0x031e, 0x0402, 0x0404, 0x0000 };

   bool finalRes = false;
   bool res;
   GDCM_NAME_SPACE::SeqEntry *se;

   se = CheckIfSequenceExists(currentItem, 0x0008, 0x1140);
   displaySeekResult(se, 0x0008, 0x1140); 
   if (se != 0)
   {
      res = dealWithSequence(se);
      if (!res)
         return false;
   }
   
   for(int i=0; tabElement[i]!=0x0000; i++)
   {
      se = CheckIfSequenceExists(currentItem, 0x0070, tabElement[i]);
      //displaySeekResult(se, 0x0070, tabElement[i]);       
      if (se != 0)
      {
         res = dealWithSequence(se);
         if (res)
            finalRes = true;
      }
   } 
   return (finalRes);
}


//----------------------------------------------------------------------------------------------------
 
bool dealWithEndLevelItem(GDCM_NAME_SPACE::SQItem* currentItem)
{
  // probably this list should be cleaned up, too.
  // (I didn't find the exact architecture of Presentation State)
   int tabElement[] = {         0x0008, 0x0009, 0x005a, 0x0060, 0x0086, 
                        0x0308, 0x0309, 0x030A, 0x030d, 0x0311, 0x0314, 
                        0x0318, 0x031c, 0x031e, 0x0402, 0x0404, 0x0000 };

   bool res = false;
   GDCM_NAME_SPACE::SeqEntry *se;

   se = CheckIfSequenceExists(currentItem, 0x0008, 0x1140);
   displaySeekResult(se, 0x0008, 0x1140); 
   if (se != 0)
   {
      res = true;
      dealWithSequence(se);
   }

   for(int i=0; tabElement[i]!=0x0000; i++)
   {
      se = CheckIfSequenceExists(currentItem, 0x0070, tabElement[i]);
      displaySeekResult(se, 0x0070, tabElement[i]);       
      if (se != 0)
      {
         res = true;
         dealWithSequence(se);
      }
   } 
   return (res);
}
//----------------------------------------------------------------------------------------------------

bool dealWithSequence(GDCM_NAME_SPACE::SeqEntry* se)
{    
   uint16_t g = se->GetGroup();
   uint16_t e = se->GetElement();
   if (verbose)  
      std::cout << std::hex << "\n------------------------ deal with " << g <<"|" << e <<  std::dec 
                << "  " << se->GetName() << std::endl;

   GDCM_NAME_SPACE::SQItem *si = se->GetFirstSQItem();
   if (!si) {
      if (verbose)
         std::cout << "Sequence " << std::hex << g <<"|" << e <<  std::dec <<  "has no Item ?!?" << std::endl;
      return false;
   }
   std::string referencedFileUID;
   //std::cout << "\n============================== [" << currentPSFileName << "] =================" << std::endl;  
   if (g == 0x0008) {
      if (verbose)
         si->Print(std::cout);
      referencedFileUID = dealWithReferencedFile(si);
//      std::cout << "   referencedFile UID[" << referencedFileUID <<"]" << std::endl;
      
   } else if (g == 0x0070) {
      if (verbose)   
         si->Print(std::cout);   
   } else {
        if (verbose)
           std::cout << "Unexpected Group " << std::hex << g << std::hex << std::endl;
   }
   
   si =  se->GetNextSQItem();
   if (si)
   {
      if (verbose)
         std::cout << "Sequence " << std::hex << g <<"|" << e <<  std::dec <<  "should have only ONE Item ?!?" << std::endl;
     // if (verbose)  
     //    si->Print(std::cout);
      return false;
   }
   return true;
}

//----------------------------------------------------------------------------------------------------

void displaySeekResult(GDCM_NAME_SPACE::SeqEntry* se, uint16_t g, uint16_t e)
{  
      if (se)
      {
        // std::cout << std::hex <<  g << "|" << e << std::dec << " [" << se->GetName() << "] exists" <<std::endl;
      }
      else
      {
         if (verbose)
            std::cout << " No " << std::hex <<  g << "|" << e << std::dec << " found" <<std::endl;
      }
}
      
//----------------------------------------------------------------------------------------------------  

bool TestPresentationState(GDCM_NAME_SPACE::File *f, GDCM_NAME_SPACE::File *fPS)
{

/*------------------------------------------------------

0070 0001 SQ 1 Graphic Annotation Sequence
0070 0008 SQ 1 Text Object Sequence
0070 0009 SQ 1 Graphic Object Sequence
0070 005a SQ 1 Displayed Area Selection Sequence
0070 0060 SQ 1 Graphic Layer Sequence
0070 0086 SQ 1 Content Creator's Identification Code Sequence
0070 0308 SQ 1 Registration Sequence
0070 0309 SQ 1 Matrix Registration Sequence
0070 030a SQ 1 Matrix Sequence
0070 030d SQ 1 Registration Type Code Sequence
0070 0311 SQ 1 Fiducial Identifier Code Sequence
0070 0314 SQ 1 Used Fiducials Sequence
0070 0318 SQ 1 Graphic Coordinates Data Sequence
0070 031c SQ 1 Fiducial Set Sequence
0070 031e SQ 1 Fiducial Sequence
0070 0402 SQ 1 Blending Sequence
0070 0404 SQ 1 Referenced Spatial Registration Sequence
------------------------------------------------------- */

/*------------------------------------------------------
Relevant part of Dicom V3 Dict

0070 0000 UL 1 Group Length
0070 0001 SQ 1 Graphic Annotation Sequence
0070 0002 CS 1 Graphic Layer
0070 0003 CS 1 Bounding Box Annotation Units
0070 0004 CS 1 Anchor Point Annotation Units
0070 0005 CS 1 Graphic Annotation Units
0070 0006 ST 1 Unformatted Text Value
0070 0008 SQ 1 Text Object Sequence
0070 0009 SQ 1 Graphic Object Sequence
0070 0010 FL 2 Bounding Box Top Left Hand Corner
0070 0011 FL 2 Bounding Box Bottom Right Hand Corner
0070 0012 CS 1 Bounding Box Text Horizontal Justification
0070 0014 FL 2 Anchor Point
0070 0015 CS 1 Anchor Point Visibility
0070 0020 US 1 Graphic Dimensions
0070 0021 US 1 Number of Graphic Points
0070 0022 FL 2-2n Graphic Data
0070 0023 CS 1 Graphic Type
0070 0024 CS 1 Graphic Filled
0070 0041 CS 1 Image Horizontal Flip
0070 0042 US 1 Image Rotation
0070 0052 SL 2 Displayed Area Top Left Hand Corner
0070 0053 SL 2 Displayed Area Bottom Right Hand Corner
0070 005a SQ 1 Displayed Area Selection Sequence
0070 0060 SQ 1 Graphic Layer Sequence
0070 0062 IS 1 Graphic Layer Order
0070 0066 US 1 Graphic Layer Recommended Display Grayscale Value
0070 0067 US 3 Graphic Layer Recommended Display RGB Value (RET)
0070 0068 LO 1 Graphic Layer Description
0070 0080 CS 1 Content Label
0070 0081 LO 1 Content Description
0070 0082 DA 1 Presentation Creation Date
0070 0083 TM 1 Presentation Creation Time
0070 0084 PN 1 Content Creator's Name
0070 0086 SQ 1 Content Creator's Identification Code Sequence
0070 0100 CS 1 Presentation Size Mode
0070 0101 DS 2 Presentation Pixel Spacing
0070 0102 IS 2 Presentation Pixel Aspect Ratio
0070 0103 FL 1 Presentation Pixel Magnification Ratio
0070 0306 CS 1 Shape Type
0070 0308 SQ 1 Registration Sequence
0070 0309 SQ 1 Matrix Registration Sequence
0070 030a SQ 1 Matrix Sequence
0070 030c CS 1 Frame of Reference Transformation Matrix Type
0070 030d SQ 1 Registration Type Code Sequence
0070 030f ST 1 Fiducial Description
0070 0310 SH 1 Fiducial Identifier
0070 0311 SQ 1 Fiducial Identifier Code Sequence
0070 0312 FD 1 Contour Uncertainty Radius
0070 0314 SQ 1 Used Fiducials Sequence
0070 0318 SQ 1 Graphic Coordinates Data Sequence
0070 031a UI 1 Fiducial UID
0070 031c SQ 1 Fiducial Set Sequence
0070 031e SQ 1 Fiducial Sequence
0070 0401 US 3 Graphic Layer Recommended Display CIELab Value
0070 0402 SQ 1 Blending Sequence
0070 0403 FL 1 Relative Opacity
0070 0404 SQ 1 Referenced Spatial Registration Sequence
0070 0405 CS 1 Blending Position
------------------------------------------------------- */

}

// ----------------------------------------------------------------------------------


GDCM_NAME_SPACE::SeqEntry *CheckIfSequenceExists( GDCM_NAME_SPACE::File *fPS, uint16_t gr, uint16_t el)
{
   GDCM_NAME_SPACE::SeqEntry *se= fPS->GetSeqEntry(gr, el);
   return se;     
}

// ----------------------------------------------------------------------------------
  
GDCM_NAME_SPACE::SeqEntry *CheckIfSequenceExists( GDCM_NAME_SPACE::SQItem *si, uint16_t gr, uint16_t el)
{
   GDCM_NAME_SPACE::SeqEntry *se= si->GetSeqEntry(gr, el);
   return se;     
}

// ----------------------------------------------------------------------------------

std::string dealWithReferencedFile(GDCM_NAME_SPACE::SQItem *si)
{
  // GDCM_NAME_SPACE::DocEntry *si = GetDocEntry(0x0008, 0x1155);
  
  // Look for : 0004|1511  [UI]  [Referenced SOP Instance UID in File]  
  std::string referencedFileUID = si->GetEntryString(0x0008, 0x1155);
  // std::cout << "   referencedFile UID[" << referencedFileUID <<"]" << std::endl;
      
  if (verbose)
     std::cout << "m[" << referencedFileUID << "] = ";

  std::cout << "[" << m[referencedFileUID].referencedInstanceNumber << "] [" << m[referencedFileUID].referencedFileName << "]" << std::endl;

  return referencedFileUID;
   
}  

// ----------------------------------------------------------------------------------
std::map<std::string, FILEINFO> dealWithDicomDir(const char *dicomdirname)
{
   FILEINFO fi;
   GDCM_NAME_SPACE::File *f;
   bool res;
   std::map<std::string, FILEINFO> m;
         
         std::cout << std::endl<<" Start reading DICOMDIR :[" << dicomdirname << "]"
                   << std::endl;
   
         f = GDCM_NAME_SPACE::File::New();
         //f->SetLoadMode(loadMode);
         f->SetFileName( dicomdirname );
 
         // Don't load a DicomDir !
         res = f->GDCM_NAME_SPACE::Document::Load();

         if ( !res )
         {
            std::cout << "Cannot process file [" << dicomdirname << "]" 
                      << std::endl;
            std::cout << "Either it doesn't exist, or it's read protected " 
                      << std::endl;
            std::cout << "or it's not a Dicom File, or its 'header' is bugged" 
                      << std::endl;
            std::cout << "use 'PrintFile filein=... debug' "
                      << "to try to guess the pb"
                      << std::endl;
            f->Delete();
            /// \TODO : throw an exception
            return m;;
         }

   // Get 0004|1220 [SQ]  [Directory Record Sequence]
   
   GDCM_NAME_SPACE::SeqEntry *dsr= f->GetSeqEntry(0x0004,0x1220);
   if (!dsr)
   {
         if (verbose)
            std::cout << "[" << dicomdirname << "] : Hopeless (" << std::hex <<  0x0094 << "|" << 0x01220 
                    << std::dec << " -Directory Record Sequence- doesn't exist...)" <<std::endl;

         f->Delete();
       /// \TODO : throw an exception
         return m;      
   }

   GDCM_NAME_SPACE::SQItem* currentItem = dsr->GetFirstSQItem(); // Get the first 'ROI'
   if (currentItem == NULL)
   {
      if (verbose)
         std::cout << "======== Deal With NOTHING! (Sequence 0004|1220 [Directory Record Sequence] has NO item ?!?)" << std::endl;
       /// \TODO : throw an exception
         return m;   
   }
  
   int i =0;
   while (currentItem != NULL)
   {
       // Get 0020|0013 [IS]  [Instance Number]
       fi.referencedInstanceNumber = currentItem->GetEntryString(0x0020, 0x0013);
  
       // Get 0004|1500 [CS] [Referenced File ID]
       fi.referencedFileName = currentItem->GetEntryString(0x0004, 0x1500);

       // Get0004|1511 [UI] [Referenced SOP Instance UID in File] 
       std::string referencedFileSopInstanceUID = currentItem->GetEntryString(0x0004, 0x1511);

       m[referencedFileSopInstanceUID] = fi;
       currentItem = dsr->GetNextSQItem(); // Get the next 'ROI'
       i++;
   }

   if (verbose)   
      for(std::map<std::string, FILEINFO>::iterator it = m.begin(); it != m.end(); ++it)
      {
         std::cout << "m[" << (*it).first << "] = [" << (*it).second.referencedInstanceNumber << "] ["  << (*it).second.referencedFileName << std::endl;
      }
 
 return m;  

}
