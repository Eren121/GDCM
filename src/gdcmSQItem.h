/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSQItem.h,v $
  Language:  C++
  Date:      $Date: 2005/01/16 04:50:42 $
  Version:   $Revision: 1.32 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#ifndef GDCMSQITEM_H
#define GDCMSQITEM_H

#include "gdcmDocEntrySet.h"
#include "gdcmElementSet.h"

#include <list>
#include <fstream>

namespace gdcm 
{
class DocEntry;

//-----------------------------------------------------------------------------
typedef std::list<DocEntry *> ListDocEntry;
//-----------------------------------------------------------------------------
/**
 * \brief a SeqEntry is composed by a set of SQItems.
 *        Each SQItem is composed by a set of DocEntry
 *        A DocEntry may be a SeqEntry
 *        ... and so forth 
 */ 
class GDCM_EXPORT SQItem : public DocEntrySet 
{
public:
   SQItem(int depthLevel);
   ~SQItem();

   virtual void Print(std::ostream &os = std::cout, std::string const & indent = "" ); 
   void WriteContent(std::ofstream *fp, FileType filetype);

   /// \brief   returns the DocEntry chained List for this SQ Item.
   ListDocEntry const &GetDocEntries() const { return DocEntries; };
   
   /// \brief   adds the passed DocEntry to the DocEntry chained List for
   /// this SQ Item.      
   bool AddEntry(DocEntry *Entry); // add to the List
   bool RemoveEntry(DocEntry *EntryToRemove);
   bool RemoveEntryNoDestroy(DocEntry *EntryToRemove);
  
   DocEntry *GetDocEntry(uint16_t group, uint16_t element);
   
   bool SetEntry(std::string const &val, uint16_t group, 
                                         uint16_t element);
    
   std::string GetEntry(uint16_t group, uint16_t element);

   /// \brief   returns the ordinal position of a given SQItem
   int GetSQItemNumber() { return SQItemNumber; };

   /// \brief   Sets the ordinal position of a given SQItem
   void SetSQItemNumber(int itemNumber) { SQItemNumber = itemNumber; };

   ///  \brief Accessor on \ref SQDepthLevel.
   int GetDepthLevel() { return SQDepthLevel; }
                                                                                
   ///  \brief Accessor on \ref SQDepthLevel.
   void SetDepthLevel(int depth) { SQDepthLevel = depth; }

   ///  \brief Accessor on \ref BaseTagKey.
   void SetBaseTagKey( BaseTagKey const &key ) { BaseTagKeyNested = key; }

   ///  \brief Accessor on \ref BaseTagKey.
   BaseTagKey const &GetBaseTagKey() const { return BaseTagKeyNested; }

   void Initialize();
   DocEntry *GetNextEntry();

protected:
// Variables that need to be access in subclasses

   /// \brief Chained list of (Elementary) Doc Entries
   ListDocEntry DocEntries;
   /// Chained list iterator, used to visit the TagHT variable
   ListDocEntry::iterator ItDocEntries;
   
   /// \brief pointer to the HTable of the Document,
   ///       (because we don't know it within any DicomDirObject nor any SQItem)
   // TagDocEntryHT *PtagHT;

private:

   /// \brief Sequences can be nested. This \ref SQDepthLevel represents
   ///        the level of the nesting of instances of this class.
   ///        \ref SQDepthLevel and its \ref SeqEntry::SQDepthLevel
   ///        counterpart are only defined on printing purposes
   ///        (see \ref Print).
   int SQDepthLevel;

   /// \brief A TagKey of a DocEntry nested in a sequence is prepended
   ///        with this BaseTagKey.
   BaseTagKey BaseTagKeyNested;

   /// \brief SQ Item ordinal number 
   int SQItemNumber;
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
