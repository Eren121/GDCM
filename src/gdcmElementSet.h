/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmElementSet.h,v $
  Language:  C++
  Date:      $Date: 2005/01/24 16:10:52 $
  Version:   $Revision: 1.36 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMELEMENTSET_H
#define GDCMELEMENTSET_H

#include "gdcmDocEntrySet.h"

#include <map>
#include <iostream>
#include <fstream>

namespace gdcm 
{
class ValEntry;
class BinEntry;
class SeqEntry;

typedef std::map<TagKey, DocEntry *> TagDocEntryHT;

//-----------------------------------------------------------------------------
/**
 * \brief
 * \ref ElementSet is based on the STL map<> container
 * (see \ref ElementSet::TagHT), as opposed to \ref SQItem
 * which is based on an STL list container (see \ref ListDocEntry).
 * It contains the 'zero-level- DocEntry (out of any Dicom Sequence)
 */
class GDCM_EXPORT ElementSet : public DocEntrySet
{
public:
   ElementSet(int);
   ~ElementSet();

   virtual void Print(std::ostream &os = std::cout, std::string const &indent = "" ); 

   bool AddEntry(DocEntry *Entry);
   bool RemoveEntry(DocEntry *EntryToRemove);
   bool RemoveEntryNoDestroy(DocEntry *EntryToRemove);
   
   void WriteContent(std::ofstream *fp, FileType filetype); 

   DocEntry *GetFirstEntry();
   DocEntry *GetNextEntry();
   DocEntry *GetLastEntry();
   DocEntry *GetPreviousEntry();

   DocEntry *GetDocEntry(uint16_t group, uint16_t elem);
   ValEntry *GetValEntry(uint16_t group, uint16_t elem);
   BinEntry *GetBinEntry(uint16_t group, uint16_t elem);
   SeqEntry *GetSeqEntry(uint16_t group, uint16_t elem);

   bool IsEmpty() { return TagHT.empty(); };
   bool CheckIfEntryExist(uint16_t group, uint16_t elem);

   std::string GetEntryValue(uint16_t group, uint16_t elem);
   int GetEntryLength(uint16_t group, uint16_t elem);
   std::string GetEntryVR(uint16_t group, uint16_t elem);

   bool SetEntryValue(std::string const& content, 
                      uint16_t group, uint16_t elem);

protected:

private:
// Variables
   /// Hash Table (map), to provide fast access
   TagDocEntryHT TagHT; 
   /// Hash Table (map) iterator, used to visit the TagHT variable
   TagDocEntryHT::iterator ItTagHT; 
     
   friend class DicomDir;        //For accessing private TagHT
   friend class DocEntryArchive; //For accessing private TagHT
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif

