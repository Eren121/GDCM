// gdcmDicomDirImage.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDirImage.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDicomDirImage
 * \brief   
 * @param   begin iterator of begin for the object
 * @param   end   iterator of end for the object
 */
gdcmDicomDirImage::gdcmDicomDirImage(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

/**
 * \ingroup gdcmImage
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

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
