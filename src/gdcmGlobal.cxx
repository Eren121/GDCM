/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmGlobal.cxx,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:07 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmGlobal.h"
#include "gdcmDebug.h"
#include <stdio.h>
#include <ctype.h>   // For isspace
#include <string.h>  // CLEANME: could this be only string ? Related to Win32 ?

/**
 * \ingroup Globals
 * \brief Pointer to a container, holding _all_ the Dicom Dictionaries.
 */
gdcmDictSet         *gdcmGlobal::Dicts  = (gdcmDictSet *)0;

/**
 * \ingroup Globals
 * \brief   Pointer to a hash table containing the 'Value Representations'.
 */
gdcmVR              *gdcmGlobal::VR     = (gdcmVR *)0;

/**
 * \ingroup Globals
 * \brief   Pointer to a hash table containing the Transfer Syntax codes
 *          and their english description 
 */
gdcmTS              *gdcmGlobal::TS     = (gdcmTS *)0;

/**
 * \ingroup Globals
 * \brief   Pointer to the hash table containing the Dicom Elements
 *          necessary to describe each part of a DICOMDIR 
 */
gdcmDicomDirElement *gdcmGlobal::ddElem = (gdcmDicomDirElement *)0;

/**
 * \ingroup Globals
 * \brief   Global container
 */
gdcmGlobal gdcmGlob;

/**
 * \ingroup gdcmGlobal
 * \brief   constructor : populates the various H Tables
 */
gdcmGlobal::gdcmGlobal(void) {
   if (VR || TS || Dicts || ddElem)
      dbg.Verbose(0, "gdcmGlobal::gdcmGlobal : VR or TS or Dicts already allocated");
   Dicts  = new gdcmDictSet();
   VR     = new gdcmVR();
   TS     = new gdcmTS();
   ddElem = new gdcmDicomDirElement();
}

/**
 * \ingroup gdcmGlobal
 * \brief   canonical destructor 
 */
gdcmGlobal::~gdcmGlobal() {
   delete Dicts;
   delete VR;
   delete TS;
   delete ddElem;
}
/**
 * \ingroup gdcmGlobal
 * \brief   returns a pointer to the 'Value Representation Table' 
 */
gdcmVR *gdcmGlobal::GetVR(void) {
   return VR;
}
/**
 * \ingroup gdcmGlobal
 * \brief   returns a pointer to the 'Transfert Syntax Table' 
 */
gdcmTS *gdcmGlobal::GetTS(void) {
   return TS;
}
/**
 * \ingroup gdcmGlobal
 * \brief   returns a pointer to Dictionaries Table 
 */
gdcmDictSet *gdcmGlobal::GetDicts(void) {
   return Dicts;
}
/**
 * \ingroup gdcmGlobal
 * \brief   returns a pointer to the DicomDir related elements Table 
 */
gdcmDicomDirElement *gdcmGlobal::GetDicomDirElements(void) {
   return ddElem;
}
