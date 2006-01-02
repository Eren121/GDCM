/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exCTPET.cxx,v $
  Language:  C++
  Date:      $Date: 2006/01/02 21:54:23 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmSerieHelper.h"
#include "gdcmDirList.h"
#include "gdcmUtil.h"
#include "gdcmDataEntry.h"

int main(int argc, char *argv[])
{
  if(argc < 2 )
    {
    std::cerr << "RTFM" << std::endl;
    return 1;
    }

  // Get the directory name
  const char *directory = argv[1];
#if 0
  gdcm::SerieHelper *s;
   std::cout << "Dir Name :[" << directory << "]" << std::endl;

   s = gdcm::SerieHelper::New();
   s->SetLoadMode(gdcm::LD_NOSHADOW | gdcm::LD_NOSEQ);
   // 0008|0060 [CS] [Modality] [CT]   
   gdcm::TagKey modality(0x0008,0x0060);
   s->AddRestriction(modality, "CT", gdcm::GDCM_EQUAL); // Keep only files where
                                              // restriction is true
   s->SetDirectory(directory, true); // true : recursive exploration

//   std::cout << " ---------------------------------------- Finish parsing :["
//             << directory << "]" << std::endl;
//
//   s->Print();
//   std::cout << " ---------------------------------------- Finish printing (1)"
//             << std::endl;

   int nbFiles;
   // For all the Single SerieUID Files Sets of the gdcm::Serie
   gdcm::FileList *l = s->GetFirstSingleSerieUIDFileSet();
   while (l)
   { 
      nbFiles = l->size() ;
      //if ( l->size() > 3 ) // Why not ? Just an example, for testing
      {
         //std::cout << "Sort list : " << nbFiles << " long" << std::endl;
         s->OrderFileList(l);  // sort the list
      }
      l = s->GetNextSingleSerieUIDFileSet();
   } 
  s->Delete();
#endif

  // Open a file A
  // Open another file B
  // Same Serie/Study ?
  // No -> return
  // Yes -> continue
  // Same Frame of Reference
  // No -> Return
  // Yes -> continue
  // A is CT and B is PT (PET)
  // No -> return
  // Yes -> continue
  // Same Image Position (no string comparison !!)
  // Even if floating point comparison are dangerous, string comp will not work in general case
  // No -> Return
  // Yes: We found a match

  const char filename1[] = "/tmp/PETCT/case1/WHOLEB001_CT001.dcm";
  const char filename2[] = "/tmp/PETCT/case1/CT_PET001_CT100.dcm";
  gdcm::File *fileRef = gdcm::File::New();
  fileRef->SetFileName( filename2 );
  fileRef->Load();
  // 0008 0060 CS 1 Modality
  std::string modalityRef = fileRef->GetEntryString(0x0008,0x0060);
  if( modalityRef == gdcm::GDCM_UNFOUND ) return 1;
  if ( !gdcm::Util::DicomStringEqual(modalityRef, "CT") ) return 1;
    // 0020 000d UI 1 Study Instance UID
    // 0020 000e UI REL Series Instance UID
  std::string series_uid_ref = fileRef->GetEntryString(0x0020, 0x000e);
  // 0020 0052 UI 1 Frame of Reference UID
  std::string frame_uid_ref = fileRef->GetEntryString(0x0020, 0x0052);
  // 0020 0032 DS 3 Image Position (Patient)
  gdcm::DataEntry *imagePosRef = fileRef->GetDataEntry(0x0020,0x0032);
  assert( imagePosRef->GetValueCount() == 3 );

  gdcm::DirList dirList( directory, true );
  const gdcm::DirListType filenames = dirList.GetFilenames();
  gdcm::DirListType::const_iterator it = filenames.begin();
  gdcm::File *file = gdcm::File::New();
  for( ; it != filenames.end(); ++it)
    {
    file->SetFileName( *it );
    file->Load();
    std::string modality   = file->GetEntryString(0x0008,0x0060);
    // This is a dual modality: modality should be *different*
    if( modality == modalityRef ) continue;
    if ( !gdcm::Util::DicomStringEqual(modality, "PT") ) continue;
    std::string series_uid = file->GetEntryString(0x0020, 0x000e);
    // Not same series !
    if( series_uid == series_uid_ref ) continue;
    std::string frame_uid = file->GetEntryString(0x0020, 0x0052);
    if( frame_uid_ref != frame_uid ) continue;
    gdcm::DataEntry *imagePos = file->GetDataEntry(0x0020,0x0032);
    assert( imagePos->GetValueCount() == 3 );
    if( imagePos->GetValue(0) == imagePosRef->GetValue(0)
      && imagePos->GetValue(1) == imagePosRef->GetValue(1)
      && imagePos->GetValue(2) == imagePosRef->GetValue(2) )
      {
      std::cerr << "We found a match: " << *it << std::endl;
      }
    }
  return 0;
}

