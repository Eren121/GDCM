// gdcmBinEntry.h
//-----------------------------------------------------------------------------
#ifndef GDCMBinEntry_H
#define GDCMBinEntry_H

#include <iostream>
#include <stdio.h>

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmBinEntry
 * \brief   The dicom header of a Dicom file contains a set of such entries
 *          (when successfuly parsed against a given Dicom dictionary)
 *          This one contains a 'string value'.
 */
class GDCM_EXPORT gdcmBinEntry  : public gdcmValEntry {

public:

protected:

private:

// Variables

   
   /// \brief unsecure memory area to hold 'non string' values 
   /// (ie : Lookup Tables, overlays, icons)   
   void *voidArea;








};

//-----------------------------------------------------------------------------
#endif

