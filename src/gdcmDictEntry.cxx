/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDictEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/27 14:12:24 $
  Version:   $Revision: 1.55 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDictEntry.h"
#include "gdcmDebug.h"
#include "gdcmUtil.h"

#include <iomanip> // for std::ios::left, ...
#include <fstream>
#include <stdio.h> // for sprintf

namespace gdcm 
{
//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor
 * @param   group      DICOM-Group Number
 * @param   elem       DICOM-Element Number
 * @param   vr         Value Representation
 * @param   vm         Value Multiplicity 
 * @param   name       description of the element
*/
DictEntry::DictEntry(uint16_t group, uint16_t elem,
                     VRKey const &vr, 
                     TagName const &vm,
                     TagName const &name):
   DicomEntry(group,elem,vr)
{
   VM      = vm;
   Name    = name;
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief Class allocator
 * @param   group      DICOM-Group Number
 * @param   elem       DICOM-Element Number
 * @param   vr         Value Representation
 * @param   vm         Value Multiplicity 
 * @param   name       description of the element
*/
DictEntry *DictEntry::New(uint16_t group, uint16_t elem,
                          VRKey const &vr,
                          TagName const &vm,
                          TagName const &name)
{
   return new DictEntry(group,elem,vr,vm,name);
}

/**
 * \brief       If-and only if-the V(alue) R(epresentation)
 * \            is unset then overwrite it.
 * @param vr    New V(alue) R(epresentation) to be set.
 */
void DictEntry::SetVR(VRKey const &vr) 
{
   if ( IsVRUnknown() )
   {
      DicomEntry::SetVR(vr);
   }
   else 
   {
      gdcmErrorMacro( "Overwriting VR might compromise a dictionary");
   }
}

/**
 * \brief       If-and only if-the V(alue) M(ultiplicity)
 * \            is unset then overwrite it.
 * @param vm    New V(alue) M(ultiplicity) to be set.
 */
void DictEntry::SetVM(TagName const &vm) 
{
   if ( IsVMUnknown() )
   {
      VM = vm;
   }
   else 
   {
      gdcmErrorMacro( "Overwriting VM might compromise a dictionary");
   }
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints an entry of the Dicom DictionaryEntry
 * @param   os ostream we want to print in
 * @param indent Indentation string to be prepended during printing
 */
void DictEntry::Print(std::ostream &os, std::string const &indent )
{
   DicomEntry::Print(os,indent);

   std::ostringstream s;

   if ( PrintLevel >= 1 )
   {
      s.setf(std::ios::left);
      s << std::setw(66-GetName().length()) << " ";
   }

   s << "[" << GetName()<< "]";
   os << s.str() << std::endl;
}

//-----------------------------------------------------------------------------
} // end namespace gdcm

