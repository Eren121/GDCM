/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSerieHelper.h,v $
  Language:  C++
  Date:      $Date: 2005/11/08 16:31:21 $
  Version:   $Revision: 1.27 $
                                                                                
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
#include "gdcmTagKey.h" 
#include "gdcmDebug.h"  // for LEGACY
 
#include <vector>
#include <iostream>
#include <map>

namespace gdcm 
{
class File;


   typedef std::vector<File* > FileList;
   
   /// \brief XCoherent stands for 'Extra Coherent', 
   ///        (The name 'Coherent' would be enough but it was used before;
   ///        I don't want to put a bomb in the code)
   ///        Any 'better name' is welcome !
   typedef std::map<std::string, FileList *> XCoherentFileSetmap; 
      
   typedef bool (*BOOL_FUNCTION_PFILE_PFILE_POINTER)(File *, File *);

//-----------------------------------------------------------------------------
/**
 * \brief  
 * - This class should be used for a stack of 2D dicom images.
 *   It allows to explore (recursively or not) a directory and 
 *   makes a set of 'Single SerieUID Filesets' 
 *   It allows :
 *   - to sort any of the 'Single SerieUID Fileset' on the image position.
 *   - to split any of the Single SerieUID Filesets (better use this name than
 *   'Coherent File List' : it's NOT a std::list, files are NOT coherent ...)
 *    into several XCoherent Filesets 
 *   XCoherent stands for 'Extra Coherent' (same orientation, or same position)
 */
class GDCM_EXPORT SerieHelper 
{
public:
   /// SingleSerieUIDFileSetmap replaces the former CoherentFileListmap
   /// ( List were actually std::vectors, and wher no coherent at all :
   ///   They were only Single SeriesInstanceUID File sets)
   typedef std::map<std::string, FileList *> SingleSerieUIDFileSetmap;

   typedef std::vector<File* > FileVector;

  
   SerieHelper();
   ~SerieHelper();
   void Print(std::ostream &os = std::cout, std::string const &indent = "" );

   /// \todo should return bool or throw error ?
   void AddFileName(std::string const &filename);
   void AddGdcmFile(File *header);

   void SetDirectory(std::string const &dir, bool recursive=false);
   bool IsCoherent(FileList *fileSet);
   void OrderFileList(FileList *fileSet);
   
   /// \brief Gets the FIRST Single SerieUID Fileset.
   ///        Deprecated; kept not to break the API
   /// \note Caller must call OrderFileList first
   /// @return the (first) Single SerieUID Fileset
   const FileList &GetFileList()
                           { return *SingleSerieUIDFileSetHT.begin()->second; }
  
   GDCM_LEGACY(   FileList *GetFirstCoherentFileList()  );
   GDCM_LEGACY(   FileList *GetNextCoherentFileList()   );
   GDCM_LEGACY(   FileList *GetCoherentFileList(std::string serieUID)  );

   FileList *GetFirstSingleSerieUIDFileSet();
   FileList *GetNextSingleSerieUIDFileSet();
   FileList *GetSingleSerieUIDFileSet(std::string serieUID);
   /// brief returns the 'Series Instance UID' Single SerieUID FileSet
   std::string GetCurrentSerieUIDFileSetUID()
                             { return  (*ItFileSetHt).first; }
   /// All the following allow user to restrict DICOM file to be part
   /// of a particular serie
   GDCM_LEGACY( void AddRestriction(TagKey const &key, std::string const &value) );
   void AddRestriction(TagKey const &key, std::string const &value, int op);
 
/**
 * \brief Sets the LoadMode as a boolean string. 
 *        LD_NOSEQ, LD_NOSHADOW, LD_NOSHADOWSEQ
 *        ... (nothing more, right now)
 *        WARNING : before using LD_NOSHADOW, be sure *all* your files
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

   XCoherentFileSetmap SplitOnOrientation(FileList *fileSet); 
   XCoherentFileSetmap SplitOnPosition(FileList *fileSet); 
   XCoherentFileSetmap SplitOnTagValue(FileList *fileSet,
                                               uint16_t group, uint16_t elem);
private:
   void ClearAll();
   bool UserOrdering(FileList *fileSet);
   bool ImagePositionPatientOrdering(FileList *fileSet);
   bool ImageNumberOrdering(FileList *fileSet);
   bool FileNameOrdering(FileList *fileSet);
   
   static bool ImageNumberLessThan(File *file1, File *file2);
   static bool ImageNumberGreaterThan(File *file1, File *file2);
   static bool FileNameLessThan(File *file1, File *file2);
   static bool FileNameGreaterThan(File *file1, File *file2);

//Attributes:
   
   SingleSerieUIDFileSetmap SingleSerieUIDFileSetHT;
   SingleSerieUIDFileSetmap::iterator ItFileSetHt;
   
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
    BOOL_FUNCTION_PFILE_PFILE_POINTER UserLessThanFunction;

};

} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
