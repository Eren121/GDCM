/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSeqEntry.h,v $
  Language:  C++
  Date:      $Date: 2004/06/22 13:47:33 $
  Version:   $Revision: 1.12 $
                                                                                
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
   gdcmSeqEntry(gdcmDictEntry* e, int depth);
   virtual ~gdcmSeqEntry();
   
   virtual void Print(std::ostream &os = std::cout); 
   virtual void Write(FILE *fp,FileType filetype);

   /// \brief   returns the SQITEM chained List for this SeQuence.
   inline ListSQItem &GetSQItems() { return items; }
      
   /// \brief Sets the delimitor mode
   inline void SetDelimitorMode(bool dm) { delimitor_mode = dm;}

   /// \brief Sets the Sequence Delimitation Item
   inline void SetSequenceDelimitationItem(gdcmDocEntry * e) { seq_term = e;}

   void AddEntry(gdcmSQItem *it, int itemNumber);
   gdcmSQItem *GetSQItemByOrdinalNumber(int itemNumber);

   void SetDepthLevel(int depth);
         
protected:

private:
// Variables

   /// \brief If this Sequence is in delimitor mode (length =0xffffffff) or not
   bool delimitor_mode;
   
   /// \brief Chained list of SQ Items
   ListSQItem items;
   
   /// \brief sequence terminator item 
   gdcmDocEntry *seq_term;
};

//-----------------------------------------------------------------------------
#endif

