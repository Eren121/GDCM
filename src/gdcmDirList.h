/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDirList.h,v $
  Language:  C++
  Date:      $Date: 2007/05/23 14:18:09 $
  Version:   $Revision: 1.33 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDIRLIST_H
#define GDCMDIRLIST_H

#include "gdcmBase.h"

#include <string>
#include <vector>
#include <iostream>

namespace GDCM_NAME_SPACE
{

typedef std::vector<std::string> DirListType;

//-----------------------------------------------------------------------------

// NOTE: Due to a M$VC6 'feature' we cannot export a std::list in a dll, 
// so GDCM_EXPORT keyword was removed for this class only

/**
 * \brief   List containing the file headers of all the 'gdcm readable' files
 *          found by exploring (possibly recursively) a root directory. 
 */
class GDCM_EXPORT DirList : public Base
{
public :
   DirList(std::string const &dirName, bool recursive);
   ~DirList();

   void Print(std::ostream &os = std::cout, std::string const &indent = "" );

   /// Return the name of the directory
   std::string const &GetDirName() const { return DirName; }

   /// Return the file names
   DirListType const &GetFilenames() const { return Filenames; }
   
    /// Return the number of Files
   int GetSize() const { return Filenames.size(); }  

   static bool IsDirectory(std::string const &dirName);
   
   std::string GetFirst();
   std::string GetNext();
   
private :
   int Explore(std::string const &dirName, bool recursive=false);

   /// List of file names
   DirListType Filenames;
   /// name of the root directory to explore
   std::string DirName;
   
   /// iterator on the SQItems of the current SeqEntry
   DirListType::iterator ItDirList;  
   
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
