/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirMeta.cxx,v $
  Language:  C++
  Date:      $Date: 2004/08/01 02:39:09 $
  Version:   $Revision: 1.5 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
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
gdcmDicomDirMeta::gdcmDicomDirMeta(TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
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
 * \ingroup gdcmDicomDirMeta
 * \brief   Prints the Object
 * @return
 */ 
void gdcmDicomDirMeta::Print(std::ostream &os)
{
   os << "META" << std::endl;
   gdcmObject::Print(os);
}

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
