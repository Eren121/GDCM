// gdcmDocEntrySet.h

#ifndef GDCMDOCENTRYSET_H
#define GDCMDOCENTRYSET_H

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmDocEntrySet  {

public:


   gdcmDocEntrySet(void); 
   ~gdcmDocEntrySet(void);

   void gdcmElementSet::FindDocEntryLength (gdcmDocEntry *Entry);

protected:

private:

   bool gdcmDocEntrySet::LoadDocEntriesSet(bool exception_on_error = false) 
                     throw(gdcmFormatError);
};


//-----------------------------------------------------------------------------
#endif

