/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmUtil.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/27 22:58:06 $
  Version:   $Revision: 1.56 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmUtil.h"
#include "gdcmDebug.h"

#include <stdarg.h>  //only included in implementation file
#include <stdio.h>   //only included in implementation file

namespace gdcm 
{

/**
 * \ingroup Globals
 * \brief Provide a better 'c++' approach for sprintf
 * For example c code is:
 * sprintf(trash, "%04x|%04x", group , element);
 *
 * c++ is 
 * std::ostringstream buf;
 * buf << std::right << std::setw(4) << std::setfill('0') << std::hex
 *     << group << "|" << std::right << std::setw(4) << std::setfill('0') 
 *     << std::hex <<  element;
 * buf.str();
 */

std::string Util::Format(const char* format, ...)
{
   char buffer[2048];
   va_list args;
   va_start(args, format);
   vsprintf(buffer, format, args);  //might be a security flaw
   return buffer;
}


/**
 * \ingroup Globals
 * \brief Because not available in C++ (?)
 */
void Util::Tokenize (const std::string& str,
                     std::vector<std::string>& tokens,
                     const std::string& delimiters)
{
   std::string::size_type lastPos = str.find_first_not_of(delimiters,0);
   std::string::size_type pos     = str.find_first_of    (delimiters,lastPos);
   while (std::string::npos != pos || std::string::npos != lastPos)
   {
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      lastPos = str.find_first_not_of(delimiters, pos);
      pos     = str.find_first_of    (delimiters, lastPos);
   }
}

/**
 * \ingroup Globals
 * \brief Because not available in C++ (?)
 *        Counts the number of occurences of a substring within a string
 */
 
int Util::CountSubstring (const std::string& str,
                          const std::string& subStr)
{
   int count = 0;   // counts how many times it appears
   unsigned int x = 0;       // The index position in the string

   do
   {
      x = str.find(subStr,x);       // Find the substring
      if (x != std::string::npos)   // If present
      {
         count++;                  // increase the count
         x += subStr.length();     // Skip this word
      }
   }
   while (x != std::string::npos);  // Carry on until not present

   return count;
}

/**
 * \ingroup Globals
 * \brief  Weed out a string from the non-printable characters (in order
 *         to avoid corrupting the terminal of invocation when printing)
 * @param s string to remove non printable characters from
 */
std::string Util::CreateCleanString(std::string const & s)
{
   std::string str = s;

   for(unsigned int i=0;i<str.size();i++)
   {
      if(!isprint(str[i]))
      {
         str[i]='.';
      }
   }

   if(str.size()>0)
   {
      if(!isprint(s[str.size()-1]))
      {
         if(s[str.size()-1]==0)
         {
            str[str.size()-1]=' ';
         }
      }
   }

   return str;
}

/**
 * \ingroup Globals
 * \brief   Add a SEPARATOR to the end of the name is necessary
 * @param name file/directory name to normalize 
 */
std::string Util::NormalizePath(std::string const & pathname)
{
   const char SEPARATOR_X      = '/';
   const char SEPARATOR_WIN    = '\\';
   const std::string SEPARATOR = "/";
   std::string name = pathname;
   int size = name.size();

   if( name[size-1] != SEPARATOR_X && name[size-1] != SEPARATOR_WIN )
   {
      name += SEPARATOR;
   }
   return name;
}

/**
 * \ingroup Globals
 * \brief   Get the (directory) path from a full path file name
 * @param   fullName file/directory name to extract Path from
 */
std::string Util::GetPath(std::string const & fullName)
{
   std::string res = fullName;
   int pos1 = res.rfind("/");
   int pos2 = res.rfind("\\");
   if( pos1 > pos2)
   {
      res.resize(pos1);
   }
   else
   {
      res.resize(pos2);
   }

   return res;
}

/**
 * \ingroup Globals
 * \brief   Get the (last) name of a full path file name
 * @param   fullName file/directory name to extract end name from
 */
std::string Util::GetName(std::string const & fullName)
{   
  std::string filename = fullName;

  std::string::size_type slash_pos = filename.rfind("/");
  std::string::size_type backslash_pos = filename.rfind("\\");
  slash_pos = slash_pos > backslash_pos ? slash_pos : backslash_pos;
  if(slash_pos != std::string::npos)
    {
    return filename.substr(slash_pos + 1);
    }
  else
    {
    return filename;
    }
} 

} // end namespace gdcm

