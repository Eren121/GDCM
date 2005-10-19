/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomEntry.h,v $
  Language:  C++
  Date:      $Date: 2005/10/19 13:17:05 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMENTRY_H
#define GDCMDICOMENTRY_H

#include "gdcmCommon.h"
#include "gdcmBase.h"
#include "gdcmVRKey.h"
#include "gdcmTagKey.h"

namespace gdcm 
{
//-----------------------------------------------------------------------------
/**
 * \brief
 * the DicomEntry in an element contained by the Dict.
 * It contains :
 *  - the key referenced by the DICOM norm or the constructor (for private keys)
 *    i.e. the Group number
 *         the Element number
 *  - the VR (Value Representation)
 *  - the VM (Value Multplicity)
 *  - the corresponding name in english
 */
class GDCM_EXPORT DicomEntry : public Base
{
public:
   DicomEntry(const uint16_t &group,const uint16_t &elt,
              const VRKey &vr = GDCM_VRUNKNOWN);
   ~DicomEntry();

// Print
   void Print(std::ostream &os = std::cout, std::string const &indent = "");

   /// \brief  Returns the Dicom Group Number
   /// @return the Dicom Group Number
   const uint16_t &GetGroup() const { return Tag[0]; }

   /// \brief  Returns the Dicom Element Number
   /// @return the Dicom Element Number
   const uint16_t &GetElement() const { return Tag[1]; }

   /// \brief  Set the Dicom Value Representation 
   /// \param vr the Dicom Value Representation
   virtual void SetVR(VRKey const &vr) { VR = vr; }
   /// \brief  Returns the Dicom Value Representation 
   /// @return the Dicom Value Representation
   const VRKey &GetVR() const { return VR; }
   /// \brief tells if the V(alue) R(epresentation) is known (?!)
   /// @return 
   bool IsVRUnknown() const { return VR == GDCM_VRUNKNOWN; }

   const TagKey &GetKey() const { return Tag; }

// Key creation
   static TagKey TranslateToKey(uint16_t group, uint16_t elem);

private:
   /// Dicom \ref TagKey. Contains DicomGroup number and DicomElement number
   TagKey Tag;

   /// \brief Value Representation i.e. some clue about the nature
   ///        of the data represented e.g. 
   ///        "FD" short for "Floating Point Double"(see \ref VR)
   ///        "PN" short for "Person Name"       
   VRKey VR;
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
