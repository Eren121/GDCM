// gdcmDocEntrySet.cxx
//-----------------------------------------------------------------------------
//
#include <errno.h>
#include "gdcmDebug.h"
#include "gdcmCommon.h"
#include "gdcmGlobal.h"
#include "gdcmDocEntrySet.h"
#include "gdcmException.h"
#include "gdcmDocEntry.h"
#include "gdcmSeqEntry.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDocEntrySet
 * \brief   Constructor from a given gdcmDocEntrySet
 */
gdcmDocEntrySet::gdcmDocEntrySet(int depthLevel) {
   SQDepthLevel = depthLevel + 1;
}
/**
 * \brief   Canonical destructor.
 */
gdcmDocEntrySet::~gdcmDocEntrySet(){
}
//-----------------------------------------------------------------------------
// Print
/*
 * \ingroup gdcmDocEntrySet
 * \brief   canonical Printer
 */


//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected


//-----------------------------------------------------------------------------
// Private


//-----------------------------------------------------------------------------
