// gdcmDirList.cxx
//-----------------------------------------------------------------------------
#include "gdcmDirList.h"

#include <iostream>
#include <algorithm>

#ifdef GDCM_NO_ANSI_STRING_STREAM
   #include <strstream>
   #define  ostringstream ostrstream
#else
   #include <sstream>
#endif

#ifdef _MSC_VER 
   #include <windows.h> 
   #include <direct.h>
#else
   #include <dirent.h>   
   #include <unistd.h>
#endif

//-----------------------------------------------------------------------------
const char gdcmDirList::SEPARATOR_X      = '/';
const char gdcmDirList::SEPARATOR_WIN    = '\\';
const std::string gdcmDirList::SEPARATOR = "/";

//-----------------------------------------------------------------------------
// Constructor / Destructor
/*
 * \ingroup gdcmDirList
 * \brief Constructor  
 * @param   
 */
gdcmDirList::gdcmDirList(std::string dirName,bool recursive)
{
   name=dirName;

   NormalizePath(name);
   Explore(name,recursive);
}

/*
 * \ingroup gdcmDirList
 * \brief  Destructor
 * @param   
 */
gdcmDirList::~gdcmDirList(void)
{
}

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
// Public
/*
 * \ingroup gdcmDirList
 * \brief   Get the directory name
 * @param   
 */
std::string gdcmDirList::GetDirName(void)
{
   return(name);
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
/*
 * \ingroup gdcmDirList
 * \brief   Add a SEPARATOR to the end of the directory name is necessary
 * @param   
 */
void gdcmDirList::NormalizePath(std::string &dirName)
{
   int size=dirName.size();
   if((dirName[size-1]!=SEPARATOR_X)&&(dirName[size-1]!=SEPARATOR_WIN))
   {
      dirName+=SEPARATOR;
   }
}

/*
 * \ingroup gdcmDirList
 * \brief   Explore a directory with possibility of recursion
 * @param   
 */
void gdcmDirList::Explore(std::string dirName,bool recursive)
{
   std::string fileName;

   NormalizePath(dirName);

#ifdef _MSC_VER 
   WIN32_FIND_DATA fileData; 
   HANDLE hFile=FindFirstFile((dirName+"*").c_str(),&fileData);
   int found=true;

   while( (hFile!=INVALID_HANDLE_VALUE) && (found) )
   {
      fileName=fileData.cFileName;
      if(fileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
      {
         if( (fileName!=".") && (fileName!="..") && (recursive) )
            Explore(dirName+fileName);
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
            Explore(dirName+fileName);
      }
      else
      {
         this->push_back(dirName+fileName);
      }
   }
#endif
}

//-----------------------------------------------------------------------------
