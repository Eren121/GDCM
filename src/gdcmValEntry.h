/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmValEntry.h,v $
  Language:  C++
  Date:      $Date: 2004/06/23 03:36:24 $
  Version:   $Revision: 1.15 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMValEntry_H
#define GDCMValEntry_H

#include <iostream>
#include <stdio.h>

#include "gdcmDocEntry.h"

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmValEntry
 * \brief   The dicom header of a Dicom file contains a set of such entries
 *          (when successfuly parsed against a given Dicom dictionary)
 */
class GDCM_EXPORT gdcmValEntry  : public gdcmDocEntry {

public:

   gdcmValEntry(gdcmDictEntry* e);

   gdcmValEntry(gdcmDocEntry* d); 
   virtual ~gdcmValEntry();

   /// \brief Returns the 'Value' (e.g. "Dupond Marcel") converted into a
   /// 'string', if it's stored as an integer in the header of the
   /// current Dicom Document Entry
   inline std::string  GetValue()     { return value; };
    
   /// Sets the value (string) of the current Dicom Document Entry
   inline void SetValue(std::string val)  { value = val;  };
   
   virtual void Print(std::ostream &os = std::cout); 
   virtual void gdcmValEntry::Write(FILE *fp);           
protected:

   /// \brief for 'non string' values. Will be move to gdcmBinEntry, later
   void* voidArea;  // clean it out later
   
private:

// Members :
  
   /// \brief Document Entry value, internaly represented as a std::string
   ///        The Value Representation (\ref gdcmVR) is indenpendently used
   ///        in order to interpret (decode) this field.
   std::string  value;

};

//-----------------------------------------------------------------------------
#endif

