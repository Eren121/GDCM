// gdcmDicomDirImage.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDirImage.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup gdcmDicomDirImage
 * \brief   
 * @param  begin  iterator (inside the gdcmParser chained list)
 *                on the first Header Entry (i.e Dicom Element)
 *                related to this "IMAGE" part
 * @param  end  iterator  (inside the gdcmParser chained list)
 *              on the last Header Entry (i.e Dicom Element) 
 *              related to this 'IMAGE' part
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 * @param plistEntries pointer to the chained List (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 */
gdcmDicomDirImage::gdcmDicomDirImage(ListTag::iterator begin,
                                     ListTag::iterator end,
                                     TagHeaderEntryHT *ptagHT, 
				     ListTag *plistEntries):
   gdcmObject(begin,end,ptagHT,plistEntries)
{
}

/**
 * \ingroup gdcmDicomDirImage
 * \brief   Canonical destructor.
 */
gdcmDicomDirImage::~gdcmDicomDirImage() 
{
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDicomDirImage
 * \brief   Prints the Object
 * @return
 */ 
void gdcmDicomDirImage::Print(std::ostream &os)
{
   os<<"IMAGE : ";
   for(ListTag::iterator i=beginObj;i!=endObj;++i)
   {
      if( ((*i)->GetGroup()==0x0004) && ((*i)->GetElement()==0x1500) )
         os<<(*i)->GetValue();
   }
   os<<std::endl;

   gdcmObject::Print(os);
}

/**
 * \ingroup gdcmDicomDirImage
 * \brief   TODO
 * @return
 */
gdcmDicomDirImage* gdcmDicomDirImage::NewImage(void) {}

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
