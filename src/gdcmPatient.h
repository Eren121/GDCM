// gdcmPatient.h
//-----------------------------------------------------------------------------
#ifndef GDCMPATIENT_H
#define GDCMPATIENT_H

#include "gdcmObject.h"
#include "gdcmStudy.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmStudy *> ListStudy;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmPatient : public gdcmObject 
{
public:
   gdcmPatient(ListTag::iterator begin,ListTag::iterator end);
   ~gdcmPatient(void);

   virtual void Print(std::ostream &os = std::cout);

   inline ListStudy &GetStudies() {return studies;};
   inline void AddStudy(gdcmStudy *obj) {studies.push_back(obj);};
       
private:
   ListStudy studies;
};

//-----------------------------------------------------------------------------
#endif
