// gdcmStudy.cxx
//-----------------------------------------------------------------------------
#include "gdcmStudy.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
gdcmStudy::gdcmStudy(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

gdcmStudy::~gdcmStudy() 
{
   for(ListSerie::iterator cc = series.begin();cc != series.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
void gdcmStudy::Print(std::ostream &os)
{
   os<<"STUDY"<<std::endl;
   gdcmObject::Print(os);

   for(ListSerie::iterator cc = series.begin();cc != series.end();++cc)
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
