// gdcmValEntry.h
//-----------------------------------------------------------------------------
#ifndef GDCMValEntry_H
#define GDCMValEntry_H

#include <iostream>
#include <stdio.h>

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmBinEntry
 * \brief   The dicom header of a Dicom file contains a set of such entries
 *          (when successfuly parsed against a given Dicom dictionary)
 *          This one contains a 'non string' value
 */
class GDCM_EXPORT gdcmDocEntry  : public gdcmValEntry {

public:

protected:

private:

// Variables
  
   /// \brief Header Entry value, stores as a std::string (VR will be used,
   /// later, to decode)
   std::string  value;









//-----------------------------------------------------------------------------
#endif

