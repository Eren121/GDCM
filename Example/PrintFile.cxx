/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PrintFile.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/23 13:38:17 $
  Version:   $Revision: 1.73 $
                                                                                
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

#include "gdcmFileHelper.h"
#include "gdcmDebug.h"
#include "gdcmDirList.h"
#include "gdcmGlobal.h"
#include "gdcmDictSet.h"
#include "gdcmArgMgr.h"
#include "gdcmOrientation.h"
#include <iostream>

// TODO : code factorization, for 'single file' an 'whole directory' processing

void ShowLutData(gdcm::File *f);

     // Nothing is written yet to get LUT Data user friendly
     // The following is to be moved into a PixelReadConvert method
     // Let here, waiting for a clever idea on the way to do it.

void ShowLutData(gdcm::File *f)
{  
   gdcm::SeqEntry *modLutSeq = f->GetSeqEntry(0x0028,0x3000);
   if ( modLutSeq !=0 )
   {
      gdcm::SQItem *sqi= modLutSeq->GetFirstSQItem();
      if ( sqi != 0 )
      {
         std::string lutDescriptor = sqi->GetEntryString(0x0028,0x3002);
         if (   /*lutDescriptor   == GDCM_UNFOUND*/ 0 )
         {
            //gdcmWarningMacro( "LUT Descriptor is missing" );
            std::cout << "LUT Descriptor is missing" << std::endl;
            return;
         }
         int length;   // LUT length in Bytes
         int deb;      // Subscript of the first Lut Value
         int nbits;    // Lut item size (in Bits)

         int nbRead;    // nb of items in LUT descriptor (must be = 3)

         nbRead = sscanf( lutDescriptor.c_str(),
                           "%d\\%d\\%d",
                              &length, &deb, &nbits );
         std::cout << "length " << length 
                  << " deb " << deb 
                  << " nbits " << nbits
                  << std::endl;
         if ( nbRead != 3 )
         {
               //gdcmWarningMacro( "Wrong LUT descriptor" );
               std::cout << "Wrong LUT descriptor" << std::endl;
         }
         //LUT Data (CTX dependent)    
         gdcm::DataEntry *b = sqi->GetDataEntry(0x0028,0x3006); 
         if ( b != 0 )
         { 
            int BitsAllocated = f->GetBitsAllocated();
            if ( BitsAllocated <= 8 )
            { 
               int mult;
               if ( ( nbits == 16 ) && ( BitsAllocated == 8 ) )
               {
               // when LUT item size is different than pixel size
                  mult = 2; // high byte must be = low byte
               }
               else
               {
               // See PS 3.3-2003 C.11.1.1.2 p 619
                  mult = 1;
               }
               uint8_t *lut = b->GetBinArea();
               for( int i=0; i < length; ++i )
               {
                  std::cout << i+deb << " : \t"
                              << (int) (lut[i*mult + 1]) << std::endl;
               }
            }
            else
            {
               uint16_t *lut = (uint16_t *)(b->GetBinArea());  
               for( int i=0; i < length; ++i )
               {
                  std::cout << i+deb << " : \t"
                              << (int) (((uint16_t *)lut)[i])
                              << std::endl;
               }             
            }
         }  
         else
            std::cout << "No LUT Data DataEntry (0x0028,0x3006) found?!? " 
                        << std::endl;
      }
      else
         std::cout << "No First SQ Item within (0x0028,0x3000) ?!? " 
                     << std::endl;      
   }
   else
      std::cout << "No LUT Data SeqEntry (0x0028,0x3000) found " 
                  << std::endl;
}

int main(int argc, char *argv[])
{

   START_USAGE(usage)
   " \n PrintFile : \n                                                        ",
   " Display the header of a ACR-NEMA/PAPYRUS/DICOM File                      ",
   " usage: PrintFile {filein=inputFileName|dirin=inputDirectoryName}[level=n]",
   "                       [forceload=listOfElementsToForceLoad]              ",
   "                       [dict= privateDirectory]                           ",
   "                       [ { [noshadowseq] | [noshadow][noseq] } ]          ",
   "                       [debug] [warning]                                  ",
   "      level = 0,1,2 : depending on the amount of details user wants to see",
   "      listOfElementsToForceLoad : group-elem,g2-e2,... (in hexa, no space)",
   "                                of Elements to load whatever their length ",
   "      privateDirectory : source file full path name of Shadow Group elems ",
   "      noshadowseq: user doesn't want to load Private Sequences            ",
   "      noshadow   : user doesn't want to load Private groups (odd number)  ",
   "      noseq      : user doesn't want to load Sequences                    ",
   "      debug      : user wants to run the program in 'debug mode'          ",
   "      warning    : user wants to be warned about any oddity in the File   ",
   "      showlut :user wants to display the Palette Color (as an int array)  ",
   FINISH_USAGE

   // Initialize Arguments Manager   
   gdcm::ArgMgr *am= new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage") )
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 1;
   }

   char *fileName = am->ArgMgrGetString("filein",(char *)0);
   char *dirName  = am->ArgMgrGetString("dirin",(char *)0);

   if ( (fileName == 0 && dirName == 0) ||
        (fileName != 0 && dirName != 0) )
   {
      std::cerr << std::endl
        << "Either 'filein=' or 'dirin=' must be present;" 
        << std::endl << "Not both" << std::endl;
      am->ArgMgrUsage(usage); // Display 'usage'  
      delete am;
      return 1;
   }

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();

   if (am->ArgMgrDefined("warning"))
      gdcm::Debug::WarningOn();
       
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

   int level = am->ArgMgrGetInt("level", 1);

   int forceLoadNb;
   uint16_t *elemsToForceLoad 
                           = am->ArgMgrGetXInt16Enum("forceload", &forceLoadNb);

   bool showlut = ( 0 != am->ArgMgrDefined("SHOWLUT") );

   bool ddict = am->ArgMgrDefined("dict") ? true : false;
   char *dict = 0;

   if (ddict)
   {
     dict = am->ArgMgrGetString("dict",(char *)0);
   }

   /* if unused Param we give up */
   if ( am->ArgMgrPrintUnusedLabels() )
   {
      am->ArgMgrUsage(usage);
      delete am;
      return 1;
   } 

   delete am;  // we don't need Argument Manager any longer

   // ----------- End Arguments Manager ---------

 
   if (ddict)
   {
      gdcm::Global::GetDicts()->GetDefaultPubDict()->AddDict(dict);   
   }

   if ( fileName != 0 ) // ====== Deal with a single file ======
   { 
      gdcm::File *f = gdcm::File::New();
      f->SetLoadMode(loadMode);
      f->SetFileName( fileName );

      for (int ri=0; ri<forceLoadNb; ri++)
      {
         f->AddForceLoadElement((uint32_t)elemsToForceLoad[2*ri], 
                                (uint32_t)elemsToForceLoad[2*ri+1] ); 
      }

      bool res = f->Load();
      // gdcm::File::IsReadable() is no usable here, because we deal with
      // any kind of gdcm-Parsable *document* 
      // not only gdcm::File (as opposed to gdcm::DicomDir)
      if ( !res )
      {
         std::cout << "Cannot process file [" << fileName << "]" << std::endl;
         std::cout << "Either it doesn't exist, or it's read protected " 
                   << std::endl;
         std::cout << "or it's not a Dicom File, or its 'header' is bugged" 
                   << std::endl;
         std::cout << "use 'PrintFile filein=... debug' to try to guess the pb"
                   << std::endl;
         f->Delete();
         return 0;
      }

      gdcm::FileHelper *fh = gdcm::FileHelper::New(f);
      fh->SetPrintLevel( level );

      fh->Print();

      std::cout << "\n\n" << std::endl; 

      std::cout <<std::endl;
      std::cout <<" dataSize    " << fh->GetImageDataSize()    << std::endl;
      std::cout <<" dataSizeRaw " << fh->GetImageDataRawSize() << std::endl;

      int nX,nY,nZ,sPP,planarConfig;
      std::string pixelType;
      nX=f->GetXSize();
      nY=f->GetYSize();
      nZ=f->GetZSize();
      std::cout << " DIMX=" << nX << " DIMY=" << nY << " DIMZ=" << nZ 
                << std::endl;

      pixelType    = f->GetPixelType();
      sPP          = f->GetSamplesPerPixel();
      std::cout << " pixelType= ["            << pixelType 
                << "] SamplesPerPixel= ["     << sPP
                << "] ";

      if (sPP == 3)
      {
         planarConfig = f->GetPlanarConfiguration();
         std::cout << " PlanarConfiguration= [" << planarConfig 
                << "] "<< std::endl;
      } 
      std::cout << " PhotometricInterpretation= [" 
                << f->GetEntryString(0x0028,0x0004)
                << "] "<< std::endl;

      int numberOfScalarComponents=f->GetNumberOfScalarComponents();
      std::cout << " NumberOfScalarComponents = " << numberOfScalarComponents 
                <<std::endl
                << " LUT = " << (f->HasLUT() ? "TRUE" : "FALSE")
                << std::endl;

      if ( f->GetDataEntry(0x0002,0x0010) )
         if ( f->GetDataEntry(0x0002,0x0010)->IsNotLoaded() ) 
         {
            std::cout << "Transfer Syntax not loaded. " << std::endl
                     << "Better you increase MAX_SIZE_LOAD_ELEMENT_VALUE"
                  << std::endl;
            f->Delete();
            return 0;
         }
  
      std::string transferSyntaxName = f->GetTransferSyntaxName();
      std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" 
                << std::endl;
      std::cout << " SwapCode= " << f->GetSwapCode() << std::endl;
      std::cout << " ------" << std::endl;

      //std::cout << "\n\n" << std::endl; 
      //std::cout << "X spacing " << f->GetXSpacing() << std::endl;
      //std::cout << "Y spacing " << f->GetYSpacing() << std::endl;
      //std::cout << "Z spacing " << f->GetZSpacing() << std::endl;

//------------------------------
      // Lets's get and print some usefull fields about 'Orientation'
      // ------------------------------------------------------------

      std::string strPatientPosition = 
                                      f->GetEntryString(0x0018,0x5100);
      if ( strPatientPosition != gdcm::GDCM_UNFOUND 
        && strPatientPosition != "" )  
            std::cout << "PatientPosition (0x0010,0x5100)= [" 
                      << strPatientPosition << "]" << std::endl;
 
      std::string strViewPosition = 
                                      f->GetEntryString(0x0018,0x5101);
      if ( strViewPosition != gdcm::GDCM_UNFOUND 
        && strViewPosition != "" )  
            std::cout << "strViewPosition (0x0010,0x5101)= [" 
                      << strViewPosition << "]" << std::endl;
      
     std::string strPatientOrientation = 
                                      f->GetEntryString(0x0020,0x0020);
      if ( strPatientOrientation != gdcm::GDCM_UNFOUND
        && strPatientOrientation != "")  
         std::cout << "PatientOrientation (0x0020,0x0020)= [" 
                   << strPatientOrientation << "]" << std::endl;

      std::string strImageOrientationPatient = 
                                      f->GetEntryString(0x0020,0x0037);  
      if ( strImageOrientationPatient != gdcm::GDCM_UNFOUND
        && strImageOrientationPatient != "" )  
         std::cout << "ImageOrientationPatient (0x0020,0x0037)= [" 
                   << strImageOrientationPatient << "]" << std::endl;

      std::string strImageOrientationRET = 
                                      f->GetEntryString(0x0020,0x0035);
      if ( strImageOrientationRET != gdcm::GDCM_UNFOUND
        && strImageOrientationRET != "" )  
         std::cout << "ImageOrientationRET (0x0020,0x0035)= [" 
                   << strImageOrientationRET << "]" << std::endl;

      // Let's compute 'user friendly' results about 'Orientation'
      // ---------------------------------------------------------
 
      gdcm::Orientation *o = gdcm::Orientation::New();

      if ( strImageOrientationPatient != gdcm::GDCM_UNFOUND ||
           strImageOrientationRET     != gdcm::GDCM_UNFOUND )
      {
  
         gdcm::OrientationType orient = o->GetOrientationType( f );
 
         std::cout << "TypeOrientation = " << orient << " (-> " 
                   << o->GetOrientationTypeString(orient) << " )" << std::endl;
      }

      std::string ori = o->GetOrientation ( f );
      if (ori != "\\" )
         std::cout << "Orientation [" << ori << "]" << std::endl;
 
       o->gdcm::Orientation::Delete(); 
//------------------------------


      // Display the LUT as an int array (for debugging purpose)
      if ( f->HasLUT() && showlut )
      {
         uint8_t* lutrgba = fh->GetLutRGBA();
         if ( lutrgba == 0 )
         {
            std::cout << "Lut RGBA (Palette Color) not built " << std::endl;
 
           // Nothing is written yet to get LUT Data user friendly
           // The following is to be moved into a PixelRedaConvert method
  
            gdcm::SeqEntry *modLutSeq = f->GetSeqEntry(0x0028,0x3000);
            if ( modLutSeq !=0 )
            {
               gdcm::SQItem *sqi= modLutSeq->GetFirstSQItem();
               if ( !sqi )
               {
                  std::string lutDescriptor = sqi->GetEntryString(0x0028,0x3002);
                  int length;   // LUT length in Bytes
                  int deb;      // Subscript of the first Lut Value
                  int nbits;    // Lut item size (in Bits)
                  int nbRead;   // nb of items in LUT descriptor (must be = 3)

                  nbRead = sscanf( lutDescriptor.c_str(),
                                    "%d\\%d\\%d",
                                     &length, &deb, &nbits );
                  if ( nbRead != 3 )
                  {
                      //gdcmWarningMacro( "Wrong LUT descriptor" );
                      std::cout << "Wrong LUT descriptor" << std::endl;
                  }                                                  
                  gdcm::DataEntry *b = sqi->GetDataEntry(0x0028,0x3006);
                  if ( b != 0 )
                  {
                     if ( b->GetLength() != 0 )
                     {
                        std::cout << "---------------------------------------"
                               << " We should never reach this point      "
                               << std::endl;
                        //LoadEntryBinArea(b);    //LUT Data (CTX dependent)
                     }   
                 }
              }      
            }
            else
               std::cout << "No LUT Data (0x0028,0x3000) found " << std::endl;
        }
         else
         {
            if ( fh->GetLutItemSize() == 8 )
            {
               for (int i=0;i<fh->GetLutItemNumber();i++)
                  std::cout << i << " : \t"
                         << (int)(lutrgba[i*4])   << " "
                         << (int)(lutrgba[i*4+1]) << " "
                         << (int)(lutrgba[i*4+2]) << std::endl;
            }
            else // LutItemSize assumed to be = 16
            {
               uint16_t* lutrgba16 = (uint16_t*)lutrgba;
               for (int i=0;i<fh->GetLutItemNumber();i++)
                  std::cout << i << " : \t"
                         << (int)(lutrgba16[i*4])   << " "
                         << (int)(lutrgba16[i*4+1]) << " "
                         << (int)(lutrgba16[i*4+2]) << std::endl;
            }
         }
      }
      else if (showlut)
      {
         std::cout << "Try LUT Data "<< std::endl;
         ShowLutData(f);
      }

      // Parsability of the gdcm::Document already checked, after Load() !
      
      if ( f->IsReadable() )
      {
         std::cout <<std::endl<<fileName<<" is Readable"<<std::endl;
      }
      else if ( f->GetSeqEntry(0x0041,0x1010) )
      {
         std::cout <<std::endl<<fileName<<" looks like a 'PAPYRUS image' file"
                   <<std::endl;
      }
      else if ( f->GetSeqEntry(0x0004,0x1220) )
      {
         std::cout <<std::endl<<fileName<<" looks like a 'DICOMDIR file'"
                   <<std::endl;
      }
      else 
      {
         std::cout <<std::endl<<fileName<<" doesn't look like an image file "
             <<std::endl; 
      }
 
      std::cout<<std::flush;
      f->Delete();
      fh->Delete();
   }
   else  // ====== Deal with a Directory ======
   {
      std::cout << "dirName [" << dirName << "]" << std::endl;
      gdcm::DirList dirList(dirName,1); // gets recursively the file list
      gdcm::DirListType fileList = dirList.GetFilenames();
      gdcm::File *f;
      bool res;

      for( gdcm::DirListType::iterator it  = fileList.begin();
                                 it != fileList.end();
                                 ++it )
      {
         std::cout << std::endl<<" Start processing :[" << it->c_str() << "]"
                   << std::endl;
         f = gdcm::File::New();
         f->SetLoadMode(loadMode);
         f->SetFileName( it->c_str() );

         for (int ri=0; ri<forceLoadNb; ri++)
         {
            printf("%04x,%04x\n",elemsToForceLoad[2*ri], 
                                 elemsToForceLoad[2*ri+1]);
            f->AddForceLoadElement((uint32_t)elemsToForceLoad[2*ri], 
                                   (uint32_t)elemsToForceLoad[2*ri+1]); 
         }
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

         gdcm::FileHelper *fh = gdcm::FileHelper::New(f);
         fh->SetPrintLevel( level );
         fh->Print();

//------------------------------

         // Lets's get and print some usefull fields about 'Orientation'
         // ------------------------------------------------------------

         std::string strPatientPosition = 
                                       f->GetEntryString(0x0018,0x5100);
         if ( strPatientPosition != gdcm::GDCM_UNFOUND 
         && strPatientPosition != "" )  
               std::cout << "PatientPosition (0x0010,0x5100)= [" 
                        << strPatientPosition << "]" << std::endl;
    
         std::string strViewPosition = 
                                       f->GetEntryString(0x0018,0x5101);
         if ( strViewPosition != gdcm::GDCM_UNFOUND 
         && strViewPosition != "" )  
               std::cout << "strViewPosition (0x0010,0x5101)= [" 
                        << strViewPosition << "]" << std::endl;
         
         std::string strPatientOrientation = 
                                       f->GetEntryString(0x0020,0x0020);
         if ( strPatientOrientation != gdcm::GDCM_UNFOUND
         && strPatientOrientation != "")  
            std::cout << "PatientOrientation (0x0020,0x0020)= [" 
                     << strPatientOrientation << "]" << std::endl;

         std::string strImageOrientationPatient = 
                                       f->GetEntryString(0x0020,0x0037);  
         if ( strImageOrientationPatient != gdcm::GDCM_UNFOUND
         && strImageOrientationPatient != "" )  
            std::cout << "ImageOrientationPatient (0x0020,0x0037)= [" 
                     << strImageOrientationPatient << "]" << std::endl;

         std::string strImageOrientationRET = 
                                       f->GetEntryString(0x0020,0x0035);
         if ( strImageOrientationRET != gdcm::GDCM_UNFOUND
         && strImageOrientationRET != "" )
         {
            std::cout << "ImageOrientationRET (0x0020,0x0035)= [" 
                     << strImageOrientationRET << "]" << std::endl;
         }

         // Let's compute 'user friendly' results about 'Orientation'
         // ---------------------------------------------------------
    
         gdcm::Orientation *o = gdcm::Orientation::New(); 


         if ( strImageOrientationPatient != gdcm::GDCM_UNFOUND ||
            strImageOrientationRET     != gdcm::GDCM_UNFOUND )
         {
     
            gdcm::OrientationType orient = o->GetOrientationType( f );
    
            std::cout << "TypeOrientation = " << orient << " (-> " 
                     << o->GetOrientationTypeString(orient) << " )" << std::endl;
         }

         std::string ori = o->GetOrientation ( f );
         if (ori != "\\" )
            std::cout << "Orientation [" << ori << "]" << std::endl;
   
         o->gdcm::Orientation::Delete(); 

//------------------------------- 
        
         if (f->IsReadable())
            std::cout <<std::endl<<it->c_str()<<" is Readable"<<std::endl;
         else
            std::cout <<std::endl<<it->c_str()<<" is NOT Readable"<<std::endl;
         std::cout << "\n\n" << std::endl;
         f->Delete();
         fh->Delete();
      }
      std::cout<<std::flush;
   }
   return 0;
}
