// gdcmPatient.h
//-----------------------------------------------------------------------------
#ifndef GDCMPATIENT_H
#define GDCMPATIENT_H

#include "gdcmObject.h"
#include "gdcmStudy.h"

//-----------------------------------------------------------------------------

typedef std::list<gdcmStudy> lStudy;

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmPatient : public gdcmObject {    //
public:

   gdcmPatient();
   ~gdcmPatient();

   std::string GetEntryByNumber(guint16 group, guint16 element);
   std::string GetEntryByName(TagName name);

   inline lStudy GetStudies() {return studies;};

private:

   lStudy studies;

};

//-----------------------------------------------------------------------------
#endif
