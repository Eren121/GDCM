/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmValEntry.h,v $
  Language:  C++
  Date:      $Date: 2005/01/21 11:40:56 $
  Version:   $Revision: 1.37 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMVALENTRY_H
#define GDCMVALENTRY_H

#include "gdcmDocEntry.h"

#include <iostream>

namespace gdcm 
{
//-----------------------------------------------------------------------------
/**
 * \brief   Any Dicom Document (File header or DicomDir) contains 
 *           a set of DocEntry  - Dicom entries -
 *          (when successfuly parsed against a given Dicom dictionary)
 *          ValEntry is an elementary DocEntry (as opposed to SeqEntry)
 */
class GDCM_EXPORT ValEntry  : public DocEntry
{
public:
   ValEntry(DictEntry *e);
   ValEntry(DocEntry *d); 
   ~ValEntry();

   /// \brief Returns the 'Value' (e.g. "Dupond Marcel") converted into a
   /// 'string', event if it's physically stored as an integer in the header of the
   /// current Document (File or DicomDir)
   std::string const &GetValue() const { return Value; };
    
   /// Sets the value (string) of the current Dicom entry.
   /// The size is updated
   virtual void SetValue(std::string const &val);

   virtual void Print(std::ostream &os = std::cout, std::string const & indent = ""); 

   virtual void WriteContent(std::ofstream *fp, FileType filetype);

protected:
   /// Sets the value (string) of the current Dicom entry
   void SetValueOnly(std::string const &val) { Value = val; }; 
   
private:
// Members :
  
   /// \brief Dicom entry value, internaly represented as a std::string.
   ///        The Value Representation (\ref VR) is independently used
   ///        in order to interpret (decode) this field.
   std::string Value;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif

