// gdcmImage.h
//-----------------------------------------------------------------------------
#ifndef GDCMIMAGE_H
#define GDCMIMAGE_H

#include "gdcmObject.h"

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmImage : public gdcmObject {
public:

   gdcmImage();
   ~gdcmImage();

   std::string GetEntryByNumber(guint16 group, guint16 element);
   std::string GetEntryByName(TagName name);


};

//-----------------------------------------------------------------------------
#endif
