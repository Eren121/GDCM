// gdcmDicomDirImage.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMIMAGE_H
#define GDCMDICOMIMAGE_H

#include "gdcmObject.h"

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirImage : public gdcmObject 
{
public:
   gdcmDicomDirImage(ListTag::iterator begin,ListTag::iterator end);
   ~gdcmDicomDirImage(void);

   virtual void Print(std::ostream &os = std::cout);
};

//-----------------------------------------------------------------------------
#endif
