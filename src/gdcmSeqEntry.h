/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSeqEntry.h,v $
  Language:  C++
  Date:      $Date: 2005/01/11 15:15:38 $
  Version:   $Revision: 1.27 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMSQDOCENTRY_H
#define GDCMSQDOCENTRY_H

#include "gdcmDocEntry.h"

#include <list>

namespace gdcm 
{
class SQItem;
//-----------------------------------------------------------------------------
typedef std::list<SQItem *> ListSQItem;
//-----------------------------------------------------------------------------
/**
 * \brief a SeqEntry (as opposed to a ValEntry) is a non elementary DocEntry.
 *        It is composed by a set of SQItems.
 *        Each SQItem is composed by a set of DocEntry
 *        A DocEntry may be a SeqEntry
 *        ... and so forth 
 */ 
class GDCM_EXPORT SeqEntry : public DocEntry 
{
public:
   SeqEntry( DictEntry *e);
   SeqEntry( DocEntry *d, int depth );
   ~SeqEntry();
   
   void Print(std::ostream &os = std::cout); 
   void WriteContent(std::ofstream *fp, FileType filetype);

   /// returns the SQITEM chained List for this SeQuence.
   ListSQItem const &GetSQItems() const { return Items; }
      
   /// Sets the delimitor mode
   void SetDelimitorMode(bool dm) { DelimitorMode = dm; }

   /// Sets the Sequence Delimitation Item
   void SetSequenceDelimitationItem(DocEntry *e) { SeqTerm = e;}

   void AddEntry(SQItem *it, int itemNumber);
   SQItem *GetSQItemByOrdinalNumber(int itemNumber);

   /// Gets the depth level
   int GetDepthLevel() const { return SQDepthLevel; }

   /// Sets the depth level of a Sequence Entry embedded in a SeQuence
   void SetDepthLevel(int depth) { SQDepthLevel = depth; }

protected:

private:
// Variables

   /// If this Sequence is in delimitor mode (length =0xffffffff) or not
   bool DelimitorMode;
   
   /// Chained list of SQ Items
   ListSQItem Items;
   
   /// sequence terminator item 
   DocEntry *SeqTerm;

   /// \brief Defines the depth level of this \ref SeqEntry inside
   ///        the (optionaly) nested sequences. \ref SQDepthLevel
   ///        and its \ref SQItem::SQDepthLevel counterpart
   ///        are only defined on printing purposes (see \ref Print).
   int SQDepthLevel;
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif

