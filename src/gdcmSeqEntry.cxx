/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSeqEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/22 03:05:42 $
  Version:   $Revision: 1.32 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmTS.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"

#include <iostream>
#include <iomanip>
#include <fstream>

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup SeqEntry
 * \brief   Constructor from a given SeqEntry
 */
SeqEntry::SeqEntry( DictEntry* e ) 
             : DocEntry(e)
{
   UsableLength = 0;
   ReadLength = 0xffffffff;
   SQDepthLevel = -1;

   delimitor_mode = false;
   seq_term  = NULL;
}

/**
 * \brief   Constructor from a given SeqEntry
 * @param   e Pointer to existing Doc entry
 * @param   depth depth level of the current Seq entry
  */
SeqEntry::SeqEntry( DocEntry* e, int depth )
             : DocEntry( e->GetDictEntry() )
{
   this->UsableLength = 0;
   this->ReadLength   = 0xffffffff;
   SQDepthLevel = depth;

   this->ImplicitVR   = e->IsImplicitVR();
   this->Offset       = e->GetOffset();
}

/**
 * \brief   Canonical destructor.
 */
SeqEntry::~SeqEntry() {
   for(ListSQItem::iterator cc = items.begin();cc != items.end();++cc)
   {
      delete *cc;
   }
   if (!seq_term)
      delete seq_term;
}

/**
 * \brief   canonical Printer
 */
void SeqEntry::Print( std::ostream &os )
{
   // First, Print the Dicom Element itself.
   SetPrintLevel(2);   
   DocEntry::Print(os);
   os << std::endl;

   if (GetReadLength() == 0)
      return;

   // Then, Print each SQ Item   
   for(ListSQItem::iterator cc = items.begin();cc != items.end();++cc)
   {
      (*cc)->Print(os);   
   }

   // at end, print the sequence terminator item, if any
   if (delimitor_mode) {
      for ( int i = 0; i < SQDepthLevel; i++ )
      {
         os << "   | " ;
      }
      if (seq_term != NULL)
      {
         seq_term->Print(os);
         os << std::endl;
      } 
      else 
      {
         // fuse
         os << "      -------------- should have a sequence terminator item";
      }
   }                    
}

/*
 * \brief   canonical Writer
 */
void SeqEntry::Write(std::ofstream* fp, FileType filetype)
{
   uint16_t seq_term_gr = 0xfffe;
   uint16_t seq_term_el = 0xe0dd;
   uint32_t seq_term_lg = 0xffffffff;

   //uint16_t item_term_gr = 0xfffe;
   //uint16_t item_term_el = 0xe00d;
   
   DocEntry::Write(fp, filetype);
   for(ListSQItem::iterator cc  = GetSQItems().begin();
                            cc != GetSQItems().end();
                          ++cc)
   {        
      (*cc)->Write(fp, filetype);
   }
   
   // we force the writting of a Sequence Delimitation item
   // because we wrote the Sequence as a 'no Length' sequence
   fp->write ( (char*)&seq_term_gr,(size_t)2 );
   fp->write ( (char*)&seq_term_el,(size_t)2 );
   fp->write ( (char*)&seq_term_lg,(size_t)4 );
}

//-----------------------------------------------------------------------------
// Public

/// \brief   adds the passed ITEM to the ITEM chained List for this SeQuence.
void SeqEntry::AddEntry(SQItem *sqItem, int itemNumber)
{
   sqItem->SetSQItemNumber(itemNumber);
   items.push_back(sqItem);
}

/**
 * \brief return a pointer to the SQItem referenced by its ordinal number.
 *        Returns the first item when argument is negative.
 *        Returns the last item when argument is bigget than the total
 *        item number.
 */
SQItem* SeqEntry::GetSQItemByOrdinalNumber(int nb)
{
   if (nb<0)
      return (*(items.begin()));
   int count = 0 ;
   for(ListSQItem::iterator cc = items.begin();
       cc != items.end();
       count ++, ++cc){
      if (count==nb)
         return *cc;
   }
   return (*(items.end())); // Euhhhhh ?!? Is this the last one . FIXME
}
//-----------------------------------------------------------------------------
// Protected


//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
} // end namespace gdcm

