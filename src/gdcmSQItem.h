/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSQItem.h,v $
  Language:  C++
  Date:      $Date: 2004/10/12 04:35:47 $
  Version:   $Revision: 1.19 $
                                                                                
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

//#include "gdcmDocEntry.h"
//#include "gdcmDocument.h"
//#include "gdcmBinEntry.h"

#include <list>

namespace gdcm 
{
class DocEntry;

//-----------------------------------------------------------------------------
typedef std::list<DocEntry *> ListDocEntry;
//-----------------------------------------------------------------------------
class GDCM_EXPORT SQItem 
  : 
  public DocEntrySet 
{
public:
   SQItem(int);
   ~SQItem();

   virtual void Print(std::ostream &os = std::cout); 
   virtual void Write(FILE *fp, FileType filetype);

   /// \brief   returns the DocEntry chained List for this SQ Item.
   ListDocEntry &GetDocEntries() { return docEntries; };
   
   /// \brief   adds the passed DocEntry to the DocEntry chained List for
   /// this SQ Item.      
   void AddDocEntry(DocEntry *e) { docEntries.push_back(e); };

   virtual bool AddEntry(DocEntry *Entry); // add to the List
  
   DocEntry *GetDocEntryByNumber(uint16_t group, uint16_t element);
   // FIXME method to write
   //DocEntry *GetDocEntryByName  (std::string Name);
   
   bool SetEntryByNumber(std::string val, uint16_t group, uint16_t element);                   
    
   virtual std::string GetEntryByNumber(uint16_t group, uint16_t element);

   /// \brief   returns the ordinal position of a given SQItem
   int GetSQItemNumber() { return SQItemNumber; };

   /// \brief   Sets the ordinal position of a given SQItem
   void SetSQItemNumber(int itemNumber) { SQItemNumber = itemNumber; };

   /// Accessor on \ref SQDepthLevel.
   int GetDepthLevel() { return SQDepthLevel; }
                                                                                
   /// Accessor on \ref SQDepthLevel.
   void SetDepthLevel(int depth) { SQDepthLevel = depth; }

   /// Accessor on \ref BaseTagKey.
   void           SetBaseTagKey( BaseTagKey key ) { BaseTagKeyNested = key; }

   /// Accessor on \ref BaseTagKey.
   BaseTagKey GetBaseTagKey( ) { return BaseTagKeyNested; }


protected:

// Variables

   /// \brief chained list of (Elementary) Doc Entries
   ListDocEntry docEntries;
  
   /// \brief SQ Item ordinal number 
   int SQItemNumber;

   ///\brief pointer to the HTable of the Document,
   ///       (because we don't know it within any DicomDirObject nor any SQItem)
   TagDocEntryHT *PtagHT;

       
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

};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
