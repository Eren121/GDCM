// gdcmSequence.h
//-----------------------------------------------------------------------------
#ifndef GDCMSEQUENCE_H
#define GDCMSEQUENCE_H

#include "gdcmObject.h"
#include "gdcmImage.h"

//-----------------------------------------------------------------------------

typedef std::list<gdcmImage> lImage;

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmSequence : public gdcmObject {
public:

   gdcmSequence();
   ~gdcmSequence();

   std::string GetEntryByNumber(guint16 group, guint16 element);
   std::string GetEntryByName(TagName name);

   inline lImage GetImages() {return images;};

private:

   lImage images;

};

//-----------------------------------------------------------------------------
#endif
