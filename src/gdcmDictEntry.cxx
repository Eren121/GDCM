/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDictEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/07 22:06:47 $
  Version:   $Revision: 1.37 $
                                                                                
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

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor
 * @param   group      DICOM-Group Number
 * @param   element    DICOM-Element Number
 * @param   vr         Value Representation
 * @param   vm         Value Mutlplicity 
 * @param   name      description of the element
*/

DictEntry::DictEntry(uint16_t group, uint16_t element,
                     TagName const &vr, 
                     TagName const &vm,
                     TagName const &name)
{
   Group   = group;
   Element = element;
   VR      = vr;
   VM      = vm;
   Name    = name;
   Key     = TranslateToKey(group, element);
}

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   concatenates 2 uint16_t (supposed to be a Dicom group number 
 *                                              and a Dicom element number)
 * @param  group the Dicom group number used to build the tag
 * @param  element the Dicom element number used to build the tag
 * @return the built tag
 */
TagKey DictEntry::TranslateToKey(uint16_t group, uint16_t element)
{
   return Util::Format("%04x|%04x", group, element);
}

//-----------------------------------------------------------------------------
/**
 * \brief       If-and only if-the V(alue) R(epresentation)
 * \            is unset then overwrite it.
 * @param vr    New V(alue) R(epresentation) to be set.
 */
void DictEntry::SetVR(TagName const &vr) 
{
   if ( IsVRUnknown() )
   {
      VR = vr;
   }
   else 
   {
      gdcmErrorMacro( "Overwriting VR might compromise a dictionary");
   }
}

//-----------------------------------------------------------------------------
/**
 * \brief       If-and only if-the V(alue) M(ultiplicity)
 * \            is unset then overwrite it.
 * @param vr    New V(alue) M(ultiplicity) to be set.
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
} // end namespace gdcm

