/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirMeta.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/12 04:35:44 $
  Version:   $Revision: 1.12 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDicomDirMeta.h"
#include "gdcmDocument.h"

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup DicomDirMeta
 * \brief  Constructor
 */ 
DicomDirMeta::DicomDirMeta(TagDocEntryHT* ptagHT):
   DicomDirObject(ptagHT)
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
   for (ListDocEntry::iterator i = docEntries.begin();  
        i != docEntries.end();
        ++i)
      (*i)->Print();    
}


//-----------------------------------------------------------------------------
// Public


/**
 * \brief   Writes the Meta Elements
 * @return
 */ 
void DicomDirMeta::Write(FILE* fp, FileType t)
{   
   for (ListDocEntry::iterator i = docEntries.begin();  
        i != docEntries.end();
        ++i)
      (*i)->Write(fp, t);   
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
