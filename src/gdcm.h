// gdcm.h
//-----------------------------------------------------------------------------
#ifndef GDCM_H
#define GDCM_H

////////////////////////////////////////////////////////////////////////////
// Tag based hash tables.
// We shall use as keys the strings (as the C++ type) obtained by
// concatenating the group value and the element value (both of type
// unsigned 16 bit integers in Dicom) expressed in hexadecimal.
// Example: consider the tag given as (group, element) = (0x0010, 0x0010).
// Then the corresponding TagKey shall be the string 0010|0010 (where
// the | (pipe symbol) acts as a separator).  
// Refer to gdcmDictEntry::TranslateToKey for this conversion function.

#include "gdcmException.h"
#include "gdcmCommon.h"

#include "gdcmDictEntry.h"
#include "gdcmDict.h"
#include "gdcmDictSet.h"

#include "gdcmHeaderEntry.h"

#include "gdcmHeader.h"
#include "gdcmHeaderHelper.h"
#include "gdcmFile.h"

//-----------------------------------------------------------------------------
#endif // #ifndef GDCM_H
