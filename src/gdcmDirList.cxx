/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDirList.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/28 09:30:58 $
  Version:   $Revision: 1.17 $
                                                                                
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
#endif

// Constructor / Destructor
/**
 * \ingroup gdcmDirList
 * \brief Constructor  
 * @param  dirName root directory name
 * @param  recursive whether we want to explore recursively or not 
 */
gdcmDirList::gdcmDirList(std::string dirName,bool recursive)
{
   name=dirName;
   NormalizePath(name);
   Explore(name,recursive);
}

/**
 * \ingroup gdcmDirList
 * \brief  Destructor
 */
gdcmDirList::~gdcmDirList(void)
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
std::string gdcmDirList::GetDirName(void)
{
   return(name);
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

/**
 * \ingroup gdcmDirList
 * \brief   Explore a directory with possibility of recursion
 * @param  dirName directory to explore
 * @param  recursive whether we want recursion or not
 */
void gdcmDirList::Explore(std::string dirName,bool recursive)
{
   std::string fileName;
   NormalizePath(dirName);
#if defined(_MSC_VER) || (__CYGWIN__)
   WIN32_FIND_DATA fileData; 
   HANDLE hFile=FindFirstFile((dirName+"*").c_str(),&fileData);
   int found=true;

   while( (hFile!=INVALID_HANDLE_VALUE) && (found) )
   {
      fileName=fileData.cFileName;
      if(fileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
      {
         if( (fileName!=".") && (fileName!="..") && (recursive) )
            Explore(dirName+fileName,recursive);
      }
      else
      {
         this->push_back(dirName+fileName);
      }

      found=FindNextFile(hFile,&fileData);
   }

#else
   struct dirent **namelist;
   int n=scandir(dirName.c_str(), &namelist, 0, alphasort);

   for (int i= 0;i < n; i++) 
   {
      fileName=namelist[i]->d_name;     
      if(namelist[i]->d_type==DT_DIR)
      {
         if( (fileName!=".") && (fileName!="..") && (recursive) )
            Explore(dirName+fileName,recursive);
      }
      else
      {
         this->push_back(dirName+fileName);
      }
   }
#endif
}

//-----------------------------------------------------------------------------
