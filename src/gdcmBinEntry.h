/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmBinEntry.h,v $
  Language:  C++
  Date:      $Date: 2005/01/11 15:15:37 $
  Version:   $Revision: 1.30 $
                                                                                
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

namespace gdcm 
{

//-----------------------------------------------------------------------------
/**
 * \brief   Any Dicom Document (File Header or DicomDir) contains 
 *           a set of DocEntry entries 
 *          (when successfuly parsed against a given Dicom dictionary)
 *          BinEntry is a specialisation of ValEntry (for non std::string
 *          representable values)
 */
 
class GDCM_EXPORT BinEntry  : public ValEntry
{
public:
   BinEntry( DictEntry *e );
   BinEntry( DocEntry *d ); 

   ~BinEntry();
   
   void Print( std::ostream &os = std::cout );
   void WriteContent( std::ofstream *fp, FileType ft);

   /// \brief Returns the area value of the current Dicom Header Entry
   ///  when it's not string-translatable (e.g : a LUT table)         
   uint8_t *GetBinArea()  { return BinArea; }
   void  SetBinArea( uint8_t *area, bool self = true );

   /// Sets the value (string) of the current Dicom Document Entry
   virtual void SetValue(std::string const &val) { SetValueOnly(val); };
private:
   /// \brief unsecure memory area to hold 'non string' values 
   ///       (ie : Lookup Tables, overlays, icons)   
   uint8_t *BinArea;
   bool SelfArea;
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif

