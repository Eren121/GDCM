// gdcmSerie.h
//-----------------------------------------------------------------------------
#ifndef GDCMSERIE_H
#define GDCMSERIE_H

#include "gdcmObject.h"
#include "gdcmImage.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmImage *> ListImage;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmSerie : public gdcmObject 
{
public:
   gdcmSerie(ListTag::iterator begin,ListTag::iterator end);
   ~gdcmSerie(void);

   virtual void Print(std::ostream &os = std::cout);

   inline ListImage &GetImages() {return images;};
   inline void AddImage(gdcmImage *obj) {images.push_back(obj);};

private:
   ListImage images;
};

//-----------------------------------------------------------------------------
#endif
