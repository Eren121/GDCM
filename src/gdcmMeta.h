// gdcmMeta.h
//-----------------------------------------------------------------------------
#ifndef GDCMMETA_H
#define GDCMMETA_H

#include "gdcmObject.h"

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmMeta : public gdcmObject 
{
public:
   gdcmMeta(ListTag::iterator begin,ListTag::iterator end);
   ~gdcmMeta(void);

   virtual void Print(std::ostream &os = std::cout);
};

//-----------------------------------------------------------------------------
#endif
