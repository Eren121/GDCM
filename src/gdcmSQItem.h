/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSQItem.h,v $
  Language:  C++
  Date:      $Date: 2004/07/19 11:51:26 $
  Version:   $Revision: 1.12 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#ifndef GDCMSQITEM_H
#define GDCMSQITEM_H

#include <list>
#include "gdcmDocEntry.h"
#include "gdcmDocEntrySet.h"
#include "gdcmDocument.h"


class gdcmBinEntry;

//-----------------------------------------------------------------------------
typedef std::list<gdcmDocEntry *> ListDocEntry;
//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmSQItem : public gdcmDocEntrySet
{
public:
   gdcmSQItem(int);
   ~gdcmSQItem(void);

   virtual void Print(std::ostream &os = std::cout); 
   virtual void Write(FILE *fp, FileType filetype);

   /// \brief   returns the DocEntry chained List for this SQ Item.
   ListDocEntry &GetDocEntries() { return docEntries; };
   
   /// \brief   adds the passed DocEntry to the DocEntry chained List for
   /// this SQ Item.      
   void AddDocEntry(gdcmDocEntry *e) { docEntries.push_back(e); };

   virtual bool AddEntry(gdcmDocEntry *Entry); // add to the List
  
   gdcmDocEntry *GetDocEntryByNumber(uint16_t group, uint16_t element);
   // FIXME method to write
   //gdcmDocEntry *GetDocEntryByName  (std::string Name);
   
   bool SetEntryByNumber(std::string val, uint16_t group, uint16_t element);                   
    
   virtual std::string GetEntryByNumber(uint16_t group, uint16_t element);

   int GetSQItemNumber() { return SQItemNumber; };

   void SetSQItemNumber(int itemNumber) { SQItemNumber=itemNumber; };

protected:

// Variables

   /// \brief chained list of (Elementary) Doc Entries
   ListDocEntry docEntries;
  
   /// \brief SQ Item ordinal number 
   int SQItemNumber;

   ///\brief pointer to the HTable of the gdcmDocument,
   ///       (because we don't know it within any gdcmObject nor any gdcmSQItem)
   TagDocEntryHT *ptagHT;
       
private:


};

//-----------------------------------------------------------------------------
#endif
