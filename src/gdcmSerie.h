// gdcmSerie.h
//-----------------------------------------------------------------------------
#ifndef GDCMSERIE_H
#define GDCMSERIE_H

#include "gdcmObject.h"
#include "gdcmImage.h"

//-----------------------------------------------------------------------------

typedef std::list<gdcmImage> lImage;

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmSerie : public gdcmObject {
public:

   gdcmSerie();
   ~gdcmSerie();

   std::string GetEntryByNumber(guint16 group, guint16 element);
   std::string GetEntryByName(TagName name);

   inline lImage GetImages() {return images;};

private:

   lImage images;

};

//-----------------------------------------------------------------------------
#endif
