/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestCopyDicom.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/21 11:40:52 $
  Version:   $Revision: 1.20 $
                                                                                
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

// Here we load a gdcmFile and then try to create from scratch a copy of it,
// copying field by field the dicom image

int main(int argc, char* argv[])
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
         std::cerr << "Don't try to cheat, I am removing the file anyway" << std::endl;
         if( !RemoveFile( output.c_str() ) )
         {
            std::cerr << "Ouch, the file exist, but I cannot remove it" << std::endl;
            return 1;
         }
      }
      gdcm::FileHelper *original = new gdcm::FileHelper( filename );
   
      std::cout << "--- Original ----------------------" << std::endl;
      //original->GetFile()->Print();
   
      gdcm::FileHelper *copy = new gdcm::FileHelper( output );

      size_t dataSize = original->GetImageDataSize();
      uint8_t* imageData = original->GetImageData();
      (void)imageData;
      (void)dataSize;
  
      //First of all copy the header field by field
  
      // Warning :Accessor gdcmElementSet::GetEntry() should not exist 
      // It was commented out by Mathieu, that was a *good* idea
      // (the user does NOT have to know the way we implemented the File !)
      // Waiting for a 'clean' solution, I keep the method ...JPRx

      gdcm::DocEntry* d=original->GetFile()->GetFirstEntry();
      while(d)
      {
         if ( gdcm::BinEntry* b = dynamic_cast<gdcm::BinEntry*>(d) )
         {              
            copy->GetFile()->ReplaceOrCreate( 
                                 b->GetBinArea(),
                                 b->GetLength(),
                                 b->GetGroup(), 
                                 b->GetElement(),
                                 b->GetVR() );
         }
         else if ( gdcm::ValEntry* v = dynamic_cast<gdcm::ValEntry*>(d) )
         {   
            copy->GetFile()->ReplaceOrCreate( 
                                 v->GetValue(),
                                 v->GetGroup(), 
                                 v->GetElement(),
                                 v->GetVR() ); 
         }
         else
         {
          // We skip pb of SQ recursive exploration
          //std::cout << "Skipped Sequence " 
          //          << "------------- " << d->GetVR() << " "<< std::hex
          //          << d->GetGroup() << " " << d->GetElement()
          //  << std::endl;    
         }

         d=original->GetFile()->GetNextEntry();
      }

      //copy->GetImageData();
      //copy->SetImageData(imageData, dataSize);

      std::cout << "--- Copy ----------------------" << std::endl;
      std::cout <<std::endl << "DO NOT care about Offset"  <<std::endl<<std::endl;; 
      copy->GetFile()->Print();
      std::cout << "--- ---- ----------------------" << std::endl;
   
      copy->WriteDcmExplVR( output );

      return 0;
}



