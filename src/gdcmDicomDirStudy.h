// gdcmDicomDirStudy.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMDIRSTUDY_H
#define GDCMDICOMDIRSTUDY_H

#include "gdcmObject.h"
#include "gdcmDicomDirSerie.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmDicomDirSerie *> ListDicomDirSerie;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirStudy : public gdcmObject {
public:
   gdcmDicomDirStudy(ListTag::iterator begin,ListTag::iterator end);
   ~gdcmDicomDirStudy(void);

   virtual void Print(std::ostream &os = std::cout);

   inline ListDicomDirSerie &GetDicomDirSeries() {return series;};
   inline void AddDicomDirSerie(gdcmDicomDirSerie *obj) {series.push_back(obj);};

private:
   ListDicomDirSerie series;
};

//-----------------------------------------------------------------------------
#endif
