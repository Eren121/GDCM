// gdcmDirList.h
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

/// \
   static const char SEPARATOR_X;
/// /  
static const char SEPARATOR_WIN;
/// depending on the O.S.
   static const std::string SEPARATOR;

private :
   void Explore(std::string dirName,bool recursive=false);
/// name of the root directory to explore
   std::string name;
};

//-----------------------------------------------------------------------------
#endif
