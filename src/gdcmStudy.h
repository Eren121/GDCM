// gdcmStudy.h
//-----------------------------------------------------------------------------
#ifndef GDCMSTUDY_H
#define GDCMSTUDY_H

#include "gdcmObject.h"
#include "gdcmSerie.h"

//-----------------------------------------------------------------------------

typedef std::list<gdcmSerie> lSerie;

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmStudy : public gdcmObject {
public:

   gdcmStudy();
   ~gdcmStudy();

   std::string GetEntryByNumber(guint16 group, guint16 element);
   std::string GetEntryByName(TagName name);

   inline lSerie GetSeries() {return series;};

private:

   lSerie series;

};

//-----------------------------------------------------------------------------
#endif
