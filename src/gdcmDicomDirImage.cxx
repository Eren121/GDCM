/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirImage.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:47 $
  Version:   $Revision: 1.7 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
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
 * @param  s  SQ Item holdoing the elements
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmDocEntries)
 */
gdcmDicomDirImage::gdcmDicomDirImage(gdcmSQItem *s, TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
{
   docEntries = s->GetDocEntries();
}

gdcmDicomDirImage::gdcmDicomDirImage(TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
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
   os<<"IMAGE : ";
   for(ListDocEntry::iterator i=docEntries.begin();i!=docEntries.end();++i)
   {
      if( ((*i)->GetGroup()==0x0004) && ((*i)->GetElement()==0x1500) )
         os<<((gdcmValEntry *)(*i))->GetValue();
   }
   os<<std::endl;

   gdcmObject::Print(os);
}

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
