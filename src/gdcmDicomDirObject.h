/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirObject.h,v $
  Language:  C++
  Date:      $Date: 2004/12/03 20:16:57 $
  Version:   $Revision: 1.8 $
                                                                                
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
/**
 * \ingroup DicomDirObject
 * \brief   Base object
 */
class GDCM_EXPORT DicomDirObject : public SQItem
{
typedef std::list<DicomDirObject *> ListContent;
public:

   DicomDirObject(int depth = 1);
   ~DicomDirObject();

   /**
    * \brief   Sets the print level for the Dicom Header 
    * \note    0 for Light Print; 1 for 'medium' Print, 2 for Heavy
    */
   void SetPrintLevel(int level) { PrintLevel = level; };
   
   TagDocEntryHT GetEntry();
   void FillObject(ListDicomDirMetaElem const & elemList);

protected:

   // Constructor and destructor are protected to avoid end user to
   // instanciate from this class. 
   // NO ! DicomDir needs to instanciate it!

// Members :
   ///\brief detail level to be printed 
   int PrintLevel;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
