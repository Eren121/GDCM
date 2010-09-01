/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: BrukerToMhd.cxx,v $
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

/**
 * Writes an old style Bruker Dicom file, InTag compliant, from a Raw File
 * User has to supply parameters. 
 * 
 */
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmDebug.h"
#include "gdcmUtil.h"
#include "gdcmDirList.h"

#include "gdcmArgMgr.h"

#include <iostream>
#include <sstream>

void DealWithNiveau1(std::string level1Directory);
void DealWithNiveau2(std::string level2Directory);
void DealWithNiveau3(std::string level3Directory);
/**
  * \brief   
  *          - explores  the given root directory e.g. :
  *            B67d1.Bp1
  *              subject
  *              AdjStatePerStudy
  *              1
  *                 acqp
  *                 fid
  *                 imnd
  *                 pulseprogram
  *                 spnam0
  *                 spnam1
  *                 pdata
  *                    1
  *                       2dseq
  *                       d3proc
  *                       isa
  *                       meta
  *                       procs
  *                       roi
  *                    2
  *                       ...                   
  * 
  *              2
  *                 acqp
  *                 fid
  *                 ...
  *                 pdata
  *              3         
  *                ...
  *          - fills a single level Directory with *all* the files,
  *            converted into a Brucker-like Dicom, Intags compliant
  */  
  



int main(int argc, char *argv[])
{
   START_USAGE(usage)
   " \n BrukerToMhd : \n                                                      ",
   " - explores the given directory, at the 3 levels,                         ",
   " - fills an equivalent Directory with the MHD files,                      ",
   "           converted into a Brucker-like Dicom, InTags compliant          ",
   " usage: BrukerToMhd dirin=rootDirectoryName                               ",
   "                   dirout=outputDirectoryName                             ",
   "                   [{b|l}] b:BigEndian,l:LittleEndian default : l         ",
   "                   [debug] [verbose] [listonly]                           ",
   "                                                                          ",
   "  debug      : developper wants to run the program in 'debug mode'        ",
   FINISH_USAGE
   

   // ------------ Initialize Arguments Manager ----------------  
   GDCM_NAME_SPACE::ArgMgr *am= new GDCM_NAME_SPACE::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage") )
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 1;
   }

   const char *dirNamein;   
   dirNamein  = am->ArgMgrGetString("dirin",".");

   const char *dirNameout;   
   dirNameout  = am->ArgMgrGetString("dirout",".");
   
   int b = am->ArgMgrDefined("b");
   int l = am->ArgMgrDefined("l");
   
   if (am->ArgMgrDefined("debug"))
      GDCM_NAME_SPACE::Debug::DebugOn();

   int verbose  = am->ArgMgrDefined("verbose");      
   int listonly = am->ArgMgrDefined("listonly");
   
   /* if unused Param we give up */
   if ( am->ArgMgrPrintUnusedLabels() )
   {
      am->ArgMgrUsage(usage);
      delete am;
      return 1;
   } 

   delete am;  // we don't need Argument Manager any longer

   // ----------- End Arguments Manager ---------
   

   // ----- Begin Processing -----


   bool bigEndian = GDCM_NAME_SPACE::Util::IsCurrentProcessorBigEndian();

 
   if ( ! GDCM_NAME_SPACE::DirList::IsDirectory(dirNamein) )
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
   if ( ! GDCM_NAME_SPACE::DirList::IsDirectory(dirNameout) )    // dirout not found
   {
      std::string strDirNameout(dirNameout);          // to please gcc 4
      systemCommand = "mkdir " +strDirNameout;        // create it!
      if (verbose)
         std::cout << systemCommand << std::endl;
      system (systemCommand.c_str());
      if ( ! GDCM_NAME_SPACE::DirList::IsDirectory(dirNameout) ) // be sure it worked
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
   GDCM_NAME_SPACE::DirList dirList(strDirNamein, false, true); // DON'T get recursively the list of files
   std::string strDirNameout(dirNameout);   
   
   if (listonly)
   {
      std::cout << "------------List of found files ------------" << std::endl;
      dirList.Print();
      std::cout << std::endl;
      return 1;
   }


   GDCM_NAME_SPACE::DirListType fileNames;
   fileNames = dirList.GetFilenames();

  // GDCM_NAME_SPACE::File *f;
  // GDCM_NAME_SPACE::FileHelper *fh;

   std::string outputFileName;

   // -----------------------------------------------------
   // Iterate to ALL the objets(files/directories) found in the input directory
   // -----------------------------------------------------
   GDCM_NAME_SPACE::DirListType::iterator it;
   
   for (it = fileNames.begin();
        it != fileNames.end();
      ++it)
   {
      if ( ! GDCM_NAME_SPACE::DirList::IsDirectory(*it) )
      { 
         std::cout << "[" << *it << "] is a file" << std::endl;
      }
      
   }
   
    for (it = fileNames.begin();
         it != fileNames.end();
       ++it)
   {
      if ( GDCM_NAME_SPACE::DirList::IsDirectory(*it) )
      { 
         std::cout << "[" << *it << "] is a directory" << std::endl;
         DealWithNiveau1(*it);
      }
   }
   
   /*
   
 // Open the Raw file
   std::ifstream *Fp = new std::ifstream((*it).c_str(), std::ios::in | std::ios::binary);
   if ( ! *Fp )
   {
      std::cout << "Cannot open file: [" << *it << "]" << std::endl;
      delete Fp;
      Fp = 0;
      return 0;
   }
   if (verbose)
      std::cout << "Success to open file: [" << *it << "]" << std::endl;

   // Read the pixels

   int singlePlaneDataSize =  nX*nY*samplesPerPixel*pixelSizeOut;
   int dataSizeIn          =  nX*nY*samplesPerPixel*pixelSize*nZ;

   uint8_t *pixels         = new uint8_t[dataSizeIn];
   uint8_t *planePixelsOut = new uint8_t[singlePlaneDataSize];

   Fp->read((char*)pixels, (size_t)dataSizeIn);

   if ( pixelSize !=1 && ( (l && bigEndian) || (b && ! bigEndian) ) )
   {  
      ConvertSwapZone(pixelSize, pixels, dataSizeIn);
   }

if (verbose)
   std::cout << "After ConvertSwapZone" << std::endl;

// Copy (and convert) pixels of a single plane

     switch ( pixelTypeCode )
     {
       case 8    : CFR(PU8);  break;
       case -8   : CFR(PS8);  break;
       case -16  : CFR(PU16); break;
       case 16   : CFR(PS16); break;
       case -32  : CFR(PS32); break;
       case 32   : CFR(PU32); break;
       case 33   : CFR(PF32); break;
       case 64   : CFR(PD64); break;
     }

if (verbose)
   std::cout << "After CFR" << std::endl;
   
// Create an empty FileHelper

   GDCM_NAME_SPACE::FileHelper *fileH = GDCM_NAME_SPACE::FileHelper::New();
 
 // Get the (empty) image header.
   GDCM_NAME_SPACE::File *fileToBuild = fileH->GetFile();

   // 'Study Instance UID'
   // The user is allowed to create his own Study, 
   //          keeping the same 'Study Instance UID' for various images
   // The user may add images to a 'Manufacturer Study',
   //          adding new Series to an already existing Study

   fileToBuild->InsertEntryString(strStudyUID,0x0020,0x000d,"UI");  //  Study UID   

   // 'Serie Instance UID'
   // The user is allowed to create his own Series, 
   // keeping the same 'Serie Instance UID' for various images
   // The user shouldn't add any image to a 'Manufacturer Serie'
   // but there is no way no to prevent him for doing that
   
   fileToBuild->InsertEntryString(strSerieUID,0x0020,0x000e,"UI");  //  Serie UID
   
   std::ostringstream str;

   // Set the image size
   str.str("");
   str << nX;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0011, "US"); // Columns
   str.str("");
   str << nY;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0010, "US"); // Rows
   
       if (verbose)
         std::cout << "before  debut des choses serieuses2"<< std::endl;      
  
//   str.str("");
//   str << nZ;
//   fileToBuild->InsertEntryString(str.str(),0x0028,0x0008, "IS"); // Number of Frames

   // Set the pixel type
   
   str.str("");
   str << pixelSizeOut*8;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0100, "US"); // Bits Allocated

   str.str("");
   str << pixelSizeOut*8;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0101, "US"); // Bits Stored

   str.str("");
   str << ( pixelSizeOut*8 - 1 );
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0102, "US"); // High Bit

   str.str("");
   str << pixelSign;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0103, "US"); // Pixel Representation

// If you deal with a Serie of images, as slices of a volume,
// it's up to you to tell gdcm, for each image, what are the values of :
// 
// 0020 0032 DS 3 Image Position (Patient)
// 0020 0037 DS 6 Image Orientation (Patient)

   str.str("");
   str << "0.0\\0.0\\0.0";

// take Frame Index as position (we don't know anything else) NO!

   fileToBuild->InsertEntryString(str.str(),0x0020,0x0032, "DS");

   fileToBuild->InsertEntryString("1.0\\0.0\\0.0\\0.0\\1.0\\0.0",0x0020,0x0037, "DS"); //[1\0\0\0\1\0] : Axial
   
   
   
   
   str.str("");
   str << samplesPerPixel;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0002, "US"); // Samples per Pixel

   if (strlen(patientName) != 0)
      fileToBuild->InsertEntryString(patientName,0x0010,0x0010, "PN"); // Patient's Name
    
   //  0=white  
   if(monochrome1)
      fileH->SetPhotometricInterpretationToMonochrome1();

// Special InTag, now.

      fileH->InsertEntryString(chSessionIndex, 0x0020, 0x0012, "IS");

      // Deal with  0x0021, 0x1020 : 'SLICE INDEX'
      // will stay to 0, since the stuff deals with single slice directories
      char chSliceIndex[5];
      sprintf(chSliceIndex, "%04d", sliceIndex);
      std::string strChSliceIndex(chSliceIndex);

      // Deal with  0x0021, 0x1040 : 'FRAME INDEX'

      str.str("");
      str << frameIndex;
      frameIndex++;  // be ready for next one 

      fileH->InsertEntryString(strChSliceIndex, 0x0021, 0x1020, "IS");
      fileH->InsertEntryString(str.str(),    0x0021, 0x1040, "IS");

      // Pixel Size
      /// \TODO Ask user to supply 'Pixel Size' value
      float pxSzX = 1.0;
      float pxSzY = 1.0;

      // Field of view
      char fov[64];
      sprintf(fov, "%f\\%f",nX*pxSzX, nY*pxSzY);
      fileH->InsertEntryString(fov, 0x0019, 0x1000, "DS");


// Set the image Pixel Data
   fileH->SetImageData(planePixelsOut,singlePlaneDataSize);

// Set the writting mode and write the image
   fileH->SetWriteModeToRaw();
   
   

 // Write a DICOM Explicit VR file
   fileH->SetWriteTypeToDcmExplVR();


   outputFileName = strDirNameout +  GDCM_NAME_SPACE::GDCM_FILESEPARATOR + *it + "_ForInTag.dcm";
   if( !fileH->Write(outputFileName ) )
   {
      std::cout << "Failed for [" << outputFileName << "]\n"
                << "           File is unwrittable\n";
   }

// End of :    for (GDCM_NAME_SPACE::DirListType::iterator it = fileNames.begin() ... 


  
   fileH->Delete();

   delete[] pixels;
   delete[] planePixelsOut;
  }
*/
  // return 1;
 } // end of : for (GDCM_NAME_SPACE::DirListType::iterator it



void DealWithNiveau1(std::string level1Directory){

   GDCM_NAME_SPACE::DirList dirList(level1Directory, false, true); // DON'T get recursively the list of files

   GDCM_NAME_SPACE::DirListType fileNames;
   fileNames = dirList.GetFilenames();
   // -----------------------------------------------------
   // Iterate to ALL the objets(files/directories) found in the input directory
   // -----------------------------------------------------
   GDCM_NAME_SPACE::DirListType::iterator it;
   
   for (it = fileNames.begin();
        it != fileNames.end();
      ++it)
   {
      if ( ! GDCM_NAME_SPACE::DirList::IsDirectory(*it) )
      { 
         std::cout << "--- [" << *it << "] is a file" << std::endl;
      }
      
   }
   
    for (it = fileNames.begin();
         it != fileNames.end();
       ++it)
   {
      if ( GDCM_NAME_SPACE::DirList::IsDirectory(*it) )
      { 
         std::cout << "--- [" << *it << "] is a directory" << std::endl;
         DealWithNiveau2(*it);
      }
   }
}


void DealWithNiveau2(std::string level2Directory){

   GDCM_NAME_SPACE::DirList dirList(level2Directory, false, true); // DON'T get recursively the list of files

   GDCM_NAME_SPACE::DirListType fileNames;
   fileNames = dirList.GetFilenames();

   // -----------------------------------------------------
   // Iterate to ALL the objets(files/directories) found in the input directory
   // -----------------------------------------------------
   GDCM_NAME_SPACE::DirListType::iterator it;
   
   for (it = fileNames.begin();
        it != fileNames.end();
      ++it)
   {
      if ( ! GDCM_NAME_SPACE::DirList::IsDirectory(*it) )
      { 
         std::cout << "--- --- [" << *it << "] is a file" << std::endl;
      }
      
   }
   
   for (it = fileNames.begin();
         it != fileNames.end();
       ++it)
   {
      if ( GDCM_NAME_SPACE::DirList::IsDirectory(*it) )
      { 
         std::cout << "--- --- [" << *it << "] is a directory" << std::endl;
         DealWithNiveau3(*it);
      }
    }
}


void DealWithNiveau3(std::string level3Directory){

   GDCM_NAME_SPACE::DirList dirList(level3Directory, false, true); // DON'T get recursively the list of files

   GDCM_NAME_SPACE::DirListType fileNames;
   fileNames = dirList.GetFilenames();

   // -----------------------------------------------------
   // Iterate fo ALL the directories found in the input directory
   // -----------------------------------------------------

   for (GDCM_NAME_SPACE::DirListType::iterator it = fileNames.begin();
                                    it != fileNames.end();
                                  ++it)
   {

      if ( ! GDCM_NAME_SPACE::DirList::IsDirectory(*it) )
      {
         std::cout << "--- --- --- [" << *it << "] is a file" << std::endl;
      }
      else
      {
         std::cout << "--- --- --- [" << *it << "] is a directory" << std::endl;
      
      }
    }
}
