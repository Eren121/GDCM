/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntrySet.h,v $
  Language:  C++
  Date:      $Date: 2004/08/26 15:29:52 $
  Version:   $Revision: 1.15 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDOCENTRYSET_H
#define GDCMDOCENTRYSET_H

#include "gdcmException.h"
#include "gdcmDocEntry.h"
 
//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmDocEntrySet
{
public:

   gdcmDocEntrySet(int depth = 0); 
   virtual ~gdcmDocEntrySet();

   /// \brief adds any type of entry to the entry set (pure vitual)
   virtual bool AddEntry(gdcmDocEntry *Entry) = 0; // pure virtual
 
   /// \brief prints any type of entry to the entry set (pure vitual)
   virtual void Print (std::ostream & os = std::cout) = 0;// pure virtual

   /// \brief write any type of entry to the entry set
   virtual void Write (FILE *fp, FileType filetype) = 0;// pure virtual

   /// \brief Gets the depth level of a Dicom Header Entry embedded in a
   ///        SeQuence
   int GetDepthLevel() { return SQDepthLevel; }

   /// \brief Sets the depth level of a Dicom Header Entry embedded in a
   /// SeQuence
   void SetDepthLevel(int depth) { SQDepthLevel = depth; }

   virtual gdcmDocEntry* GetDocEntryByNumber(uint16_t group,
                                             uint16_t element) = 0;
   gdcmDocEntry *GetDocEntryByName(std::string const & name);
   virtual std::string GetEntryByNumber(uint16_t group,uint16_t element) = 0;
   std::string GetEntryByName(TagName const & name);
   gdcmDictEntry *NewVirtualDictEntry(uint16_t group, 
                                      uint16_t element,
                                      std::string const & vr     = "unkn",
                                      std::string const & fourth = "unkn",
                                      std::string const & name   = "unkn");
  
protected:

// DocEntry  related utilities 
   gdcmValEntry* NewValEntryByNumber(uint16_t group, 
                                     uint16_t element);
   gdcmBinEntry* NewBinEntryByNumber(uint16_t group, 
                                     uint16_t element);
   gdcmDocEntry* NewDocEntryByNumber(uint16_t group, 
                                     uint16_t element); 
   gdcmDocEntry* NewDocEntryByName  (std::string const & name);
   gdcmSeqEntry* NewSeqEntryByNumber(uint16_t group, 
                                     uint16_t element);

// DictEntry  related utilities
   gdcmDictEntry *GetDictEntryByName  (std::string const & name);
   gdcmDictEntry *GetDictEntryByNumber(uint16_t, uint16_t);

   /// Gives the depth level of the element set inside SeQuences   
   int SQDepthLevel;
private:
};


//-----------------------------------------------------------------------------
#endif

