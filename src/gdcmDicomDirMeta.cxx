/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirMeta.cxx,v $
  Language:  C++
  Date:      $Date: 2004/12/06 11:37:38 $
  Version:   $Revision: 1.18 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDicomDirMeta.h"
#include "gdcmDocument.h"
#include "gdcmDocEntry.h"

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup DicomDirMeta
 * \brief  Constructor
 */ 
DicomDirMeta::DicomDirMeta():
   DicomDirObject()
{
}

/**
 * \ingroup DicomDirMeta
 * \brief   Canonical destructor.
 */
DicomDirMeta::~DicomDirMeta() 
{
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints the Meta Elements
 */ 
void DicomDirMeta::Print(std::ostream& os)
{
   os << "META" << std::endl;
   // warning : META doesn't behave exactly like a Objet 
   for (ListDocEntry::iterator i = DocEntries.begin();
        i != DocEntries.end();
        ++i)
   {
      (*i)->Print();
      os << std::endl;
   }
}


//-----------------------------------------------------------------------------
// Public


/**
 * \brief   Writes the Meta Elements
 * @return
 */ 
void DicomDirMeta::WriteContent(std::ofstream* fp, FileType t)
{   
   for (ListDocEntry::iterator i = DocEntries.begin();  
                              i != DocEntries.end();
                              ++i)
   {
      (*i)->WriteContent(fp, t);
   }
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
