/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirImage.cxx,v $
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

#include "gdcmDicomDirImage.h"
#include "gdcmValEntry.h"
//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup gdcmDicomDirImage
 * \brief  Constructor 
 * @param  s  SQ Item holding the elements
 * @param ptagHT pointer to the HTable (gdcmDicomDirObject needs it 
 *               to build the gdcmDocEntries)
 */
gdcmDicomDirImage::gdcmDicomDirImage(gdcmSQItem *s, TagDocEntryHT *ptagHT):
   gdcmDicomDirObject(ptagHT)
{
   docEntries = s->GetDocEntries();
}

/**
 * \ingroup gdcmDicomDirImage
 * \brief  Constructor 
 * @param ptagHT pointer to the HTable (gdcmDicomDirObject needs it 
 *               to build the gdcmDocEntries)
 */
gdcmDicomDirImage::gdcmDicomDirImage(TagDocEntryHT *ptagHT):
   gdcmDicomDirObject(ptagHT)
{
}
/**
 * \ingroup gdcmDicomDirImage
 * \brief   Canonical destructor.
 */
gdcmDicomDirImage::~gdcmDicomDirImage() 
{
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDicomDirImage
 * \brief   Prints the Object
 * @return
 */ 
void gdcmDicomDirImage::Print(std::ostream &os)
{
   os << "IMAGE : ";
   for(ListDocEntry::iterator i=docEntries.begin();i!=docEntries.end();++i)
   {
      if( (*i)->GetGroup() == 0x0004 && (*i)->GetElement() == 0x1500 )
      {
         os << ((gdcmValEntry *)(*i))->GetValue(); //FIXME
      }
   }
   os << std::endl;

   gdcmDicomDirObject::Print(os);
}

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
