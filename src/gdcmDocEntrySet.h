/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntrySet.h,v $
  Language:  C++
  Date:      $Date: 2005/01/20 11:39:49 $
  Version:   $Revision: 1.39 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDOCENTRYSET_H
#define GDCMDOCENTRYSET_H

#include "gdcmBase.h"
#include "gdcmException.h"
#include <fstream>

namespace gdcm 
{
class DocEntry;
class ValEntry;
class BinEntry;
class SeqEntry;
class DictEntry;

typedef std::string BaseTagKey;
//-----------------------------------------------------------------------------

/**
 * \brief
 * \ref DocEntrySet is an abstract base class for \ref ElementSet
 * and \ref SQItem which are both containers for DocEntries.
 * \ref ElementSet is based on the STL map<> container
 * (see \ref ElementSet::TagHT), as opposed to \ref SQItem
 * which is based on an STL list container (see \ref ListDocEntry).
 * Since the syntax for adding a new element to a map<> or a list<>
 * differ, \ref DocEntrySet is designed as an adapter to unify the
 * interfaces of \ref DocEntrySet and \ref ElementSet.
 * As an illustration of this design, please refer to the implementation
 * of \ref AddEntry (or any pure virtual method) in both derived classes.
 * This adapter unification of interfaces enables the parsing of a
 * DICOM header containing (optionaly heavily nested) sequences to be
 * written recursively [see \ref Document::ParseDES 
 * which calls \ref Document::ParseSQ, which in turns calls 
 * \ref Document::ParseDES ].
 *
 * \note Developpers should strongly resist to the temptation of adding
 *       members to this class since this class is designed as an adapter 
 *       in the form of an abstract base class.
 */
class GDCM_EXPORT DocEntrySet : public Base
{
friend class File;
public:
   DocEntrySet() {};
   virtual ~DocEntrySet() {};

   // ------- '... =0;' stands for 'Pure Virtual'

   /// \brief adds any type of entry to the entry set
   virtual bool AddEntry(DocEntry *Entry) = 0;

   /// \brief Removes any type of entry out of the entry set, and destroys it
   virtual bool RemoveEntry(DocEntry *EntryToRemove)=0;

   /// \brief Removes any type of entry out of the entry set, DOESN'T destroy it
   virtual bool RemoveEntryNoDestroy(DocEntry *EntryToRemove)= 0;

   /// \brief write any type of entry to the entry set
   virtual void WriteContent (std::ofstream *fp, 
                              FileType filetype) = 0;

   /// \brief Gets any type of DocEntry, identified by its (group,elem)
   virtual DocEntry *GetDocEntry(uint16_t group,
                                 uint16_t elem) = 0;

   /// \brief Gets a ValEntry, identified by its (group,elem)
   virtual ValEntry *GetValEntry(uint16_t group,
                                 uint16_t elem) = 0;
   /// \brief Gets a BinEntry, identified by its (group,elem)
   virtual BinEntry *GetBinEntry(uint16_t group,
                                 uint16_t elem) = 0;

   /// \brief Gets a SeqEntry, identified by its (group,elem)
   virtual SeqEntry *GetSeqEntry(uint16_t group,
                                 uint16_t elem) = 0;

   /// \brief Gets the 'string value' of a ValEntry
   ///        identified by its (group,elem) - Sorry for the name !...-
   virtual std::string GetEntry(uint16_t group, uint16_t elem) = 0;

   DictEntry *NewVirtualDictEntry(uint16_t group, 
                                  uint16_t elem,
                                  TagName const &vr     = GDCM_UNKNOWN,
                                  TagName const &vm     = GDCM_UNKNOWN,
                                  TagName const &name   = GDCM_UNKNOWN );

protected:
// DocEntry  related utilities 

   ValEntry *NewValEntry(uint16_t group,uint16_t elem,
                         TagName const &vr = GDCM_UNKNOWN);
   BinEntry *NewBinEntry(uint16_t group,uint16_t elem,
                         TagName const &vr = GDCM_UNKNOWN);
   SeqEntry *NewSeqEntry(uint16_t group,uint16_t elem);

// DictEntry  related utilities
   DictEntry *GetDictEntry(uint16_t group, uint16_t elem);
   DictEntry *GetDictEntry(uint16_t group, uint16_t elem,
                           TagName const &vr);
};

} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif

