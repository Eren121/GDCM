/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSeqEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2004/09/13 12:10:53 $
  Version:   $Revision: 1.27 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
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
//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmSeqEntry
 * \brief   Constructor from a given gdcmSeqEntry
 */
gdcmSeqEntry::gdcmSeqEntry(gdcmDictEntry* e, int depth) 
             : gdcmDocEntry(e)
{
   UsableLength = 0;
   ReadLength = 0xffffffff;
   SQDepthLevel = depth;

   delimitor_mode = false;
   seq_term  = NULL;
}

/**
 * \brief   Constructor from a given gdcmSeqEntry
 * @param   e Pointer to existing Doc entry
 * @param   depth depth level of the current Seq entry
  */
gdcmSeqEntry::gdcmSeqEntry( gdcmDocEntry* e, int depth )
             : gdcmDocEntry( e->GetDictEntry() )
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
gdcmSeqEntry::~gdcmSeqEntry() {
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
void gdcmSeqEntry::Print(std::ostream &os)
{
   // First, Print the Dicom Element itself.
   SetPrintLevel(2);   
   gdcmDocEntry::Print(os);
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
      for (int i=0;i<SQDepthLevel+1;i++)
         os << "   | " ;
      if (seq_term != NULL) {
         seq_term->Print(os);
         os << std::endl;
      } 
      else 
         // fusible
         os << "      -------------- should have a sequence terminator item";
   }                    
}

/*
 * \brief   canonical Writer
 */
void gdcmSeqEntry::Write(FILE *fp, FileType filetype)
{
   uint16_t seq_term_gr = 0xfffe;
   uint16_t seq_term_el = 0xe0dd;
   uint32_t seq_term_lg = 0xffffffff;

   //uint16_t item_term_gr = 0xfffe;
   //uint16_t item_term_el = 0xe00d;
   
   gdcmDocEntry::Write(fp, filetype);
   for(ListSQItem::iterator cc  = GetSQItems().begin();
                            cc != GetSQItems().end();
                          ++cc)
   {        
      (*cc)->Write(fp, filetype);
   }
   
   // we force the writting of a Sequence Delimitation item
   // because we wrote the Sequence as a 'no Length' sequence
   fwrite ( &seq_term_gr,(size_t)2 ,(size_t)1 ,fp);
   fwrite ( &seq_term_el,(size_t)2 ,(size_t)1 ,fp);
   fwrite ( &seq_term_lg,(size_t)4 ,(size_t)1 ,fp); 
}

//-----------------------------------------------------------------------------
// Public

/// \brief   adds the passed ITEM to the ITEM chained List for this SeQuence.
void gdcmSeqEntry::AddEntry(gdcmSQItem *sqItem, int itemNumber)
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
gdcmSQItem *gdcmSeqEntry::GetSQItemByOrdinalNumber(int nb)
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
