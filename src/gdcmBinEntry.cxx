/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmBinEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/19 23:51:03 $
  Version:   $Revision: 1.10 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmBinEntry.h"
#include "gdcmDebug.h"


//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \brief   Constructor from a given gdcmBinEntry
 */
gdcmBinEntry::gdcmBinEntry(gdcmDictEntry* e) : gdcmValEntry(e) {
   this->voidArea = NULL;
}

/**
 * \brief   Constructor from a given gdcmBinEntry
 * @param   e Pointer to existing Doc entry
 */
gdcmBinEntry::gdcmBinEntry(gdcmDocEntry* e) : gdcmValEntry(e->GetDictEntry()){
   this->UsableLength = e->GetLength();
	this->ReadLength   = e->GetReadLength();	
	this->ImplicitVR   = e->IsImplicitVR();
	this->Offset       = e->GetOffset();	
	this->printLevel   = e->GetPrintLevel();	
	this->SQDepthLevel = e->GetDepthLevel();	
	
   this->voidArea = NULL; // let's be carefull !
}

/**
 * \brief   Canonical destructor.
 */
gdcmBinEntry::~gdcmBinEntry(){
   if (voidArea)
      free (voidArea);
}


//-----------------------------------------------------------------------------
// Print
/*
 * \ingroup gdcmDocEntry
 * \brief   canonical Printer
 */
 
void gdcmBinEntry::Print(std::ostream &os) {
   PrintCommonPart(os);
   /// \todo Write a true specialisation of Print i.e. display something
   ///       for BinEntry extension.
   dbg.Verbose(1, "gdcmBinEntry::Print: so WHAT ?");
}
//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
   
//-----------------------------------------------------------------------------
