/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSeqEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/24 18:03:14 $
  Version:   $Revision: 1.20 $
                                                                                
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
   delimitor_mode = false;
   seq_term  = NULL;
   SQDepthLevel = depth;
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

/*
 * \brief   canonical Printer
 */
void gdcmSeqEntry::Print(std::ostream &os){

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
   guint16 seq_term_gr = 0xfffe;
   guint16 seq_term_el = 0xe0dd;
   guint32 seq_term_lg = 0x00000000;

   guint16 item_term_gr = 0xfffe;
   guint16 item_term_el = 0xe00d;

   gdcmDocEntry::Write(fp, filetype);
   for(ListSQItem::iterator cc  = GetSQItems().begin();
                            cc != GetSQItems().end();
                          ++cc)
   {
      (*cc)->Write(fp, filetype);

   fwrite ( &item_term_gr,(size_t)2 ,(size_t)1 ,fp);
   fwrite ( &item_term_el,(size_t)2 ,(size_t)1 ,fp);   
   fwrite ( &seq_term_lg,(size_t)4 ,(size_t)1 ,fp); 
   }
    //we force the writting of a Sequence Delimitaion item
    // because we wrote the Sequence as a 'no Length' sequence
   fwrite ( &seq_term_gr,(size_t)2 ,(size_t)1 ,fp);
   fwrite ( &seq_term_el,(size_t)2 ,(size_t)1 ,fp);
   fwrite ( &seq_term_lg,(size_t)4 ,(size_t)1 ,fp); 
}

//-----------------------------------------------------------------------------
// Public

 /// \brief   adds the passed ITEM to the ITEM chained List for this SeQuence.      
void gdcmSeqEntry::AddEntry(gdcmSQItem *sqItem, int itemNumber) {
   sqItem->SetSQItemNumber(itemNumber);
   items.push_back(sqItem);
}

/// \brief Sets the depth level of a Sequence Entry embedded in a SeQuence 
void gdcmSeqEntry::SetDepthLevel(int depth) {
   SQDepthLevel = depth;
}

/// \brief return a pointer to the SQItem referenced by its ordinal number
/// (returns the first one if ordinal number is <0
///  returns the last  one if ordinal number is > item number

gdcmSQItem *gdcmSeqEntry::GetSQItemByOrdinalNumber(int nb) {
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
