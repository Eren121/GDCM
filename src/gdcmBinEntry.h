/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmBinEntry.h,v $
  Language:  C++
  Date:      $Date: 2004/10/07 21:05:39 $
  Version:   $Revision: 1.21 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMBINENTRY_H
#define GDCMBINENTRY_H

#include "gdcmValEntry.h"

#include <iostream>

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
   void Write(FILE*, FileType);

   /// \brief Returns the area value of the current Dicom Header Entry
   ///  when it's not string-translatable (e.g : a LUT table)         
   uint8_t* GetBinArea()  { return BinArea; }
   void  SetBinArea( uint8_t* area );
         
protected:

private:

// Variables
   
   /// \brief unsecure memory area to hold 'non string' values 
   ///       (ie : Lookup Tables, overlays, icons)   
   uint8_t* BinArea;

};

//-----------------------------------------------------------------------------
#endif

