/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmUtil.h,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:48 $
  Version:   $Revision: 1.29 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMUTIL_H
#define GDCMUTIL_H

#include <vector>
#include <string>

#include "gdcmCommon.h"

/**
 * \defgroup Globals Utility functions
 * \brief    Here are some utility functions, belonging to NO class,
 *           dealing with strings, file names... that can be called
 *           from anywhere by whomsoever they can help.
 */

//-----------------------------------------------------------------------------
GDCM_EXPORT std::istream & eatwhite(std::istream & is);

GDCM_EXPORT void Tokenize (const std::string& str,
               std::vector<std::string>& tokens,
               const std::string& delimiters = " ");

GDCM_EXPORT std::string CreateCleanString(std::string s);
GDCM_EXPORT void        NormalizePath(std::string &name);
GDCM_EXPORT std::string GetPath(std::string &fullName);
GDCM_EXPORT std::string GetName(std::string &fullName);
//-----------------------------------------------------------------------------
#endif
