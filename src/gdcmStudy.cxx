// gdcmStudy.cxx
//-----------------------------------------------------------------------------
#include "gdcmStudy.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmStudy
 * \brief   
 * @param   begin iterator of begin for the object
 * @param   end   iterator of end for the object
 */
gdcmStudy::gdcmStudy(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

/**
 * \ingroup gdcmStudy
 * \brief   Canonical destructor.
 */
gdcmStudy::~gdcmStudy() 
{
   for(ListSerie::iterator cc = series.begin();cc != series.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmStudy
 * \brief   Prints the Object
 * @return
 */ 
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
