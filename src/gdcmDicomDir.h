// gdcmDICOMDIR.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMDIR_H
#define GDCMDICOMDIR_H

#include "gdcmHeader.h"
#include "gdcmCommon.h"
#include "gdcmPatient.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmPatient *> ListPatient;

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmDicomDir
 * \brief    gdcmDicomDir defines an object representing a DICOMDIR in memory.
 *
 */
class GDCM_EXPORT gdcmDicomDir: public gdcmParser 
{
public:
   gdcmDicomDir(ListTag *l,           bool exception_on_error = false);
   gdcmDicomDir(std::string &FileName,bool exception_on_error = false);
   
   ~gdcmDicomDir(void);

   void SetPrintLevel(int level) { printLevel = level; };
   virtual void Print(std::ostream &os = std::cout);

   inline ListPatient &GetPatients() {return patients;};
   bool   Write(std::string fileName);


   typedef enum
   {
      GDCM_NONE,
      GDCM_PATIENT,
      GDCM_STUDY,
      GDCM_SERIE,
      GDCM_IMAGE,
   } gdcmDicomDirType;
   
private:
   void CreateDicomDir(void);
   void AddObjectToEnd(gdcmDicomDirType type,ListTag::iterator begin,ListTag::iterator end);
   void AddPatientToEnd(ListTag::iterator begin,ListTag::iterator end);
   void AddStudyToEnd(ListTag::iterator begin,ListTag::iterator end);
   void AddSerieToEnd(ListTag::iterator begin,ListTag::iterator end);
   void AddImageToEnd(ListTag::iterator begin,ListTag::iterator end);

   ListPatient patients;
};

//-----------------------------------------------------------------------------
#endif
