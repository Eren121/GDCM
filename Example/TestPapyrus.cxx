/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestPapyrus.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/01 10:34:59 $
  Version:   $Revision: 1.12 $
                                                                                
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

//#include <fstream>

#ifndef _WIN32
#include <unistd.h> //for access, unlink
#else
#include <io.h> //for _access
#endif

// return true if the file exists
bool FileExists(const char* filename)
{
#ifdef _MSC_VER
# define access _access
#endif
#ifndef R_OK
# define R_OK 04
#endif
  if ( access(filename, R_OK) != 0 )
    {
    return false;
    }
  else
    {
    return true;
    }
}

bool RemoveFile(const char* source)
{
#ifdef _MSC_VER
#define _unlink unlink
#endif
  return unlink(source) != 0 ? false : true;
}

// ----------------------------------------------------------------------
// Here we load a supposed to be Papyrus File (gdcm::File compliant)
// and then try to get the pixels, using low-level SeqEntry accessors.
// Since it's not a general purpose Papyrus related program
// (just a light example) we suppose *everything* is clean
// and we don't perform any integrity check
// ----------------------------------------------------------------------

int main(int argc, char* argv[])
{
   if (argc < 3)
   {
      std::cerr << "Usage :" << std::endl << 
      argv[0] << " input_papyrus output_dicom verbose" << std::endl;
      return 1;
   }

   std::string filename = argv[1];
   std::string output = argv[2];

   if (argc > 3)
      gdcm::Debug::DebugOn();

   if( FileExists( output.c_str() ) )
   {
      if( !RemoveFile( output.c_str() ) )
      {
         std::cerr << "Ouch, the file exist, but I cannot remove it" << std::endl;
         return 1;
      }
   }
   gdcm::FileHelper *original = new gdcm::FileHelper( filename );
   gdcm::File *h = original->GetFile();

   // Look for private Papyrus Sequence
   gdcm::SeqEntry *seqPapyrus= h->GetSeqEntry(0x0041, 0x1050);
   if (!seqPapyrus)
   {
      std::cout << "NOT a Papyrus File" << std::endl;
      delete h;
      return 1;
   }

   gdcm::SQItem *sqi = seqPapyrus->GetFirstSQItem();
   if (sqi == 0)
   {
      std::cout << "NO SQItem found within private Papyrus Sequence"
          << std::endl;
      delete h;
      return 1;
   }
      
   std::string TransferSyntax;
   std::string StudyDate;
   std::string StudyTime;
   std::string Modality;
   std::string PatientName;
   std::string MediaStSOPinstUID;

// Get informations on the file : 
//  Modality, Transfer Syntax, Study Date, Study Time
// Patient Name, Media Storage SOP Instance UID, etc

   MediaStSOPinstUID   =   h->GetEntryValue(0x0002,0x0002);
   TransferSyntax      =   h->GetEntryValue(0x0002,0x0010);
   StudyDate           = sqi->GetEntryValue(0x0008,0x0020);
   StudyTime           = sqi->GetEntryValue(0x0008,0x0030);
   Modality            = sqi->GetEntryValue(0x0008,0x0060);
   PatientName         = sqi->GetEntryValue(0x0010,0x0010);

   std::cout << "TransferSyntax " << TransferSyntax << std::endl;

   std::string Rows;
   std::string Columns;
   std::string SamplesPerPixel;
   std::string BitsAllocated;
   std::string BitsStored;
   std::string HighBit;
   std::string PixelRepresentation;
   

   // we brutally suppose all the images within a Papyrus file
   // have the same caracteristics.
   // if you're aware they have not, just move the GetEntryValue
   // inside the loop

   // Get caracteristics of the first image
   SamplesPerPixel     = sqi->GetEntryValue(0x0028,0x0002);
   Rows                = sqi->GetEntryValue(0x0028,0x0010);
   Columns             = sqi->GetEntryValue(0x0028,0x0011);
   BitsAllocated       = sqi->GetEntryValue(0x0028,0x0100);
   BitsStored          = sqi->GetEntryValue(0x0028,0x0101);
   HighBit             = sqi->GetEntryValue(0x0028,0x0102);
   PixelRepresentation = sqi->GetEntryValue(0x0028,0x0102);

   // just convert those needed to compute PixelArea length
   int iRows            = (uint32_t) atoi( Rows.c_str() );
   int iColumns         = (uint32_t) atoi( Columns.c_str() );
   int iSamplesPerPixel = (uint32_t) atoi( SamplesPerPixel.c_str() );
   int iBitsAllocated   = (uint32_t) atoi( BitsAllocated.c_str() );

   int lgrImage = iRows*iColumns * iSamplesPerPixel * (iBitsAllocated/8);

   // compute number of images
   int nbImages = seqPapyrus->GetNumberOfSQItems();
   std::cout <<"Number of frames :" << nbImages << std::endl;  

   //  allocate enough room to get the pixels of all images.
   uint8_t *PixelArea = new uint8_t[lgrImage*nbImages];
   uint8_t *currentPosition = PixelArea;
  gdcm::BinEntry *pixels;

   // declare and open the file
   std::ifstream *Fp;
   Fp = new std::ifstream(filename.c_str(), std::ios::in | std::ios::binary);
   if( ! *Fp )
   {
      std::cout <<  "Cannot open file: " << filename << std::endl;
      //gdcmDebugMacro( "Cannot open file: " << filename.c_str() );
      delete Fp;
      Fp = 0;
      return 0;
   }
   // to be sure to be at the beginning
   Fp->seekg(0,  std::ios::end);

   uint32_t offset;
   std::string previousRows = Rows;
   sqi = seqPapyrus->GetFirstSQItem();
   while (sqi)
   {
      std::cout << "One more image read. Keep waiting" << std::endl;
      Rows = sqi->GetEntryValue(0x0028,0x0010);
      // minimum integrity check
      if (Rows != previousRows)
      {
         std::cout << "Consistency check failed " << std::endl;
         return 1;
      }
      // get the images pixels
      pixels = sqi->GetBinEntry(0x7fe0,0x0010);
      offset = pixels->GetOffset();
      // perform a fseek, on offset length on the 'right' length
      Fp->seekg(offset, std::ios::beg);
      // perform a fread into the right place
      Fp->read((char *)currentPosition, (size_t)lgrImage);
      currentPosition +=lgrImage;

      std::string previousRowNb = Rows;

      sqi =  seqPapyrus->GetNextSQItem();
   }

   // build up a new File, with file info + images info + global pixel area.

   std::string NumberOfFrames = gdcm::Util::Format("%d", nbImages); 

   gdcm::File *n = new gdcm::File();

   n->InsertValEntry(MediaStSOPinstUID,  0x0002,0x0002);
  // Whe keep default gdcm Transfer Syntax (Explicit VR Little Endian)
  // since using Papyrus one (Implicit VR Little Endian) is a mess
   //n->InsertValEntry(TransferSyntax,     0x0002,0x0010);
   n->InsertValEntry(StudyDate,          0x0008,0x0020);
   n->InsertValEntry(StudyTime,          0x0008,0x0030);
   n->InsertValEntry(Modality,           0x0008,0x0060);
   n->InsertValEntry(PatientName,        0x0010,0x0010);

   n->InsertValEntry(SamplesPerPixel,    0x0028,0x0002);
   n->InsertValEntry(NumberOfFrames,     0x0028,0x0008);
   n->InsertValEntry(Rows,               0x0028,0x0010);
   n->InsertValEntry(Columns,            0x0028,0x0011);
   n->InsertValEntry(BitsAllocated,      0x0028,0x0100);
   n->InsertValEntry(BitsStored,         0x0028,0x0101);
   n->InsertValEntry(HighBit,            0x0028,0x0102);
   n->InsertValEntry(PixelRepresentation,0x0028,0x0102);

   // create the file
   gdcm::FileHelper *file = new gdcm::FileHelper(n);

   file->SetImageData(PixelArea,lgrImage*nbImages);
   file->SetWriteTypeToDcmExplVR();

   //file->SetPrintLevel(2);
   file->Print();

   // Write the file
   file->Write(argv[2]); 
   if (!file)
   {
      std::cout <<"Fail to open (write) file:[" << argv[2]<< "]" << std::endl;;
      return 1;  
   }
   return 0;
}
