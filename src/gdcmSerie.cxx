// gdcmSequence.cxx
//-----------------------------------------------------------------------------
#include "gdcmSerie.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
gdcmSerie::gdcmSerie(ListTag::iterator begin,ListTag::iterator end):
   gdcmObject(begin,end)
{
}

gdcmSerie::~gdcmSerie() 
{
   for(ListImage::iterator cc = images.begin();cc != images.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
void gdcmSerie::Print(std::ostream &os)
{
   os<<"SERIE"<<std::endl;
   gdcmObject::Print(os);

   for(ListImage::iterator cc = images.begin();cc != images.end();++cc)
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
