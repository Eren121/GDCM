// gdcmDicomDirMeta.h
//-----------------------------------------------------------------------------
#include "gdcmDicomDirMeta.h"
#include "gdcmDocument.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup gdcmMeta
 * \brief  Constructor
 */ 
gdcmDicomDirMeta::gdcmDicomDirMeta(TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
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
