// gdcmMeta.h
//-----------------------------------------------------------------------------
#include "gdcmMeta.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmMeta
 * \brief   
 * @param   begin iterator of begin for the object
 * @param   end   iterator of end   for the object
 */
gdcmMeta::gdcmMeta(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

/**
 * \ingroup gdcmMeta
 * \brief   Canonical destructor.
 */
gdcmMeta::~gdcmMeta() 
{
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmMeta
 * \brief   Prints the Object
 * @return
 */ 
void gdcmMeta::Print(std::ostream &os)
{
   os<<"META :"<<std::endl;
   gdcmObject::Print(os);
}

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
