/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSQItem.h,v $
  Language:  C++
  Date:      $Date: 2004/06/22 13:47:33 $
  Version:   $Revision: 1.10 $
                                                                                
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
   inline ListDocEntry &GetDocEntries() {return docEntries;};   
   
   /// \brief   adds the passed DocEntry to the DocEntry chained List for
   /// this SQ Item.      
   inline void AddDocEntry(gdcmDocEntry *e) {docEntries.push_back(e);};         

   virtual bool AddEntry(gdcmDocEntry *Entry); // add to the List
  
   gdcmDocEntry *GetDocEntryByNumber(guint16 group, guint16 element);
   gdcmDocEntry *GetDocEntryByName  (std::string Name);
   
   bool SetEntryByNumber(std::string val,guint16 group, guint16 element);                   
    
   virtual std::string GetEntryByNumber(guint16 group, guint16 element);

   inline int GetSQItemNumber()
      {return SQItemNumber;};

   inline void SetSQItemNumber(int itemNumber)
      {SQItemNumber=itemNumber;};     

protected:

// DocEntry related utilities 

	
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
