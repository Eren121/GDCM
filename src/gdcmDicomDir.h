// gdcmDICOMDIR.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMDIR_H
#define GDCMDICOMDIR_H

#include "gdcmHeader.h"
#include "gdcmCommon.h"
#include "gdcmPatient.h"

//-----------------------------------------------------------------------------

typedef std::list<gdcmPatient *> lPatient;

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmDicomDir
 * \brief    gdcmDicomDir defines an object representing a DICOMDIR in memory.
 *
 */
class GDCM_EXPORT gdcmDicomDir: public gdcmParser {
public:

   gdcmDicomDir(std::string &FileName,bool exception_on_error = false );
   ~gdcmDicomDir();

   inline lPatient &GetPatients() {return patients;};
   inline void AddPatient(gdcmPatient *patient) {patients.push_back(patient);};
   
private:
   lPatient patients;

};

//-----------------------------------------------------------------------------
#endif
