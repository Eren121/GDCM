/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDirList.cxx,v $
  Language:  C++
  Date:      $Date: 2004/07/26 19:36:56 $
  Version:   $Revision: 1.18 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDirList.h"
#include "gdcmUtil.h"

#include <iostream>
#include <algorithm>

#if defined(_MSC_VER) || defined (__CYGWIN__)
   #include <windows.h> 
#ifdef _MSC_VER
   #include <direct.h>
#endif //_MSC_VER
#else
   #include <dirent.h>   
   #include <unistd.h>
   #include <sys/stat.h>
   #include <sys/types.h>
#endif

// Constructor / Destructor
/**
 * \ingroup gdcmDirList
 * \brief Constructor  
 * @param  dirName root directory name
 * @param  recursive whether we want to explore recursively or not 
 */
gdcmDirList::gdcmDirList(std::string dirName, bool recursive)
{
   name = dirName;
   NormalizePath(name);
   Explore(name, recursive);
}

/**
 * \ingroup gdcmDirList
 * \brief  Destructor
 */
gdcmDirList::~gdcmDirList()
{
}

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup gdcmDirList
 * \brief   Get the directory name
 * @return the directory name 
 */
std::string gdcmDirList::GetDirName()
{
   return name;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

/**
 * \ingroup gdcmDirList
 * \brief   Explore a directory with possibility of recursion
 *          return number of files read
 * @param  dirName directory to explore
 * @param  recursive whether we want recursion or not
 */
int gdcmDirList::Explore(std::string dirName, bool recursive)
{
   int numberOfFiles = 0;
   std::string fileName;
   NormalizePath(dirName);
#if defined(_MSC_VER) || (__CYGWIN__)
   WIN32_FIND_DATA fileData; 
   HANDLE hFile=FindFirstFile((dirName+"*").c_str(),&fileData);
   int found = true;

   while( hFile != INVALID_HANDLE_VALUE && found )
   {
      fileName = fileData.cFileName;
      if( fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
      {
         // Is the '.' and '..' usefull ?
         if( fileName != "." && fileName != ".." && recursive )
         {
            numberOfFiles += Explore(dirName+fileName,recursive);
         }
      }
      else
      {
         this->push_back(dirName+fileName);
         numberOfFiles++;
      }

      found = FindNextFile(hFile,&fileData);
   }

#else
  // Real POSIX implementation: scandir is a BSD extension only, and doesn't 
  // work on debian for example

   DIR* dir = opendir(dirName.c_str());
   if (!dir)
   {
      return 0;
   }

   // According to POSIX, the dirent structure contains a field char d_name[]
   // of  unspecified  size,  with  at most NAME_MAX characters preceding the
   // terminating null character.  Use of other fields will harm  the  porta-
   // bility  of  your  programs.

   struct stat buf;
   dirent* d = 0;
   for (d = readdir(dir); d; d = readdir(dir))
   {
      fileName = dirName + d->d_name;
      stat(fileName.c_str(), &buf); //really discard output ?
      if( S_ISREG(buf.st_mode) ) //is it a regular file?
      {
         this->push_back( fileName );
         numberOfFiles++;
      }
      else if( S_ISDIR(buf.st_mode) ) //directory?
      {
         if( d->d_name[0] != '.' && recursive ) //we are also skipping hidden files
         {
            numberOfFiles += Explore( fileName, recursive);
         }
      }
      else
      {
         // we might need to do a different treament
         //abort();
      }
   }
  closedir(dir);
#endif

  return numberOfFiles;
}

//-----------------------------------------------------------------------------
