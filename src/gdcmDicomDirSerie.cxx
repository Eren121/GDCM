// gdcmDicomDirSerie.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDirSerie.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDicomDirSerie
 * \brief   
 * @param   begin iterator of begin for the object
 * @param   end   iterator of end for the object
 */
gdcmDicomDirSerie::gdcmDicomDirSerie(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

/**
 * \ingroup gdcmDicomDirSerie
 * \brief   Canonical destructor.
 */
gdcmDicomDirSerie::~gdcmDicomDirSerie() 
{
   for(ListDicomDirImage::iterator cc = images.begin();cc != images.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDicomDirSerie
 * \brief   Prints the Object
 * @return
 */ 
void gdcmDicomDirSerie::Print(std::ostream &os)
{
   os<<"SERIE"<<std::endl;
   gdcmObject::Print(os);

   for(ListDicomDirImage::iterator cc = images.begin();cc != images.end();++cc)
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
