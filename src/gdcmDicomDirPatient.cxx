// gdcmDicomDirPatient.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDirPatient.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDicomDirPatient
 * \brief   
 * @param   begin iterator of begin for the object
 * @param   end   iterator of end for the object
 */
gdcmDicomDirPatient::gdcmDicomDirPatient(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

/**
 * \ingroup gdcmDicomDirPatient
 * \brief   Canonical destructor.
 */
gdcmDicomDirPatient::~gdcmDicomDirPatient() 
{
   for(ListDicomDirStudy::iterator cc = studies.begin();cc != studies.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDicomDirPatient
 * \brief   Prints the Object
 * @return
 */ 
void gdcmDicomDirPatient::Print(std::ostream &os)
{
   os<<"PATIENT"<<std::endl;
   gdcmObject::Print(os);

   for(ListDicomDirStudy::iterator cc = studies.begin();cc!=studies.end();++cc)
   {
      (*cc)->SetPrintLevel(printLevel);
      (*cc)->Print(os);
   }
}

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
