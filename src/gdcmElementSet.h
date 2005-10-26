/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmElementSet.h,v $
  Language:  C++
  Date:      $Date: 2005/10/26 13:00:15 $
  Version:   $Revision: 1.51 $
                                                                                
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
   gdcmTypeMacro(ElementSet);

public:
   virtual void Print(std::ostream &os = std::cout, 
                      std::string const &indent = "" ); 

   void WriteContent(std::ofstream *fp, FileType filetype); 

   bool AddEntry(DocEntry *Entry);
   bool RemoveEntry(DocEntry *EntryToRemove);
   void ClearEntry();
   
   DocEntry *GetFirstEntry();
   DocEntry *GetNextEntry();
   DocEntry *GetDocEntry(uint16_t group, uint16_t elem);
   /// Tells us if the ElementSet contains no entry
   bool IsEmpty() { return TagHT.empty(); }

protected:
   ElementSet();
   ~ElementSet();
   /// \brief Some group are illegal withing some Dicom Documents
   ///        Only the Document knows it.
   bool MayIWrite(uint16_t )
                 { return true; }
private:
// Variables
   /// Hash Table (map), to provide fast access
   TagDocEntryHT TagHT;
   /// iterator, used to visit the TagHT variable
   TagDocEntryHT::iterator ItTagHT;
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif

