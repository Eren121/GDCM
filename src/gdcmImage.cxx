// gdcmImage.cxx
//-----------------------------------------------------------------------------
#include "gdcmImage.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
gdcmImage::gdcmImage(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

gdcmImage::~gdcmImage() 
{
}

//-----------------------------------------------------------------------------
// Print
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
