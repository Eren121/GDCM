// gdcmDocEntrySet.h
//-----------------------------------------------------------------------------
#ifndef GDCMDOCENTRYSET_H
#define GDCMDOCENTRYSET_H

#include "gdcmException.h"
#include "gdcmDocEntry.h"

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmDocEntrySet
{

public:

   gdcmDocEntrySet(void); 
   ~gdcmDocEntrySet(void);

   void FindDocEntryLength (gdcmDocEntry *Entry);

protected:

private:

   bool LoadDocEntrySet(bool exception_on_error = false) 
                     throw(gdcmFormatError);
};


//-----------------------------------------------------------------------------
#endif

