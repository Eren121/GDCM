/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmElementSet.h,v $
  Language:  C++
  Date:      $Date: 2004/07/02 13:55:28 $
  Version:   $Revision: 1.10 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMELEMENTSET_H
#define GDCMELEMENTSET_H

#include <map>
#include <iostream>
#include "gdcmCommon.h"
#include "gdcmDocEntrySet.h"

class gdcmValEntry;
class gdcmBinEntry;
class gdcmSeqEntry;


typedef std::map<gdcmTagKey, gdcmDocEntry *> TagDocEntryHT;

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmElementSet : public gdcmDocEntrySet
{
public:
   gdcmElementSet(int);
   ~gdcmElementSet(void);
   virtual bool AddEntry(gdcmDocEntry *Entry);
   virtual bool RemoveEntry(gdcmDocEntry *EntryToRemove);

   virtual void Print(std::ostream &os = std::cout); 
   virtual void Write(FILE *fp, FileType filetype); 

    
protected:
// Variables
   /// Hash Table (map), to provide fast access
   TagDocEntryHT tagHT; 
     
private:
   
};

//-----------------------------------------------------------------------------
#endif

