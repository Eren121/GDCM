// gdcmDicomDirMeta.h
//-----------------------------------------------------------------------------
#include "gdcmDicomDirMeta.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup gdcmMeta
 * \brief  Constructor
 * @param  begin  iterator (inside the gdcmParser chained list)
 *                on the first Header Entry (i.e Dicom Element)
 *                related to the "META" part
 * @param  end  iterator  (inside the gdcmParser chained list)
 *              on the last Header Entry (i.e Dicom Element) 
 *              related to the 'META' part
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 * @param plistEntries pointer to the chained List (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 */ 
gdcmDicomDirMeta::gdcmDicomDirMeta(ListTag::iterator begin,
                                   ListTag::iterator end,
				   TagHeaderEntryHT *ptagHT, 
				   ListTag *plistEntries):
   gdcmObject(begin,end,ptagHT,plistEntries)
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
