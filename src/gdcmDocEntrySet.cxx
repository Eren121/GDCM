/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntrySet.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:47 $
  Version:   $Revision: 1.9 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include <errno.h>
#include "gdcmDebug.h"
#include "gdcmCommon.h"
#include "gdcmGlobal.h"
#include "gdcmDocEntrySet.h"
#include "gdcmException.h"
#include "gdcmDocEntry.h"
#include "gdcmSeqEntry.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDocEntrySet
 * \brief   Constructor from a given gdcmDocEntrySet
 */
gdcmDocEntrySet::gdcmDocEntrySet(int depthLevel) {
   SQDepthLevel = depthLevel + 1;
}
/**
 * \brief   Canonical destructor.
 */
gdcmDocEntrySet::~gdcmDocEntrySet(){
}
//-----------------------------------------------------------------------------
// Print
/*
 * \ingroup gdcmDocEntrySet
 * \brief   canonical Printer
 */


//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected


//-----------------------------------------------------------------------------
// Private


//-----------------------------------------------------------------------------
