// gdcmStudy.h
//-----------------------------------------------------------------------------
#ifndef GDCMSTUDY_H
#define GDCMSTUDY_H

#include "gdcmObject.h"
#include "gdcmSerie.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmSerie *> ListSerie;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmStudy : public gdcmObject 
{
public:
   gdcmStudy(ListTag::iterator begin,ListTag::iterator end);
   ~gdcmStudy(void);

   virtual void Print(std::ostream &os = std::cout);

   inline ListSerie &GetSeries() {return series;};
   inline void AddSerie(gdcmSerie *obj) {series.push_back(obj);};

private:
   ListSerie series;
};

//-----------------------------------------------------------------------------
#endif
