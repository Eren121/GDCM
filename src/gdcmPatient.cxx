// gdcmPatient.cxx
//-----------------------------------------------------------------------------
#include "gdcmPatient.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmPatient
 * \brief   
 * @param   begin iterator of begin for the object
 * @param   end   iterator of end for the object
 */
gdcmPatient::gdcmPatient(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

/**
 * \ingroup gdcmPatient
 * \brief   Canonical destructor.
 */
gdcmPatient::~gdcmPatient() 
{
   for(ListStudy::iterator cc = studies.begin();cc != studies.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmPatient
 * \brief   Prints the Object
 * @return
 */ 
void gdcmPatient::Print(std::ostream &os)
{
   os<<"PATIENT"<<std::endl;
   gdcmObject::Print(os);

   for(ListStudy::iterator cc = studies.begin();cc!=studies.end();++cc)
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
