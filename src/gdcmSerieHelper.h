/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSerieHelper.h,v $
  Language:  C++
  Date:      $Date: 2005/08/30 08:12:40 $
  Version:   $Revision: 1.17 $
                                                                                
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
#include "gdcmDebug.h"  // for LEGACY
 
#include <vector>
#include <iostream>
#include <map>

namespace gdcm 
{
class File;
typedef std::vector<File* > FileList;

typedef bool (*BOOL_FUNCTION_PFILE_PFILE_POINTER)(File *, File *);

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
   void AddGdcmFile(File *header);

   void SetDirectory(std::string const &dir, bool recursive=false);
   bool IsCoherent(FileList *coherentFileList);
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
   GDCM_LEGACY( void AddRestriction(TagKey const &key, std::string const &value) );
   void AddRestriction(uint16_t group, uint16_t elem, 
                       std::string const &value, int op);
  
/**
 * \brief Sets the LoadMode as a boolean string. 
 *        NO_SEQ, NO_SHADOW, NO_SHADOWSEQ
 *        (nothing more, right now)
 *        WARNING : before using NO_SHADOW, be sure *all* your files
 *        contain accurate values in the 0x0000 element (if any) 
 *        of *each* Shadow Group. The parser will fail if the size is wrong !
 * @param   mode Load mode to be used    
 */
   void SetLoadMode (int mode) { LoadMode = mode; }

/// Brief User wants the files to be sorted Direct Order (default value)
   void SetSortOrderToDirect()  { DirectOrder = true;  }

/// Brief User wants the files to be sorted Reverse Order 
   void SetSortOrderToReverse() { DirectOrder = false; }

   /// to allow user to give is own comparison function
   void SetUserLessThanFunction( BOOL_FUNCTION_PFILE_PFILE_POINTER userFunc ) 
                        { UserLessThanFunction = userFunc; }   
private:
   bool UserOrdering(FileList *coherentFileList);
   bool ImagePositionPatientOrdering(FileList *coherentFileList);
   bool ImageNumberOrdering(FileList *coherentFileList);
   bool FileNameOrdering(FileList *coherentFileList);
   
   static bool ImageNumberLessThan(File *file1, File *file2);
   static bool ImageNumberGreaterThan(File *file1, File *file2);
   static bool FileNameLessThan(File *file1, File *file2);
   static bool FileNameGreaterThan(File *file1, File *file2);

//Attributes:
   CoherentFileListmap CoherentFileListHT;
   CoherentFileListmap::iterator ItListHt;

   typedef std::pair<TagKey, std::string> Rule;
   typedef std::vector<Rule> SerieRestrictions;
   SerieRestrictions Restrictions;
   
   // New style for (extented) Rules (Moreover old one doesn't compile)
   typedef struct {
      uint16_t group;
      uint16_t elem;
      std::string value;
      int op;
   } ExRule;
   typedef std::vector<ExRule> SerieExRestrictions;
   SerieExRestrictions ExRestrictions;

   /// \brief Bit string integer (each one considered as a boolean)
   ///        Bit 0 : Skip Sequences,    if possible
   ///        Bit 1 : Skip Shadow Groups if possible
   ///        Probabely, some more to add
   int LoadMode;

   /// \brief whether we want to sort in direct order or not (reverse order).
   ///        To be used by aware user only
   bool DirectOrder;

   /// \brief If user knows more about his images than gdcm does,
   ///        he may supply his own comparison function.
   /*static*/ BOOL_FUNCTION_PFILE_PFILE_POINTER UserLessThanFunction;
};

} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
