// gdcmSerie.h
//-----------------------------------------------------------------------------
#ifndef GDCMSERIE_H
#define GDCMSERIE_H

#include "gdcmObject.h"
#include "gdcmImage.h"

//-----------------------------------------------------------------------------

typedef std::list<gdcmImage *> lImage;

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmSerie : public gdcmObject {
public:

   gdcmSerie();
   ~gdcmSerie();

   inline lImage &GetImages() {return images;};
   
   lImage images;
   
private:


};

//-----------------------------------------------------------------------------
#endif
