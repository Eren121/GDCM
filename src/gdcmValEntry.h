// gdcmValEntry.h
//-----------------------------------------------------------------------------
#ifndef GDCMValEntry_H
#define GDCMValEntry_H

#include <iostream>
#include <stdio.h>

#include "gdcmDocEntry.h"
//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmBinEntry
 * \brief   The dicom header of a Dicom file contains a set of such entries
 *          (when successfuly parsed against a given Dicom dictionary)
 *          This one contains a 'non string' value
 */
class GDCM_EXPORT gdcmValEntry  : public gdcmDocEntry {

public:

   gdcmValEntry(gdcmDictEntry* e); 
   ~gdcmValEntry(void);

   /// \brief Returns the 'Value' (e.g. "Dupond Marcel") converted into a
   /// 'string', if it's stored as an integer in the Dicom Header of the
   /// current Dicom Header Entry
   inline std::string  GetValue(void)     { return value; };
    
   /// Sets the value (string) of the current Dicom Header Entry
   inline void SetValue(std::string val)  { value = val;  };
   
   virtual void Print(std::ostream &os = std::cout); 
           
protected:

   char* voidArea;  // clean it out later
   
private:

// Variables
  
   /// \brief Header Entry value, stored as a std::string (VR will be used,
   /// later, to decode)
   
   std::string  value;
   



};

//-----------------------------------------------------------------------------
#endif

