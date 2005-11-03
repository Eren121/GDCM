/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirMeta.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/03 11:08:18 $
  Version:   $Revision: 1.29 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDicomDirMeta.h"
#include "gdcmDocument.h"
#include "gdcmDocEntry.h"
#include "gdcmGlobal.h"
#include "gdcmDataEntry.h"
namespace gdcm 
{
//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief  Constructor
 */ 
DicomDirMeta::DicomDirMeta(bool empty):
   DicomDirObject()
{
   if ( !empty )
   {
      uint8_t fmiv[2] = {0x02,0x00};
      ListDicomDirStudyElem const &elemList = 
         Global::GetDicomDirElements()->GetDicomDirMetaElements();
      FillObject(elemList);

      SetEntryBinArea(fmiv, 0x0002,0x0001, 2); 
             
   }
}

/**
 * \brief   Canonical destructor.
 */
DicomDirMeta::~DicomDirMeta() 
{
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Writes the Meta Elements
 * @param fp ofstream to write to
 * @param filetype type of the file (ACR, ImplicitVR, ExplicitVR, ...)
 * @return
 */ 
void DicomDirMeta::WriteContent(std::ofstream *fp, FileType filetype)
{   
   for (ListDocEntry::iterator i = DocEntries.begin();  
                              i != DocEntries.end();
                              ++i)
   {
      (*i)->WriteContent(fp, filetype);
   }
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints the Meta Elements
 * @param os ostream to write to 
 * @param indent Indentation string to be prepended during printing
 */ 
void DicomDirMeta::Print(std::ostream &os, std::string const & )
{
   os << "META" << std::endl;
   // warning : META doesn't behave exactly like a Objet 
   for (ListDocEntry::iterator i = DocEntries.begin();
        i != DocEntries.end();
        ++i)
   {
      (*i)->SetPrintLevel(PrintLevel);
      (*i)->Print();
      os << std::endl;
   }
}

//-----------------------------------------------------------------------------
} // end namespace gdcm
