/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntrySet.h,v $
  Language:  C++
  Date:      $Date: 2004/09/16 19:21:57 $
  Version:   $Revision: 1.18 $
                                                                                
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
 
typedef std::string gdcmBaseTagKey;
//-----------------------------------------------------------------------------

/**
 * \ref gdcmDocEntrySet is an abstract base class for \ref gdcmElementSet
 * and \ref gdcmSQItem which are both containers for gdcmDocEntries.
 * \ref gdcmElementSet is based on the STL map<> container
 * (see \ref gdcmElementSet::TagHT), as opposed to \ref gdcmSQItem
 * which is based on an STL list container (see \ref gdcmSQItem::docEntries).
 * Since the syntax for adding a new element to a map<> or a list<>
 * differ, \ref gdcmDocEntrySet is designed as an adapter to unify the
 * interfaces of \ref gdcmDocEntrySet and \ref gdcmElementSet.
 * As an illustration of this design, please refer to the implementation
 * of \ref AddEntry (or any pure virtual method) in both derived classes.
 * This adapter unification of interfaces enables the parsing of a
 * DICOM header containing (optionaly heavily nested) sequences to be
 * written recursively [see \ref gdcmDocument::ParseDES 
 * which calls \ref gdcmDocument::ParseSQ, which in turns calls 
 * \ref gdcmDocument::ParseDES ].
 *
 * \note Developpers should strongly resist to the temptation of adding
 *       members to this class since this class is designed as an adapter 
 *       in the form of an abstract base class.
 */
class GDCM_EXPORT gdcmDocEntrySet
{
public:

   gdcmDocEntrySet() {}
   virtual ~gdcmDocEntrySet() {}

   /// \brief adds any type of entry to the entry set (pure vitual)
   virtual bool AddEntry(gdcmDocEntry *Entry) = 0; // pure virtual
 
   /// \brief prints any type of entry to the entry set (pure vitual)
   virtual void Print (std::ostream & os = std::cout) = 0;// pure virtual

   /// \brief write any type of entry to the entry set
   virtual void Write (FILE *fp, FileType filetype) = 0;// pure virtual

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
   gdcmDocEntry* NewDocEntryByNumber(uint16_t group, 
                                     uint16_t element,
                                     std::string const & VR); 
   gdcmDocEntry* NewDocEntryByName  (std::string const & name);
   gdcmSeqEntry* NewSeqEntryByNumber(uint16_t group, 
                                     uint16_t element);

// DictEntry  related utilities
   gdcmDictEntry *GetDictEntryByName  (std::string const & name);
   gdcmDictEntry *GetDictEntryByNumber(uint16_t, uint16_t);

};


//-----------------------------------------------------------------------------
#endif

