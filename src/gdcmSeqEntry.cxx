/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSeqEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/24 16:10:53 $
  Version:   $Revision: 1.50 $
                                                                                
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
#include "gdcmDebug.h"

#include <iostream>
#include <iomanip>
#include <fstream>

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor from a given SeqEntry
 */
SeqEntry::SeqEntry( DictEntry *e ) 
             : DocEntry(e)
{
   Length       = 0;
   ReadLength   = 0xffffffff;
   SQDepthLevel = -1;

   DelimitorMode = false;
   SeqTerm  = NULL;
}

/**
 * \brief   Constructor from a given SeqEntry
 * @param   e Pointer to existing Doc entry
 * @param   depth depth level of the current Seq entry
 */
SeqEntry::SeqEntry( DocEntry *e, int depth )
             : DocEntry( e->GetDictEntry() )
{
   Length       = 0;
   ReadLength   = 0xffffffff;
   SQDepthLevel = depth;

   ImplicitVR   = e->IsImplicitVR();
   Offset       = e->GetOffset();
   SeqTerm = NULL;
}

/**
 * \brief   Canonical destructor.
 */
SeqEntry::~SeqEntry()
{
   for(ListSQItem::iterator cc = Items.begin(); cc != Items.end(); ++cc)
   {
      delete *cc;
   }
   if (SeqTerm)
   {
      delete SeqTerm;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   canonical Printer
 */
void SeqEntry::Print( std::ostream &os, std::string const & )
{
   // First, Print the Dicom Element itself.
   os << "S ";
   DocEntry::Print(os);
   os << std::endl;

   if (GetReadLength() == 0)
      return;

   // Then, Print each SQ Item   
   for(ListSQItem::iterator cc = Items.begin(); cc != Items.end(); ++cc)
   {
      (*cc)->SetPrintLevel(PrintLevel);
      (*cc)->Print(os);   
   }

   // at end, print the sequence terminator item, if any
   if (DelimitorMode)
   {
      for ( int i = 0; i < SQDepthLevel; i++ )
      {
         os << "   | " ;
      }
      if (SeqTerm != NULL)
      {
         SeqTerm->SetPrintLevel(PrintLevel);
         SeqTerm->Print(os);
         os << std::endl;
      } 
      else 
      {
         // fuse
         gdcmVerboseMacro("  -------- should have a sequence terminator item");
      }
   }                    
}

//-----------------------------------------------------------------------------
// Public
/*
 * \brief   canonical Writer
 * @param fp pointer to an already open file
 * @param filetype type of the file (ACR, ImplicitVR, ExplicitVR, ...)
 */
void SeqEntry::WriteContent(std::ofstream *fp, FileType filetype)
{
   uint16_t seq_term_gr = 0xfffe;
   uint16_t seq_term_el = 0xe0dd;
   uint32_t seq_term_lg = 0xffffffff;

   //uint16_t item_term_gr = 0xfffe;
   //uint16_t item_term_el = 0xe00d;
   
   DocEntry::WriteContent(fp, filetype);
   for(ListSQItem::iterator cc  = Items.begin();
                            cc != Items.end();
                          ++cc)
   {        
      (*cc)->WriteContent(fp, filetype);
   }
   
   // we force the writting of a Sequence Delimitation item
   // because we wrote the Sequence as a 'no Length' sequence
   binary_write(*fp, seq_term_gr);
   binary_write(*fp, seq_term_el);
   binary_write(*fp, seq_term_lg);
}

/**
 * \brief   Get the first entry while visiting the SeqEntry
 * \return  The first SQItem if found, otherwhise NULL
 */ 
SQItem *SeqEntry::GetFirstSQItem()
{
   ItSQItem = Items.begin();
   if (ItSQItem != Items.end())
      return *ItSQItem;
   return NULL;
} 

/**
 * \brief   Get the next SQItem while visiting the SeqEntry
 * \note : meaningfull only if GetFirstEntry already called
 * \return  The next SQItem if found, otherwhise NULL
 */

SQItem *SeqEntry::GetNextSQItem()
{
   gdcmAssertMacro (ItSQItem != Items.end())
   {
      ++ItSQItem;
      if (ItSQItem != Items.end())
         return *ItSQItem;
   }
   return NULL;
}
 
/**
 * \brief return a pointer to the SQItem referenced by its ordinal number.
 *        Returns the first item when argument is negative.
 *        Returns the last item when argument is bigger than the total
 *        item number.
 */
SQItem *SeqEntry::GetSQItem(int nb)
{
   if (nb<0)
   {
      return *(Items.begin());
   }
   int count = 0 ;
   for(ListSQItem::iterator cc = Items.begin();
                           cc != Items.end();
                           count ++, ++cc)
   {
      if (count == nb)
      {
         return *cc;
      }
   }
   return *(Items.end());
}

/// \brief retuens the number of SQItems within the current Sequence
unsigned int SeqEntry::GetNumberOfSQItems()
{
   return Items.size();
}

/// \brief   adds the passed ITEM to the ITEM chained List for this SeQuence.
void SeqEntry::AddSQItem(SQItem *sqItem, int itemNumber)
{
   sqItem->SetSQItemNumber(itemNumber);
   Items.push_back(sqItem);
}

//-----------------------------------------------------------------------------
// Protected


//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
} // end namespace gdcm

