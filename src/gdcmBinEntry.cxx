/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmBinEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/23 13:02:35 $
  Version:   $Revision: 1.16 $
                                                                                
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
gdcmBinEntry::gdcmBinEntry(gdcmDictEntry* e) : gdcmValEntry(e)
{
   this->voidArea = NULL;
}

/**
 * \brief   Constructor from a given gdcmBinEntry
 * @param   e Pointer to existing Doc entry
 */
gdcmBinEntry::gdcmBinEntry(gdcmDocEntry* e) : gdcmValEntry(e->GetDictEntry())
{
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
 * \brief   canonical Printer
 */
 
void gdcmBinEntry::Print(std::ostream &os)
{
   gdcmDocEntry::Print(os);
   std::ostringstream s;
   if (voidArea != NULL)
   {
      s << " [gdcm::Binary data loaded with length is "
        << GetLength() << "]"
        << std::endl;
   }
   else
   {
      s << " [gdcm::Binary data NOT loaded]"
        << std::endl;
   }
}

/*
 * \brief   canonical Writer
 */
void gdcmBinEntry::Write(FILE *fp, FileType filetype) {
   gdcmDocEntry::Write(fp, filetype);
   void *voidArea = GetVoidArea();
   int lgr=GetLength();
   if (voidArea != NULL) 
   { // there is a 'non string' LUT, overlay, etc
      fwrite ( voidArea,(size_t)lgr ,(size_t)1 ,fp); // Elem value
      return;            
   } 
}
//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
   
//-----------------------------------------------------------------------------
