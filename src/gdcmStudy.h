// gdcmStudy.h
//-----------------------------------------------------------------------------
#ifndef GDCMSTUDY_H
#define GDCMSTUDY_H

#include "gdcmObject.h"
#include "gdcmSerie.h"

//-----------------------------------------------------------------------------

typedef std::list<gdcmSerie *> lSerie;

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmStudy : public gdcmObject {
public:

   gdcmStudy();
   ~gdcmStudy();

   inline lSerie &GetSeries() {return series;};
   
   lSerie series;
   
private:


};

//-----------------------------------------------------------------------------
#endif
