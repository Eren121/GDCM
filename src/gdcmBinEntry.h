/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmBinEntry.h,v $
  Language:  C++
  Date:      $Date: 2004/08/16 16:30:32 $
  Version:   $Revision: 1.13 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMBinEntry_H
#define GDCMBinEntry_H

#include <iostream>
#include <stdio.h>

#include "gdcmValEntry.h"

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmBinEntry
 * \brief   The dicom header of a Dicom file contains a set of such entries
 *          (when successfuly parsed against a given Dicom dictionary)
 *          This one contains a 'string value'.
 */
class GDCM_EXPORT gdcmBinEntry  : public gdcmValEntry
{
public:

   gdcmBinEntry(gdcmDictEntry* e);
   gdcmBinEntry(gdcmDocEntry* d); 

   ~gdcmBinEntry();
   
   void Print(std::ostream &os = std::cout);
   virtual void Write(FILE*, FileType);

   /// \brief Returns the area value of the current Dicom Header Entry
   ///  when it's not string-translatable (e.g : a LUT table)         
   void* GetVoidArea()  { return VoidArea; };

   /// \brief Sets the value (non string) of the current Dicom Header Entry
   void SetVoidArea(void * area)  { VoidArea = area;  };
         
protected:

private:

// Variables
   
   /// \brief unsecure memory area to hold 'non string' values 
   /// (ie : Lookup Tables, overlays, icons)   
  // void *VoidArea;

};

//-----------------------------------------------------------------------------
#endif

