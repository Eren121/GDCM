// gdcmDirList.h
//-----------------------------------------------------------------------------
#ifndef GDCMDIRLIST_H
#define GDCMDIRLIST_H

#include "gdcmCommon.h"

#include <string>
#include <list>

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmDirList
 * \brief   List containing the file headers from root directory. 
 */
class GDCM_EXPORT gdcmDirList: public std::list<std::string>
{
public :
   gdcmDirList(std::string dirName,bool recursive=false);
   virtual ~gdcmDirList(void);

   std::string GetDirName(void);

   /// Character \ 
   static const char SEPARATOR_X;
   /// Character /  
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
