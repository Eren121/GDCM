/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmElementSet.h,v $
  Language:  C++
  Date:      $Date: 2004/10/07 18:09:11 $
  Version:   $Revision: 1.18 $
                                                                                
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

class gdcmValEntry;
class gdcmBinEntry;
class gdcmSeqEntry;


typedef std::map<gdcmTagKey, gdcmDocEntry *> TagDocEntryHT;

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmElementSet : public gdcmDocEntrySet
{
public:
   gdcmElementSet(int);
   ~gdcmElementSet();
   virtual bool AddEntry(gdcmDocEntry *Entry);
   bool RemoveEntry(gdcmDocEntry *EntryToRemove);
   bool RemoveEntryNoDestroy(gdcmDocEntry *EntryToRemove);
   
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
   friend class gdcmDocument;

   /// Accessor to \ref TagHT
   TagDocEntryHT* GetTagHT() { return &TagHT; };
};

//-----------------------------------------------------------------------------
#endif

