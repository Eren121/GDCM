/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestCopyDicom.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/18 08:35:43 $
  Version:   $Revision: 1.31 $
                                                                                
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
#include "gdcmDataEntry.h"

#ifndef _WIN32
#include <unistd.h> //for access, unlink
#else
#include <io.h> //for _access
#endif

// return true if the file exists
bool FileExists(const char *filename)
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

bool RemoveFile(const char *source)
{
#ifdef _MSC_VER
#define _unlink unlink
#endif
  return unlink(source) != 0 ? false : true;
}

// Here we load a gdcmFile and then try to create from scratch a copy of it,
// copying field by field the dicom image

int main(int argc, char *argv[])
{
   if (argc < 3)
   {
      std::cerr << "Usage :" << std::endl << 
      argv[0] << " input_dicom output_dicom" << std::endl;
      return 1;
   }

// don't modify identation in order to let this source xdiffable with ../Test

      std::string filename = argv[1];
      std::string output = argv[2];

      if( FileExists( output.c_str() ) )
      {
         std::cerr << "Don't try to cheat, I am removing the file anyway" 
                   << std::endl;
         if( !RemoveFile( output.c_str() ) )
         {
            std::cerr << "Ouch, the file exist, but I cannot remove it" 
                      << std::endl;
            return 1;
         }
      }
      gdcm::File *fileOr = new gdcm::File();
      fileOr->SetFileName( filename );
      fileOr->Load();
      gdcm::FileHelper *original = new gdcm::FileHelper( fileOr );
   
      std::cout << "--- Original ----------------------" << std::endl;
      //original->GetFile()->Print();
   
      gdcm::FileHelper *copy = new gdcm::FileHelper( );
      copy->SetFileName( output );
      copy->Load();

      //size_t dataSize;
      uint8_t *imageData;
      //dataSize = original->GetImageDataSize();// just an accesor :useless here
      
      imageData = original->GetImageData(); // VERY important : 
                                      // brings pixels into memory !
      //(void)imageData; // not enough to avoid warning with icc compiler
      //(void)dataSize; //  not enough to avoid warning on 'Golgot'
  
      std::cout << imageData << std::endl; // to avoid warning ?

      //First of all copy the header field by field

      gdcm::DocEntry *d = original->GetFile()->GetFirstEntry();
      while(d)
      {
         if ( gdcm::DataEntry *de = dynamic_cast<gdcm::DataEntry *>(d) )
         {              
            copy->GetFile()->InsertEntryBinArea( de->GetBinArea(),de->GetLength(),
                                                 de->GetGroup(),de->GetElement(),
                                                 de->GetVR() );
         }
         else
         {
          // We skip pb of SQ recursive exploration
          std::cout << "Skipped Sequence " 
                    << "------------- " << d->GetVR() << " "<< std::hex
                    << d->GetGroup() << "," << d->GetElement()
                    << std::endl;    
         }

         d=original->GetFile()->GetNextEntry();
      }

      //copy->GetImageData();
      //copy->SetImageData(imageData, dataSize);

      std::cout << "--- Copy ----------------------" << std::endl;
      std::cout <<std::endl << "DO NOT care about Offset"  
                <<std::endl << std::endl;; 
      copy->GetFile()->Print();
      std::cout << "--- ---- ----------------------" << std::endl;
   
      copy->WriteDcmExplVR( output );
      

      delete fileOr;   // File
      delete original; // FileHelper
      delete copy;     // FileHelper
      return 0;
}



