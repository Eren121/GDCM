/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirObject.h,v $
  Language:  C++
  Date:      $Date: 2005/01/20 11:09:23 $
  Version:   $Revision: 1.13 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMDIROBJECT_H
#define GDCMDICOMDIROBJECT_H

#include "gdcmSQItem.h"
#include "gdcmDicomDirElement.h"

#include <string>
#include <list>

namespace gdcm 
{
//-----------------------------------------------------------------------------
class DicomDirObject;

//-----------------------------------------------------------------------------
typedef std::list<DicomDirObject *> ListContent;
//-----------------------------------------------------------------------------
/**
 * \ingroup DicomDirObject
 * \brief   Parent object for DicomDirPatient, DicomDirStudy, 
 *                          DicomDirSerie, DicomDirImage, of a DicomDir
 */
class GDCM_EXPORT DicomDirObject : public SQItem
{
public:
   void FillObject(ListDicomDirMetaElem const &elemList);

protected:
   // Constructor and destructor are protected to avoid end user to
   // instanciate from this class. 
   // NO ! DicomDir needs to instanciate it!
   DicomDirObject(int depth = 1);
   ~DicomDirObject();
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
