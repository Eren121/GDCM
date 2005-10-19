/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomEntry.cxx,v $
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

#include "gdcmDicomEntry.h"
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
DicomEntry::DicomEntry(const uint16_t &group,const uint16_t &elt,
                       const VRKey &vr)
{
   Tag.SetGroup(group);
   Tag.SetElement(elt);
   VR = vr;
}

/**
 * \brief   Destructor
 */
DicomEntry::~DicomEntry()
{
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   concatenates 2 uint16_t (supposed to be a Dicom group number 
 *                                              and a Dicom element number)
 * @param  group the Dicom group number used to build the tag
 * @param  elem the Dicom element number used to build the tag
 * @return the built tag
 */
TagKey DicomEntry::TranslateToKey(uint16_t group, uint16_t elem)
{
   // according to 'Purify', TranslateToKey is one of the most
   // time consuming methods.
   // Let's try to shorten it !
   return TagKey(group,elem);
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
void DicomEntry::Print(std::ostream &os, std::string const & )
{
   std::ostringstream s;

   s << GetKey(); 
   s << " [" << VR  << "] ";
}

//-----------------------------------------------------------------------------
} // end namespace gdcm

