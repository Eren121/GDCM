// gdcmDicomDirStudy.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDirStudy.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDicomDirStudy
 * \brief   
 * @param   begin iterator of begin for the object
 * @param   end   iterator of end for the object
 */
gdcmDicomDirStudy::gdcmDicomDirStudy(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

/**
 * \ingroup gdcmDicomDirStudy
 * \brief   Canonical destructor.
 */
gdcmDicomDirStudy::~gdcmDicomDirStudy() 
{
   for(ListDicomDirSerie::iterator cc = series.begin();cc != series.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDicomDirStudy
 * \brief   Prints the Object
 * @return
 */ 
void gdcmDicomDirStudy::Print(std::ostream &os)
{
   os<<"STUDY"<<std::endl;
   gdcmObject::Print(os);

   for(ListDicomDirSerie::iterator cc = series.begin();cc != series.end();++cc)
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
