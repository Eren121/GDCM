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
   gdcmDicomDirStudy(gdcmSQItem *s, TagDocEntryHT *ptagHT); 
   gdcmDicomDirStudy(TagDocEntryHT *ptagHT); 

   ~gdcmDicomDirStudy(void);

   virtual void Print(std::ostream &os = std::cout);
/**
 * \ingroup gdcmDicomDirStudy
 * \brief   returns the SERIE chained List for this STUDY.
 */
   inline ListDicomDirSerie &GetDicomDirSeries() 
      {return series;};
/**
 * \ingroup gdcmDicomDirStudy
 * \brief   adds the passed SERIE to the SERIE chained List for this STUDY.
 */ 
    inline void AddDicomDirSerie(gdcmDicomDirSerie *obj) 
       {series.push_back(obj);};

/**
 * \ingroup gdcmDicomDirStudy
 * \brief   TODO
 */ 
   gdcmDicomDirSerie* NewSerie(void);
    
private:
/**
* \brief chained list of DicomDirSeries (to be exploited recursively)
*/ 
   ListDicomDirSerie series;
};

//-----------------------------------------------------------------------------
#endif
