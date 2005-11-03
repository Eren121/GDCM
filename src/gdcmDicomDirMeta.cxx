/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirMeta.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/03 14:29:23 $
  Version:   $Revision: 1.30 $
                                                                                
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
#include "gdcmUtil.h"
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
   // 'Media Storage SOP Instance UID'   
   DataEntry *e00002_0013 = GetDataEntry(0x0002,0x0013);
   e00002_0013->SetString(Util::CreateUniqueUID());

   // 'Implementation Class UID'
   DataEntry *e00002_0012 = GetDataEntry(0x0002,0x0012);
   e00002_0012->SetString(Util::CreateUniqueUID());   
   
   // Entry : 0002|0000 = group length -> recalculated
   DataEntry *e0000 = GetDataEntry(0x0002,0x0000);
   std::ostringstream sLen;
   sLen << ComputeGroup0002Length( );
   e0000->SetString(sLen.str());
   
   for (ListDocEntry::iterator i = DocEntries.begin();  
                              i != DocEntries.end();
                              ++i)
   {
      (*i)->WriteContent(fp, filetype);
   }
}

/**
 * \brief Re-computes the length of the Dicom group 0002 (in the DicomDirMeta)
 */
int DicomDirMeta::ComputeGroup0002Length( ) 
{
   uint16_t gr;
   VRKey vr;
   
   int groupLength = 0;
   bool found0002 = false;   
  
   // for each Tag in the DicomDirMeta
   DocEntry *entry = GetFirstEntry();
   while( entry )
   {
      gr = entry->GetGroup();

      if ( gr == 0x0002 )
      {
         found0002 = true;

         if ( entry->GetElement() != 0x0000 )
         {
            vr = entry->GetVR();

            if ( vr == "OB" ) 
            {
               groupLength +=  4;
            }
            groupLength += 2 + 2 + 4 + entry->GetLength();   
         }
      }
      else if (found0002 )
         break;

      entry = GetNextEntry();
   }
   return groupLength; 
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
