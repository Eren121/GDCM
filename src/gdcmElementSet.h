/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmElementSet.h,v $
  Language:  C++
  Date:      $Date: 2004/12/03 20:16:58 $
  Version:   $Revision: 1.26 $
                                                                                
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

class GDCM_EXPORT ElementSet : public DocEntrySet
{
public:
   ElementSet(int);
   ~ElementSet();

   bool AddEntry(DocEntry *Entry);
   bool RemoveEntry(DocEntry *EntryToRemove);
   bool RemoveEntryNoDestroy(DocEntry *EntryToRemove);
   
   void Print(std::ostream &os = std::cout); 
   void WriteContent(std::ofstream *fp, FileType filetype); 
   
   /// Accessor to \ref TagHT
   // Do not expose this to user (public API) ! 
   // A test is using it thus put it in public (matt)
   TagDocEntryHT const & GetTagHT() const { return TagHT; };

   void Initialize();
   DocEntry *GetNextEntry();

protected:
    
private:
// Variables
   /// Hash Table (map), to provide fast access
   TagDocEntryHT TagHT; 
   /// Hash Table (map) iterator, used to visit the TagHT variable
   TagDocEntryHT::iterator ItTagHT; 
 
   friend class Document;
   friend class DicomDir; //For accessing private TagHT
   friend class DocEntryArchive; //For accessing private TagHT
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif

