/*=========================================================================
  
  Program:   gdcm
  Module:    $RCSfile: gdcmSQItem.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/25 12:54:35 $
  Version:   $Revision: 1.19 $
  
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
  
=========================================================================*/

#include "gdcmSQItem.h"
#include "gdcmSeqEntry.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"

#include "gdcmDebug.h"


//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmSQItem
 * \brief   Constructor from a given gdcmSQItem
 */
gdcmSQItem::gdcmSQItem(int depthLevel ) 
          : gdcmDocEntrySet(depthLevel) {
   SQDepthLevel = depthLevel +1;
}

/**
 * \brief   Canonical destructor.
 */
gdcmSQItem::~gdcmSQItem() 
{
   for(ListDocEntry::iterator cc = docEntries.begin();
       cc != docEntries.end();
       ++cc)
   {
      delete (*cc);
   }
   docEntries.clear();
}

//-----------------------------------------------------------------------------
// Print
/*
 * \brief   canonical Printer
 */
 void gdcmSQItem::Print(std::ostream & os) {
   std::ostringstream s;

   if (SQDepthLevel>0)
   {
      for (int i=0;i<SQDepthLevel;i++)
         s << "   | " ;
   }
   std::cout << s.str() << " --- SQItem number " << SQItemNumber  << std::endl;
   for (ListDocEntry::iterator i  = docEntries.begin();  
                               i != docEntries.end();
                             ++i)
   {
      gdcmDocEntry* Entry = *i;
      bool PrintEndLine = true;

      os << s.str();
      Entry->SetPrintLevel(2);
      Entry->Print(os);   
      if ( gdcmSeqEntry* SeqEntry = dynamic_cast<gdcmSeqEntry*>(Entry) )
      {
         (void)SeqEntry;  //not used
         PrintEndLine = false;
      }
      if (PrintEndLine)
         os << std::endl;
   } 
}

/*
 * \ingroup gdcmSQItem
 * \brief   canonical Writer
 */
void gdcmSQItem::Write(FILE *fp,FileType filetype)
{
   for (ListDocEntry::iterator i = docEntries.begin();  
        i != docEntries.end();
        ++i)
   {
      // Item Delimitor Item IS the last one of a 'no length' SQItem
      // (when it exists) we don't write it right now
      // It will be written outside, because ALL the SQItems are written
      // as 'no length'
      if ( (*i)->isItemDelimitor() )
         break;

      // Fix in order to make some MR PHILIPS images e-film readable
      // see gdcmData/gdcm-MR-PHILIPS-16-Multi-Seq.dcm:
      // we just *always* ignore spurious fffe|0000 tag ! 
      if ( ((*i)->GetGroup() == 0xfffe) && ((*i)->GetElement() == 0x0000) )  
         break; 

      // It's up to the gdcmDocEntry Writter to write the SQItem begin element
      // (fffe|e000) as a 'no length' one
      (*i)->Write(fp, filetype);
   } 
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   adds any Entry (Dicom Element) to the Sequence Item
 */
bool gdcmSQItem::AddEntry(gdcmDocEntry *entry)
{
   docEntries.push_back(entry);
   //TODO : check if it worked
   return true;
}   

/**
 * \brief   Sets Entry (Dicom Element) value of an element,
 *          specified by it's tag (Group, Number) 
 *          and the length, too ...  inside a SQ Item
 *          If the Element is not found, it's just created !
 * \warning we suppose, right now, the element belongs to a Public Group
 *          (NOT a shadow one)       
 * @param   val string value to set
 * @param   group Group number of the searched tag.
 * @param   element Element number of the searched tag.
 * @return  true if element was found or created successfully
 */

bool gdcmSQItem::SetEntryByNumber(std::string val,guint16 group, 
                                  guint16 element)
{
   for(ListDocEntry::iterator i=docEntries.begin();i!=docEntries.end();++i)
   { 
      if ( (*i)->GetGroup() == 0xfffe && (*i)->GetElement() == 0xe000 ) 
         continue;

      if (  ( group   < (*i)->GetGroup() )
          ||( group == (*i)->GetGroup() && element < (*i)->GetElement()) )
      {
         // instead of ReplaceOrCreateByNumber 
         // that is a method of gdcmDocument :-( 
         gdcmValEntry* Entry = (gdcmValEntry*)0;
         TagKey key = gdcmDictEntry::TranslateToKey(group, element);

         if ( ! ptagHT->count(key))
         {
            // we assume a Public Dictionnary *is* loaded
            gdcmDict *PubDict = gdcmGlobal::GetDicts()->GetDefaultPubDict();
            // if the invoked (group,elem) doesn't exist inside the Dictionary
            // we create a VirtualDictEntry
            gdcmDictEntry *DictEntry = PubDict->GetDictEntryByNumber(group,
                                                                     element);
            if (DictEntry == NULL)
            {
              DictEntry=gdcmGlobal::GetDicts()->NewVirtualDictEntry(group,
                                                                    element,
                                                                    "UN",
                                                                    "??","??");
            } 
            // we assume the constructor didn't fail
            Entry = new gdcmValEntry(DictEntry);
            /// \todo
            /// ----
            /// better we don't assume too much !
            /// gdcmSQItem is now used to describe any DICOMDIR related object
         } else {
            gdcmDocEntry* FoundEntry = ptagHT->find(key)->second;
            Entry = dynamic_cast<gdcmValEntry*>(FoundEntry);
            if (!Entry) 
               dbg.Verbose(0, "gdcmSQItem::SetEntryByNumber: docEntries"
                              " contains non gdcmValEntry occurences");
         }
         if (Entry)
            Entry->SetValue(val); 
         Entry->SetLength(val.length());
         docEntries.insert(i,Entry); 
         return true;
      }   
      if (group == (*i)->GetGroup() && element == (*i)->GetElement() )
      {
         if ( gdcmValEntry* Entry = dynamic_cast<gdcmValEntry*>(*i) )
            Entry->SetValue(val);
         (*i)->SetLength(val.length()); 
         return true;    
      }
   }
   return false;
}
//-----------------------------------------------------------------------------
// Protected


/**
 * \brief   Gets a Dicom Element inside a SQ Item Entry, by number
 * @return
 */
gdcmDocEntry *gdcmSQItem::GetDocEntryByNumber(guint16 group, guint16 element) {
   for(ListDocEntry::iterator i=docEntries.begin();i!=docEntries.end();++i) {
      if ( (*i)->GetGroup()==group && (*i)->GetElement()==element)
         return (*i);
   }   
   return NULL;
}

/**
 * \brief   Get the value of a Dicom Element inside a SQ Item Entry, by number
 * @return
 */ 

std::string gdcmSQItem::GetEntryByNumber(guint16 group, guint16 element) { 
   for(ListDocEntry::iterator i=docEntries.begin();i!=docEntries.end();++i) {
      if ( (*i)->GetGroup()==group && (*i)->GetElement()==element) {
         return ((gdcmValEntry *)(*i))->GetValue();
      }
   }   
   return GDCM_UNFOUND;
}
//-----------------------------------------------------------------------------
// Private


//-----------------------------------------------------------------------------
