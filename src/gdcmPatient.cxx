// gdcmPatient.cxx
//-----------------------------------------------------------------------------
#include "gdcmPatient.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
gdcmPatient::gdcmPatient(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

gdcmPatient::~gdcmPatient() 
{
   for(ListStudy::iterator cc = studies.begin();cc != studies.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
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
