// gdcmUtil.cxx
//-----------------------------------------------------------------------------
#include "gdcmUtil.h"
#include "gdcmDebug.h"
#include <stdio.h>
#include <ctype.h>   // For isspace
#include <string.h>  // CLEANME: could this be only string ? Related to Win32 ?
#include <iostream>

/**
 * \ingroup Globals
 * \brief   Because is not yet available in g++2.96
 */
std::istream& eatwhite(std::istream& is) {
   char c;
   while (is.get(c)) {
      if (!isspace(c)) {
         is.putback(c);
         break;
      }
   }
   return is;
}

/**
 * \ingroup Globals
 * \brief Because not available in C++ (?)
 */
void Tokenize (const std::string& str,
               std::vector<std::string>& tokens,
               const std::string& delimiters) {
   std::string::size_type lastPos = str.find_first_not_of(delimiters,0);
   std::string::size_type pos     = str.find_first_of    (delimiters,lastPos);
   while (std::string::npos != pos || std::string::npos != lastPos) {
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      lastPos = str.find_first_not_of(delimiters, pos);
      pos     = str.find_first_of    (delimiters, lastPos);
   }
}

/**
 * \ingroup Globals
 * \brief  Weed out a string from the non-printable characters (in order
 *         to avoid corrupting the terminal of invocation when printing)
 * @param s string to remove non printable characters from
 */
std::string CreateCleanString(std::string s) {
   std::string str=s;

   for(unsigned int i=0;i<str.size();i++)
   {
      if(!isprint(str[i]))
         str[i]='.';
   }

   if(str.size()>0)
      if(!isprint(s[str.size()-1]))
         if(s[str.size()-1]==0)
            str[str.size()-1]=' ';

   return(str);
}

/**
 * \ingroup Globals
 * \brief   Add a SEPARATOR to the end of the name is necessary
 * @param name file/directory name to normalize 
 */
void NormalizePath(std::string &name)
{
   const char SEPARATOR_X      = '/';
   const char SEPARATOR_WIN    = '\\';
   const std::string SEPARATOR = "/";
   int size=name.size();

   if((name[size-1]!=SEPARATOR_X)&&(name[size-1]!=SEPARATOR_WIN))
   {
      name+=SEPARATOR;
   }
}

/**
 * \ingroup Globals
 * \brief   Get the (directory) path from a full path file name
 * @param   fullName file/directory name to extract Path from
 */
std::string GetPath(std::string &fullName)
{
   int pos1=fullName.rfind("/");
   int pos2=fullName.rfind("\\");
   if(pos1>pos2)
      fullName.resize(pos1);
   else
      fullName.resize(pos2);
   return(fullName);
}

/**
 * \ingroup Globals
 * \brief   Get the (last) name of a full path file name
 * @param   fullName file/directory name to extract end name from
 */
std::string GetName(std::string &fullName)
{   
  int fin=fullName.length()-1;
  char a =fullName.c_str()[fin];
  if (a == '/' || a == '\\') {
     fin--;
  }
  int deb;
  for (int i=fin;i!=0;i--) {
     if (fullName.c_str()[i] == '/' || fullName.c_str()[i] == '\\')  
        break;
      deb = i;
  }    

  std::string lastName;
  for (int j=deb;j<fin+1;j++)
    lastName=lastName+fullName.c_str()[j];

  return(lastName);
} 
