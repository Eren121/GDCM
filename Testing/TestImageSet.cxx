/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestImageSet.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/08 13:39:57 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

/**
 * Write a dicom file from nothing
 * The written image is 256x256, 8 bits, unsigned char
 * The image content is a horizontal grayscale from 
 * 
 */
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmValEntry.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"

#include <iostream>
#include <sstream>
#include <list>

typedef std::list<gdcm::File *> FileList;

// If there is sameSerie, sameStudy is set to true
int CompareImages(FileList &list, bool sameSerie, bool sameStudy)
{
   gdcm::Debug::DebugOn();

   if( sameSerie )
      sameStudy = true;

   gdcm::ValEntry *entry;
   std::map<std::string, int> instUID;
   std::map<std::string, int> mediaUID;
   std::map<std::string, int> serieUID;
   std::map<std::string, int> studyUID;

   FileList::iterator it;
   for(it=list.begin();it!=list.end();++it)
   {
      // SOP Instance UID
      entry=(*it)->GetValEntry(0x0008, 0x0018);
      if( entry )
         if( instUID.find(entry->GetValue())!=instUID.end() )
            instUID[entry->GetValue()]++;
         else
            instUID[entry->GetValue()]=1;
      // Media Storage SOP Instance UID
      entry=(*it)->GetValEntry(0x0002,0x0003);
      if( entry )
         if( mediaUID.find(entry->GetValue())!=mediaUID.end() )
            mediaUID[entry->GetValue()]++;
         else
            mediaUID[entry->GetValue()]=1;
      // Series Instance UID
      entry=(*it)->GetValEntry(0x0020,0x000e);
      if( entry )
         if( serieUID.find(entry->GetValue())!=serieUID.end() )
            serieUID[entry->GetValue()]++;
         else
            serieUID[entry->GetValue()]=1;
      // Study Instance UID
      entry=(*it)->GetValEntry(0x0020,0x000d);
      if( entry )
         if( studyUID.find(entry->GetValue())!=studyUID.end() )
            studyUID[entry->GetValue()]++;
         else
            studyUID[entry->GetValue()]=1;
   }

   if( sameSerie )
   {
      if( serieUID.size()>1 )
      {
         std::cout << "Failed\n"
                   << "        Series UID not same (0x0020,0x000e)\n";
         return 1;
      }
   }
   else
   {
      if( serieUID.size()!=list.size() )
      {
         std::cout << "Failed\n"
                   << "        Some Series UID are same (0x0020,0x000e)\n";
         return 1;
      }
   }

   if( sameStudy )
   {
      if( studyUID.size()>1 )
      {
         std::cout << "Failed\n"
                   << "        Studies UID not same (0x0020,0x000d)\n";
         return 1;
      }
   }
   else
   {
      if( studyUID.size()!=list.size() )
      {
         std::cout << "Failed\n"
                   << "        Some Studies UID are same (0x0020,0x000d)\n";
         return 1;
      }
   }

   if( mediaUID.size()!=list.size() )
   {
      std::cout << "Failed\n"
                << "        Some Media UID are same (0x0002,0x0003)\n";
      return 1;
   }

   if( instUID.size()!=list.size() )
   {
      std::cout << "Failed\n"
                << "        Some Instance UID are same (0x0008,0x0018)\n";
      return 1;
   }

   return 0;
}

void ClearList(FileList &list)
{
   FileList::iterator it;
   for(it=list.begin();it!=list.end();++it)
   {
      delete (*it);
   }
   list.clear();
}

gdcm::File *WriteImage(gdcm::File *file, const std::string &fileName)
{
   // Create a 256x256x1 image 8 bits, unsigned 
   std::ostringstream str;

   // Set the image size
   file->InsertValEntry("256",0x0028,0x0011); // Columns
   file->InsertValEntry("256",0x0028,0x0010); // Rows

   // Set the pixel type
   file->InsertValEntry("8",0x0028,0x0100); // Bits Allocated
   file->InsertValEntry("8",0x0028,0x0101); // Bits Stored
   file->InsertValEntry("7",0x0028,0x0102); // High Bit

   // Set the pixel representation
   file->InsertValEntry("0",0x0028,0x0103); // Pixel Representation

   // Set the samples per pixel
   file->InsertValEntry("1",0x0028,0x0002); // Samples per Pixel

   // The so called 'prepared image', built ex nihilo just before,
   // has NO Pixel Element yet.
   // therefore, it's NEVER 'file readable' ...
    
   //if( !file->IsReadable() )
   // {
   //   std::cout << "Failed\n"
   //             << "        Prepared image isn't readable\n";
   //  return NULL;
   //}

   size_t size = 256 * 256 * 1;
   unsigned char *imageData = new unsigned char[size];
   memset(imageData,0,size);

// Write the image
   gdcm::FileHelper *hlp = new gdcm::FileHelper(file);
   hlp->SetImageData(imageData,size);
   hlp->SetWriteTypeToDcmExplVR();
   if( !hlp->Write(fileName) )
   {
      std::cout << "Failed\n"
                << "        File in unwrittable\n";

      delete hlp;
      delete[] imageData;
      return NULL;
   }
   delete[] imageData;
   delete hlp;

// Read the written image
   gdcm::File *reread = new gdcm::File(  );
   reread->SetFileName( fileName );
   reread->Load();
   if( !reread->IsReadable() )
   {
     std::cerr << "Failed" << std::endl
               << "        Could not reread written image :" << fileName << std::endl;
     delete reread;
     return NULL;
   }

   return reread;
}

int TestImageSet(int argc, char *argv[])
{
   if (argc < 1) 
   {
      std::cerr << "usage: \n" 
                << argv[0] << " (without parameters) " << std::endl 
                << std::endl;
      return 1;
   }

   std::cout << "   Description (Test::TestSequence): " << std::endl;
   std::cout << "   Tests the creation of a 4 images Set" << std::endl;
   std::cout << "   with the following steps : "<< std::endl;
   std::cout << "   step 1: create images belonging" << std::endl
             << "           to different Study and Terie" << std::endl;
   std::cout << "   step 2: create images belonging" << std::endl
             << "           to the same Serie (therefore to the same Study)" << std::endl;
   std::cout << "   step 3: create images belonging" << std::endl
             << "           to different Series within the same Study" << std::endl;
   std::cout << std::endl << std::endl;

   gdcm::File *file;
   gdcm::File *newFile;
   FileList fileList;
   int i;

   std::cout<<"     step...";
   std::string studyUID;
   std::string serieUID;

   // Step 1 : All files have different UID 
   fileList.clear();
   for(i = 0;i < 4;i++)
   {
      std::ostringstream fileName;
      fileName << "FileSeq" << i << ".dcm";
      file = new gdcm::File();
      // It's up to the user to initialize Serie UID and Study UID
      // Study Instance UID
      studyUID = gdcm::Util::CreateUniqueUID();
      file->InsertValEntry(studyUID, 0x0020, 0x000d);
      // Series Instance UID
      serieUID = gdcm::Util::CreateUniqueUID();
      file->InsertValEntry(serieUID, 0x0020, 0x000e);

      newFile = WriteImage(file, fileName.str());
      if( !newFile )
      {
         delete file;
         return 1;
      }
      else
         fileList.push_back(newFile);

      delete file;
   }

   if( CompareImages(fileList, false, false) )
   {
      ClearList(fileList);
      return 1;
   }
   ClearList(fileList);

   std::cout<<"1...";

   // Step 2 : Same Serie & Study
   fileList.clear();
   studyUID = gdcm::Util::CreateUniqueUID();
   serieUID = gdcm::Util::CreateUniqueUID();
   for(i = 0;i < 4;i++)
   {
      std::ostringstream fileName;
      fileName << "FileSeq" << i << ".dcm";
      file = new gdcm::File();
      file->InsertValEntry(studyUID, 0x0020, 0x000d);
      file->InsertValEntry(serieUID, 0x0020, 0x000e);

      newFile = WriteImage(file, fileName.str());
      if( !newFile )
      {
         delete file;
         return(1);
      }
      else
         fileList.push_back(newFile);

      delete file;
   }

   if( CompareImages(fileList, true, true) )
   {
      ClearList(fileList);
      return 1;
   }
   ClearList(fileList);

   std::cout<<"2...";

   // Step 3 : Same Study
   fileList.clear();
   serieUID = gdcm::Util::CreateUniqueUID();
   for(i = 0;i < 4;i++)
   {
      std::ostringstream fileName;
      fileName << "FileSeq" << i << ".dcm";
      file = new gdcm::File();
      file->InsertValEntry(studyUID, 0x0020, 0x000d);
      serieUID = gdcm::Util::CreateUniqueUID();
      file->InsertValEntry(serieUID, 0x0020, 0x000e);
      newFile = WriteImage(file, fileName.str());
      if( !newFile )
      {
         delete file;
         return(1);
      }
      else
         fileList.push_back(newFile);

      delete file;
   }

   if( CompareImages(fileList, false, true) )
   {
      ClearList(fileList);
      return 1;
   }
   ClearList(fileList);

   std::cout<<"3...OK\n";

   return 0;
}
