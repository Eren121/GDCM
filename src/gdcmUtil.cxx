/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmUtil.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/10 00:42:55 $
  Version:   $Revision: 1.53 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmUtil.h"
#include "gdcmDebug.h"


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
#include <stdarg.h>  //only included in implementation file
#include <stdio.h>   //only included in implementation file

std::string gdcmUtil::Format(const char* format, ...)
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
void gdcmUtil::Tokenize (const std::string& str,
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
 
int gdcmUtil::CountSubstring (const std::string& str,
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
std::string gdcmUtil::CreateCleanString(std::string s)
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
void gdcmUtil::NormalizePath(std::string &name)
{
   const char SEPARATOR_X      = '/';
   const char SEPARATOR_WIN    = '\\';
   const std::string SEPARATOR = "/";
   int size = name.size();

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
std::string gdcmUtil::GetPath(std::string &fullName)
{
   int pos1 = fullName.rfind("/");
   int pos2 = fullName.rfind("\\");
   if( pos1 > pos2)
   {
      fullName.resize(pos1);
   }
   else
   {
      fullName.resize(pos2);
   }

   return fullName;
}

/**
 * \ingroup Globals
 * \brief   Get the (last) name of a full path file name
 * @param   fullName file/directory name to extract end name from
 */
std::string gdcmUtil::GetName(std::string &fullName)
{   
   int fin = fullName.length()-1;
   char a =fullName.c_str()[fin];
   if (a == '/' || a == '\\')
   {
      fin--;
   }
   int deb = 0;
   for (int i=fin;i!=0;i--)
   {
      if (fullName.c_str()[i] == '/' || fullName.c_str()[i] == '\\')
      {
         break;
      }
      deb = i;
   }

   std::string lastName;
   for (int j=deb;j<fin+1;j++)
   {
      lastName=lastName+fullName.c_str()[j];
   }

  return lastName;
} 
