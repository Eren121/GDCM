/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSeqEntry.h,v $
  Language:  C++
  Date:      $Date: 2004/09/16 19:21:57 $
  Version:   $Revision: 1.17 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMSQDOCENTRY_H
#define GDCMSQDOCENTRY_H

#include <list>

#include "gdcmSQItem.h"
#include "gdcmDocEntry.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmSQItem *> ListSQItem;
//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmSeqEntry : public gdcmDocEntry 
{
public:
   gdcmSeqEntry( gdcmDictEntry* );
   gdcmSeqEntry(gdcmDocEntry* d,  int depth);
   virtual ~gdcmSeqEntry();
   
   virtual void Print(std::ostream &os = std::cout); 
   virtual void Write(FILE *fp, FileType);

   /// returns the SQITEM chained List for this SeQuence.
   ListSQItem &GetSQItems() { return items; }
      
   /// Sets the delimitor mode
   void SetDelimitorMode(bool dm) { delimitor_mode = dm;}

   /// Sets the Sequence Delimitation Item
   void SetSequenceDelimitationItem(gdcmDocEntry * e) { seq_term = e;}

   void AddEntry(gdcmSQItem *it, int itemNumber);
   gdcmSQItem *GetSQItemByOrdinalNumber(int itemNumber);

   /// Gets the depth level
   int GetDepthLevel() { return SQDepthLevel; }
                                                                                
   /// Sets the depth level of a Sequence Entry embedded in a SeQuence
   void SetDepthLevel(int depth) { SQDepthLevel = depth; }
protected:

private:
// Variables

   /// If this Sequence is in delimitor mode (length =0xffffffff) or not
   bool delimitor_mode;
   
   /// Chained list of SQ Items
   ListSQItem items;
   
   /// sequence terminator item 
   gdcmDocEntry *seq_term;

   /// \brief Defines the depth level of this \ref gdcmSeqEntry inside
   ///        the (optionaly) nested sequences. \ref SQDepthLevel
   ///        and its \ref gdcmSQItem::SQDepthLevel counterpart
   ///        are only defined on printing purposes (see \ref Print).
   int SQDepthLevel;
};

//-----------------------------------------------------------------------------
#endif

