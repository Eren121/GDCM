/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirImage.cxx,v $
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

#include "gdcmDicomDirImage.h"
#include "gdcmValEntry.h"

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup DicomDirImage
 * \brief  Constructor 
 * @param  s  SQ Item holding the elements
 * @param ptagHT pointer to the HTable (DicomDirObject needs it 
 *               to build the DocEntries)
 */
DicomDirImage::DicomDirImage(SQItem *s, TagDocEntryHT *ptagHT):
   DicomDirObject(ptagHT)
{
   docEntries = s->GetDocEntries();
}

/**
 * \ingroup DicomDirImage
 * \brief  Constructor 
 * @param ptagHT pointer to the HTable (DicomDirObject needs it 
 *               to build the DocEntries)
 */
DicomDirImage::DicomDirImage(TagDocEntryHT *ptagHT):
   DicomDirObject(ptagHT)
{
}
/**
 * \ingroup DicomDirImage
 * \brief   Canonical destructor.
 */
DicomDirImage::~DicomDirImage() 
{
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup DicomDirImage
 * \brief   Prints the Object
 * @return
 */ 
void DicomDirImage::Print(std::ostream &os)
{
   os << "IMAGE : ";
   for(ListDocEntry::iterator i=docEntries.begin();i!=docEntries.end();++i)
   {
      if( (*i)->GetGroup() == 0x0004 && (*i)->GetElement() == 0x1500 )
      {
         os << ((ValEntry *)(*i))->GetValue(); //FIXME
      }
   }
   os << std::endl;

   DicomDirObject::Print(os);
}

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
} // end namespace gdcm

