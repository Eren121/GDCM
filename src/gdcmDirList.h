// gdcmDir.h
//-----------------------------------------------------------------------------
#ifndef GDCMDIRLIST_H
#define GDCMDIRLIST_H

#include "gdcmCommon.h"

#include <string>
#include <list>

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDirList: public std::list<std::string>
{
public :
   gdcmDirList(std::string dirName,bool recursive=false);
   virtual ~gdcmDirList(void);

   std::string GetDirName(void);

   static const char SEPARATOR_X;
   static const char SEPARATOR_WIN;
   static const std::string SEPARATOR;

private :
   void NormalizePath(std::string &dirName);
   void Explore(std::string dirName,bool recursive=false);

   std::string name;
};

//-----------------------------------------------------------------------------
#endif
