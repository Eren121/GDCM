// gdcmDicomDirPatient.h
//-----------------------------------------------------------------------------
#ifndef GDCMPATIENT_H
#define GDCMPATIENT_H

#include "gdcmObject.h"
#include "gdcmDicomDirStudy.h"
#include "gdcmSQItem.h"
//-----------------------------------------------------------------------------
typedef std::list<gdcmDicomDirStudy *> ListDicomDirStudy;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirPatient : public gdcmObject 
{
public:
   gdcmDicomDirPatient(gdcmSQItem *s, TagDocEntryHT *ptagHT); 
   gdcmDicomDirPatient(TagDocEntryHT *ptagHT); 

   ~gdcmDicomDirPatient(void);

   virtual void Print(std::ostream &os = std::cout);
/**
 * \ingroup gdcmDicomDirPatient
 * \brief   returns the STUDY chained List for this PATIENT.
 */
   inline ListDicomDirStudy &GetDicomDirStudies() 
      {return studies;};
/**
 * \ingroup gdcmDicomDirPatient
 * \brief   adds the passed STUDY to the STUDY chained List for this PATIENT.
 */      
   inline void AddDicomDirStudy(gdcmDicomDirStudy *obj) 
      {studies.push_back(obj);};
/**
 * \ingroup gdcmDicomDirPatient
 * \brief   TODO
 */ 
   gdcmDicomDirStudy* NewStudy(void); 
         
private:

/**
* \brief chained list of DicomDirStudy
*/ 
   ListDicomDirStudy studies;
};

//-----------------------------------------------------------------------------
#endif
