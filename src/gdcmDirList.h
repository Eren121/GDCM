/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDirList.h,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:06 $
  Version:   $Revision: 1.9 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

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

   /// Character '\' 
   static const char SEPARATOR_X;
   /// Character '/'  
   static const char SEPARATOR_WIN;
   /// depending on the O.S.
   static const std::string SEPARATOR;

private :
   int Explore(std::string dirName,bool recursive=false);
   /// name of the root directory to explore
   std::string name;
};

//-----------------------------------------------------------------------------
#endif
