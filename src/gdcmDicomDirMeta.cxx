/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirMeta.cxx,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:06 $
  Version:   $Revision: 1.11 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDicomDirMeta.h"
#include "gdcmDocument.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup gdcmMeta
 * \brief  Constructor
 */ 
gdcmDicomDirMeta::gdcmDicomDirMeta(TagDocEntryHT* ptagHT):
   gdcmDicomDirObject(ptagHT)
{

}

/**
 * \ingroup gdcmDicomDirMeta
 * \brief   Canonical destructor.
 */
gdcmDicomDirMeta::~gdcmDicomDirMeta() 
{
   
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints the Meta Elements
 */ 
void gdcmDicomDirMeta::Print(std::ostream& os)
{
   os << "META" << std::endl;
   // warning : META doesn't behave exactly like a gdcmObjet 
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
void gdcmDicomDirMeta::Write(FILE* fp, FileType t)
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
