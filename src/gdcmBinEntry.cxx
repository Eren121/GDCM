/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmBinEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2004/08/31 14:24:47 $
  Version:   $Revision: 1.23 $
                                                                                
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
   VoidArea = NULL;
}

/**
 * \brief   Constructor from a given gdcmBinEntry
 * @param   e Pointer to existing Doc entry
 */
gdcmBinEntry::gdcmBinEntry(gdcmDocEntry* e) : gdcmValEntry(e->GetDictEntry())
{
   UsableLength = e->GetLength();
   ReadLength   = e->GetReadLength();
   ImplicitVR   = e->IsImplicitVR();
   Offset       = e->GetOffset();
   PrintLevel   = e->GetPrintLevel();
   SQDepthLevel = e->GetDepthLevel();

   VoidArea = NULL; // let's be carefull !
}

/**
 * \brief   Canonical destructor.
 */
gdcmBinEntry::~gdcmBinEntry()
{
   if (VoidArea)
   {
      free (VoidArea);
      VoidArea = NULL; // let's be carefull !
   }
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
   void *voidArea = GetVoidArea();
   if (voidArea)
   {
      s << " [gdcm::Binary data loaded with length is "
        << GetLength() << "]";
   }
   else
   {
      if ( GetLength() == 0 )
      {
         s << " []";
      }
      else 
      {
         s << " [gdcm::Binary data NOT loaded]";
      }
         
   }
   os << s.str();
}

/*
 * \brief   canonical Writer
 * @param fp already open file pointer
 * @param filetype type of the file to be written
*/
void gdcmBinEntry::Write(FILE *fp, FileType filetype)
{
   gdcmDocEntry::Write(fp, filetype);
   void *voidArea = GetVoidArea();
   int lgr = GetLength();
   if (voidArea)
   {
      // there is a 'non string' LUT, overlay, etc
      fwrite ( voidArea,(size_t)lgr ,(size_t)1 ,fp); // Elem value
   }
}
//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
   
//-----------------------------------------------------------------------------
