/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDictEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:06 $
  Version:   $Revision: 1.23 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDictEntry.h"
#include "gdcmDebug.h"

#include <stdio.h>    // FIXME For sprintf

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor
 * @param   InGroup    DICOM-Group Number
 * @param   InElement  DICOM-Element Number
 * @param   InVr       Value Representatiion
 * @param   InFourth  // DO NOT use any longer; 
 *                       NOT part of the Dicom Standard
 * @param   InName    description of the element
*/

gdcmDictEntry::gdcmDictEntry(uint16_t InGroup, uint16_t InElement,
                             std::string  InVr, std::string InFourth,
                             std::string  InName)
{
   group   = InGroup;
   element = InElement;
   vr      = InVr;
   fourth  = InFourth;
   name    = InName;
   key     = TranslateToKey(group, element); /// \todo Frog MEMORY LEAK.
}

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   concatenates 2 uint16_t (supposed to be a Dicom group number 
 *                                             and a Dicom element number)
 * @param  group the Dicom group   number used to build the tag
 * @param  element the Dicom element number used to build the tag
 * @return the built tag
 */
gdcmTagKey gdcmDictEntry::TranslateToKey(uint16_t group, uint16_t element)
{
   char trash[10];
   gdcmTagKey key;
   // CLEANME: better call the iostream<< with the hex manipulator on.
   // This requires some reading of the stdlibC++ sources to make the
   // proper call (or copy).
   sprintf(trash, "%04x|%04x", group , element);
   key = trash;  // Convertion through assignement
   return key;
}

/**
 * \brief       If-and only if-the V(alue) R(epresentation)
 * \            is unset then overwrite it.
 * @param NewVr New V(alue) R(epresentation) to be set.
 */
void gdcmDictEntry::SetVR(std::string NewVr) 
{
   if ( IsVRUnknown() )
      vr = NewVr;
   else 
   {
      dbg.Error(true, "gdcmDictEntry::SetVR",
                       "Overwriting vr might compromise a dictionary");
   }
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

