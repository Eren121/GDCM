// gdcmPatient.h
//-----------------------------------------------------------------------------
#ifndef GDCMPATIENT_H
#define GDCMPATIENT_H

#include "gdcmObject.h"
#include "gdcmStudy.h"

//-----------------------------------------------------------------------------

typedef std::list<gdcmStudy *> lStudy;

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmPatient : public gdcmObject {    //
public:

   gdcmPatient();
   ~gdcmPatient();

   inline lStudy &GetStudies() {return studies;};
       
   lStudy studies;
private:


};

//-----------------------------------------------------------------------------
#endif
