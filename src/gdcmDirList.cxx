/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDirList.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/14 21:52:06 $
  Version:   $Revision: 1.33 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDirList.h"
#include "gdcmUtil.h"

#include <iterator>

#ifdef _MSC_VER
   #include <windows.h> 
   #include <direct.h>
#else
   #include <dirent.h>   
   #include <sys/types.h>
   #include <sys/stat.h>
#endif

namespace gdcm 
{
// Constructor / Destructor
/**
 * \ingroup DirList
 * \brief Constructor  
 * @param  dirName root directory name
 * @param  recursive whether we want to explore recursively or not 
 */
DirList::DirList(std::string const &dirName, bool recursive)
{
   DirName = dirName;
   Explore(dirName, recursive);
}

/**
 * \ingroup DirList
 * \brief  Destructor
 */
DirList::~DirList()
{
}

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup DirList
 * \brief   Get the directory name
 * @return the directory name 
 */
std::string const &DirList::GetDirName() const
{
   return DirName;
}

void DirList::Print(std::ostream &os)
{
   copy(begin(), end(), std::ostream_iterator<std::string>(os, "\n"));
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

/**
 * \brief   Explore a directory with possibility of recursion
 *          return number of files read
 * @param  dirpath   directory to explore
 * @param  recursive whether we want recursion or not
 */
int DirList::Explore(std::string const &dirpath, bool recursive)
{
   int numberOfFiles = 0;
   std::string fileName;
   std::string dirName = Util::NormalizePath(dirpath);
#ifdef _MSC_VER
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
         push_back(dirName+fileName);
         numberOfFiles++;
      }

      found = FindNextFile(hFile, &fileData);
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
   dirent *d = 0;
   for (d = readdir(dir); d; d = readdir(dir))
   {
      fileName = dirName + d->d_name;
      stat(fileName.c_str(), &buf); //really discard output ?
      if( S_ISREG(buf.st_mode) )    //is it a regular file?
      {
         push_back( fileName );
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
} // end namespace gdcm

//-----------------------------------------------------------------------------
