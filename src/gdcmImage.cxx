// gdcmImage.cxx
//-----------------------------------------------------------------------------
#include "gdcmImage.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmImage
 * \brief   
 * @param   begin iterator of begin for the object
 * @param   end   iterator of end for the object
 */
gdcmImage::gdcmImage(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

/**
 * \ingroup gdcmImage
 * \brief   Canonical destructor.
 */
gdcmImage::~gdcmImage() 
{
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmImage
 * \brief   Prints the Object
 * @return
 */ 
void gdcmImage::Print(std::ostream &os)
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
