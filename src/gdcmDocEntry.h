/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntry.h,v $
  Language:  C++
  Date:      $Date: 2004/08/31 14:24:47 $
  Version:   $Revision: 1.17 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDocEntry_H
#define GDCMDocEntry_H

#include <iostream>
#include <stdio.h>

#include "gdcmDictEntry.h"

class gdcmHeader;
class gdcmValEntry;
class gdcmBinEntry;
class gdcmSeqEntry;

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmDocEntry
 * \brief   The dicom header of a Dicom file contains a set of such entries
 *          (when successfuly parsed against a given Dicom dictionary)
 */
class GDCM_EXPORT gdcmDocEntry
{
public:
   gdcmDocEntry(gdcmDictEntry*);

   /// Returns the Dicom Group number of the current Dicom Header Entry
   uint16_t      GetGroup()     { return DictEntry->GetGroup();  };

   /// Returns the Dicom Element number of the current Dicom Header Entry
   uint16_t      GetElement()   { return DictEntry->GetElement();};

   /// Returns the 'key' of the current Dicom Header Entry
   std::string  GetKey()       { return DictEntry->GetKey();    };

   /// \brief Returns the 'Name' '(e.g. "Patient's Name") found in the Dicom
   /// Dictionnary of the current Dicom Header Entry
   std::string  GetName()      { return DictEntry->GetName();   };

   /// \brief Returns the 'Value Representation' (e.g. "PN" : Person Name,
   /// "SL" : Signed Long), found in the Dicom Header or in the Dicom
   /// Dictionnary, of the current Dicom Header Entry
   std::string  GetVR()        { return DictEntry->GetVR();     };

   /// \brief Returns offset (since the beginning of the file, including
   /// the File Pramble, if any) of the value of the current Dicom HeaderEntry
   /// \warning offset of the *value*, not of the Dicom Header Entry
   size_t       GetOffset()    { return Offset;             };

   /// \brief Returns the actual value length of the current Dicom Header Entry
   /// \warning this value is not *always* the one stored in the Dicom Header
   ///          in case of well knowned bugs
   uint32_t GetLength() { return UsableLength; };
    
   /// \brief Returns the 'read length' of the current Dicom Header Entry
   /// \warning this value is the one stored in the Dicom Header but not
   ///          mandatoryly the one thats's used (in case on SQ, or delimiters,
   ///          the usable length is set to zero)
   uint32_t GetReadLength() { return ReadLength; };

   /// Sets the 'Value Representation' of the current Dicom Header Entry
   void SetVR(std::string const & v) { DictEntry->SetVR(v); };    

   /// \brief Sets both 'Read Length' and 'Usable Length' of the current
   /// Dicom Header Entry
   void SetLength(uint32_t l) { ReadLength = UsableLength = l;};
      
   // The following 3 members, for internal use only ! 
   
   /// \brief Sets only 'Read Length' (*not* 'Usable Length') of the current
   /// Dicom Header Entry
   void SetReadLength(uint32_t l) { ReadLength = l; };

   /// \brief Sets only 'Usable Length' (*not* 'Read Length') of the current
   /// Dicom Header Entry
   void SetUsableLength(uint32_t l) { UsableLength = l; }; 
   
   /// \brief   Sets the offset of the Dicom Element
   /// \warning use with caution !
   /// @param   of offset to be set
   void SetOffset(size_t of) { Offset = of; };

   /// Sets to TRUE the ImplicitVr flag of the current Dicom Element
   void SetImplicitVR() { ImplicitVR = true; };
 
   /// \brief Tells us if the current Dicom Element was checked as ImplicitVr
   /// @return true if the current Dicom Element was checked as ImplicitVr
   bool IsImplicitVR() { return ImplicitVR; };

   /// \brief Tells us if the VR of the current Dicom Element is Unknown
   /// @return true if the VR is unkonwn
   bool IsVRUnknown() { return DictEntry->IsVRUnknown(); };

   /// \brief   Sets the DicEntry of the current Dicom Element
   /// @param   newEntry pointer to the DictEntry
   void SetDictEntry(gdcmDictEntry *newEntry) { DictEntry = newEntry; };

   /// \brief  Gets the DicEntry of the current Dicom Element
   /// @return The DicEntry of the current Dicom Element
   gdcmDictEntry * GetDictEntry() { return DictEntry; }; 

   /// \brief Sets the print level for the Dicom Header Elements
   /// \note 0 for Light Print; 1 for 'medium' Print, 2 for Heavy
   void SetPrintLevel(int level) { PrintLevel = level; };

   /// \brief Gets the print level for the Dicom Header Elements
   int GetPrintLevel() { return PrintLevel; };
   
   virtual void Print (std::ostream & os = std::cout); 
   virtual void Write(FILE *fp, FileType filetype);
   
   uint32_t GetFullLength();
   
   void Copy(gdcmDocEntry *doc);

   bool IsItemDelimitor();
   bool IsSequenceDelimitor();   

   /// \brief Gets the depth level of a Dicom header entry embedded in
   ///        a SeQuence
   int GetDepthLevel() { return SQDepthLevel; }

   /// \brief Sets the depth level of a Dicom header entry embedded in
   ///        a SeQuence
   void SetDepthLevel(int depth) { SQDepthLevel = depth; }

private:
   // FIXME: In fact we should be more specific and use :
   // friend gdcmDocEntry * gdcmHeader::ReadNextElement(void);
   friend class gdcmHeader;    

protected:
// Variables

   /// \brief pointer to the underlying Dicom dictionary element
   gdcmDictEntry *DictEntry;
   
   /// \brief Updated from ReadLength, by FixFoungLentgh() for fixing a bug
   /// in the header or helping the parser going on    
   uint32_t UsableLength; 
  
   /// \brief Length actually read on disk (before FixFoundLength). ReadLength
   /// will be updated only when FixFoundLength actually fixes a bug in the
   /// header, not when it performs a trick to help the Parser going on.
   uint32_t ReadLength;

   /// \brief Even when reading explicit vr files, some elements happen to
   /// be implicit. Flag them here since we can't use the entry->vr without
   /// breaking the underlying dictionary.
   bool ImplicitVR;

   /// Offset from the begining of file for direct user access
   size_t Offset; 

   /// How many details are to be printed (value : 0,1,2)      
   int PrintLevel;
   
   /// Gives the depth level of elements inside SeQuences   
   int SQDepthLevel;
};

//-----------------------------------------------------------------------------
#endif
