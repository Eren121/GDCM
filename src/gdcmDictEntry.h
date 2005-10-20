/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDictEntry.h,v $
  Language:  C++
  Date:      $Date: 2005/10/20 15:24:08 $
  Version:   $Revision: 1.39 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICTENTRY_H
#define GDCMDICTENTRY_H

#include "gdcmDicomEntry.h"

namespace gdcm 
{
//-----------------------------------------------------------------------------
/**
 * \brief
 * the DictEntry in an element contained by the Dict.
 * It contains :
 *  - the key referenced by the DICOM norm or the constructor (for private keys)
 *    i.e. the Group number
 *         the Element number
 *  - the VR (Value Representation)
 *  - the VM (Value Multplicity)
 *  - the corresponding name in english
 */
class GDCM_EXPORT DictEntry : public DicomEntry
{
public:
   gdcmTypeMacro(DictEntry);

public:
   static DictEntry *New(uint16_t group, uint16_t elem,
             VRKey const &vr       = GDCM_VRUNKNOWN,
             TagName const &vm     = GDCM_UNKNOWN,
             TagName const &name   = GDCM_UNKNOWN);

// Print
   void Print(std::ostream &os = std::cout, std::string const &indent = "");

// Content of DictEntry
   virtual void SetVR(VRKey const &vr);
   virtual void SetVM(TagName const &vm);
   /// \brief   returns the VM field of the current DictEntry
   /// @return  The 'Value Multiplicity' field
   const TagName &GetVM() const { return VM; } 

   /// \brief tells if the V(alue) M(ultiplicity) is known (?!)
   /// @return 
   bool IsVMUnknown() const { return VM == GDCM_UNKNOWN; }

   /// \brief  Returns the Dicom Name of the current DictEntry
   ///         e.g. "Patient Name" for Dicom Tag (0x0010, 0x0010) 
   /// @return the Dicom Name
   const TagName &GetName() const { return Name; } 

protected:
   DictEntry(uint16_t group, uint16_t elem,
             VRKey const &vr       = GDCM_VRUNKNOWN,
             TagName const &vm     = GDCM_UNKNOWN,
             TagName const &name   = GDCM_UNKNOWN);

private:
   /// \brief Value Multiplicity (e.g. "1", "1-n", "6")
   TagName VM; 

   /// e.g. "Patient's Name"                    
   TagName Name;      
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
