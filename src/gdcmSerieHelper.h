/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSerieHelper.h,v $
  Language:  C++
  Date:      $Date: 2005/07/11 15:29:04 $
  Version:   $Revision: 1.11 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMSERIEHELPER_H
#define GDCMSERIEHELPER_H

#include "gdcmCommon.h" 

#include <vector>
#include <iostream>
#include <map>

namespace gdcm 
{
class File;
typedef std::vector<File* > FileList;

//-----------------------------------------------------------------------------
/**
 * \brief  
 * - This class should be used for a stack of 2D dicom images.
 *   It allows to explore (recursively or not) a directory and 
 *   makes a set of 'Coherent Files' list (coherent : same Serie UID)
 *   It allows to sort any of the Coherent File list on the image position
 */
class GDCM_EXPORT SerieHelper 
{
public:
   typedef std::map<std::string, FileList *> CoherentFileListmap;
   typedef std::vector<File* > FileVector;

   SerieHelper();
   ~SerieHelper();
   void Print(std::ostream &os = std::cout, std::string const &indent = "" );

   /// \todo should return bool or throw error ?
   void AddFileName(std::string const &filename);
   void SetDirectory(std::string const &dir, bool recursive=false);
   void OrderFileList(FileList *coherentFileList);
   
   /// \brief Gets the FIRST *coherent* File List.
   ///        Deprecated; kept not to break the API
   /// \note Caller must call OrderFileList first
   /// @return the (first) *coherent* File List
   const FileList &GetFileList() { return *CoherentFileListHT.begin()->second; }
  
   FileList *GetFirstCoherentFileList();
   FileList *GetNextCoherentFileList();
   FileList *GetCoherentFileList(std::string serieUID);

   /// All the following allow user to restrict DICOM file to be part
   /// of a particular serie
   void AddRestriction(TagKey const &key, std::string const &value);
  

private:
   bool ImagePositionPatientOrdering(FileList *coherentFileList);
   bool ImageNumberOrdering(FileList *coherentFileList);
   bool FileNameOrdering(FileList *coherentFileList);
   
   static bool ImageNumberLessThan(File *file1, File *file2);
   static bool FileNameLessThan(File *file1, File *file2);
   CoherentFileListmap CoherentFileListHT;
   CoherentFileListmap::iterator ItListHt;

   typedef std::pair<TagKey, std::string> Rule;
   typedef std::vector<Rule> SerieRestrictions;
   SerieRestrictions Restrictions;
};

} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
