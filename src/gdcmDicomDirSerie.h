// gdcmDicomDirSerie.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMDIRSERIE_H
#define GDCMDICOMDIRSERIE_H

#include "gdcmObject.h"
#include "gdcmDicomDirImage.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmDicomDirImage *> ListDicomDirImage;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirSerie : public gdcmObject 
{
public:
   gdcmDicomDirSerie(ListTag::iterator begin,ListTag::iterator end);
   ~gdcmDicomDirSerie(void);

   virtual void Print(std::ostream &os = std::cout);

   inline ListDicomDirImage &GetDicomDirImages() {return images;};
   inline void AddDicomDirImage(gdcmDicomDirImage *obj) {images.push_back(obj);};

private:
   ListDicomDirImage images;
};

//-----------------------------------------------------------------------------
#endif
