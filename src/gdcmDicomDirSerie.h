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
   gdcmDicomDirSerie(gdcmSQItem *s, TagDocEntryHT *ptagHT); 
   gdcmDicomDirSerie(TagDocEntryHT *ptagHT); 
   ~gdcmDicomDirSerie(void);

   virtual void Print(std::ostream &os = std::cout);
/**
 * \ingroup gdcmDicomDirSerie
 * \brief   returns the IMAGE chained List for this SERIE.
 */
   inline ListDicomDirImage &GetDicomDirImages() 
      {return images;};
/**
 * \ingroup gdcmDicomDirSerie
 * \brief   adds the passed IMAGE to the IMAGE chained List for this SERIE.
 */       
   inline void AddDicomDirImage(gdcmDicomDirImage *obj) 
      {images.push_back(obj);};

/**
 * \ingroup gdcmDicomDirSerie
 * \brief   TODO
 */ 
   gdcmDicomDirImage* NewImage(void);
    
private:
/**
* \brief chained list of DicomDirImages
*/ 
   ListDicomDirImage images;
};

//-----------------------------------------------------------------------------
#endif
