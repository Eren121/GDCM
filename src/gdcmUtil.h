/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmUtil.h,v $
  Language:  C++
  Date:      $Date: 2004/10/12 04:35:48 $
  Version:   $Revision: 1.36 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMUTIL_H
#define GDCMUTIL_H

#include "gdcmCommon.h"
#include <vector>
#include <string>

namespace gdcm 
{
/**
 * \defgroup Globals Utility functions
 * \brief    Here are some utility functions, belonging to NO class,
 *           dealing with strings, file names... that can be called
 *           from anywhere by whomsoever they can help.
 */

//-----------------------------------------------------------------------------

class GDCM_EXPORT Util
{
public:
   static std::string Format(const char* format, ...);
   static void        Tokenize (const std::string& str,
                         std::vector<std::string>& tokens,
                         const std::string& delimiters = " ");
   static int         CountSubstring (const std::string& str,
                               const std::string& subStr);       

   static std::string CreateCleanString(std::string s);
   static void        NormalizePath(std::string &name);
   static std::string GetPath(std::string &fullName);
   static std::string GetName(std::string &fullName);
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
