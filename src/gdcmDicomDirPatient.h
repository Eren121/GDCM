// gdcmDicomDirPatient.h
//-----------------------------------------------------------------------------
#ifndef GDCMPATIENT_H
#define GDCMPATIENT_H

#include "gdcmObject.h"
#include "gdcmDicomDirStudy.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmDicomDirStudy *> ListDicomDirStudy;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirPatient : public gdcmObject 
{
public:
   gdcmDicomDirPatient(ListTag::iterator begin,ListTag::iterator end);
   ~gdcmDicomDirPatient(void);

   virtual void Print(std::ostream &os = std::cout);

   inline ListDicomDirStudy &GetDicomDirStudies() {return studies;};
   inline void AddDicomDirStudy(gdcmDicomDirStudy *obj) {studies.push_back(obj);};
       
private:
   ListDicomDirStudy studies;
};

//-----------------------------------------------------------------------------
#endif
