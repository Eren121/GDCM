/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmObject.h,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:48 $
  Version:   $Revision: 1.17 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMOBJECT_H
#define GDCMOBJECT_H

#include <string>
#include <list>
#include "gdcmCommon.h"
#include "gdcmDocEntry.h"
#include "gdcmDocument.h"
#include "gdcmDicomDirElement.h"
#include "gdcmSQItem.h"

//-----------------------------------------------------------------------------
class gdcmObject;
typedef std::list<gdcmObject *> ListContent;

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmObject
 * \brief   Base object
 */
class GDCM_EXPORT gdcmObject : public gdcmSQItem
{
public:

   /**
    * \brief   Sets the print level for the Dicom Header 
    * \note    0 for Light Print; 1 for 'medium' Print, 2 for Heavy
    */
   void SetPrintLevel(int level) 
      { printLevel = level; };
   
   TagDocEntryHT GetEntry(void);
   void FillObject(std::list<gdcmElement> elemList);

protected:

   // Constructor and destructor are protected to avoid end user to
   // instanciate from this class.
   gdcmObject(TagDocEntryHT *ptagHT, int depth = 0);
   virtual ~gdcmObject(void);

// Members :
   ///\brief detail level to be printed 
   int printLevel;

private:
};

//-----------------------------------------------------------------------------
#endif
