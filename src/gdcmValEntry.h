/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmValEntry.h,v $
  Language:  C++
  Date:      $Date: 2004/09/13 12:10:53 $
  Version:   $Revision: 1.22 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMVALENTRY_H
#define GDCMVALENTRY_H

#include <iostream>
#include <stdio.h>

#include "gdcmDocEntry.h"

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmValEntry
 * \brief   The dicom header of a Dicom file contains a set of such entries
 *          (when successfuly parsed against a given Dicom dictionary)
 */
class GDCM_EXPORT gdcmValEntry  : public gdcmDocEntry
{
public:
   gdcmValEntry(gdcmDictEntry* e);
   gdcmValEntry(gdcmDocEntry* d); 
   virtual ~gdcmValEntry();

   /// \brief Returns the 'Value' (e.g. "Dupond Marcel") converted into a
   /// 'string', if it's stored as an integer in the header of the
   /// current Dicom Document Entry
   std::string GetValue() { return Value; };
    
   /// Sets the value (string) of the current Dicom Document Entry
   void SetValue(std::string const & val) { Value = val; };

   virtual void Print(std::ostream &os = std::cout); 
   virtual void Write(FILE *fp, FileType filetype);

protected:
   
private:
// Members :
  
   /// \brief Document Entry value, internaly represented as a std::string
   ///        The Value Representation (\ref gdcmVR) is independently used
   ///        in order to interpret (decode) this field.
   std::string  Value;
};

//-----------------------------------------------------------------------------
#endif

