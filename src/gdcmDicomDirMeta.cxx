// gdcmDicomDirMeta.h
//-----------------------------------------------------------------------------
#include "gdcmDicomDirMeta.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDicomDirMeta
 * \brief   
 * @param   begin iterator of begin for the object
 * @param   end   iterator of end   for the object
 */
gdcmDicomDirMeta::gdcmDicomDirMeta(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

/**
 * \ingroup gdcmDicomDirMeta
 * \brief   Canonical destructor.
 */
gdcmDicomDirMeta::~gdcmDicomDirMeta() 
{
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDicomDirMeta
 * \brief   Prints the Object
 * @return
 */ 
void gdcmDicomDirMeta::Print(std::ostream &os)
{
   os<<"META"<<std::endl;
   gdcmObject::Print(os);
}

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
