/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmElementSet.h,v $
  Language:  C++
  Date:      $Date: 2004/10/12 04:35:46 $
  Version:   $Revision: 1.19 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMELEMENTSET_H
#define GDCMELEMENTSET_H

#include "gdcmCommon.h"
#include "gdcmDocEntrySet.h"
#include <map>
#include <iostream>

class ValEntry;
class BinEntry;
class SeqEntry;
namespace gdcm 
{


typedef std::map<TagKey, DocEntry *> TagDocEntryHT;

//-----------------------------------------------------------------------------

class GDCM_EXPORT ElementSet : public DocEntrySet
{
public:
   ElementSet(int);
   ~ElementSet();
   virtual bool AddEntry(DocEntry *Entry);
   bool RemoveEntry(DocEntry *EntryToRemove);
   bool RemoveEntryNoDestroy(DocEntry *EntryToRemove);
   
   virtual void Print(std::ostream &os = std::cout); 
   virtual void Write(FILE *fp, FileType filetype); 

   /// Accessor to \ref TagHT
   // Do not expose this to user (public API) !
   // I re-add it temporaryly JPRx
   TagDocEntryHT &GetEntry() { return TagHT; };


protected:
// Variables
   /// Hash Table (map), to provide fast access
   TagDocEntryHT TagHT; 
     
private:
   /// Just for following ::GetTagHT()
   friend class Document;

   /// Accessor to \ref TagHT
   TagDocEntryHT* GetTagHT() { return &TagHT; };
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif

