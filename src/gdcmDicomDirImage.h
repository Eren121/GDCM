// gdcmDicomDirImage.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMIMAGE_H
#define GDCMDICOMIMAGE_H

#include "gdcmObject.h"

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirImage : public gdcmObject {
public:
   gdcmDicomDirImage(gdcmSQItem *s, TagDocEntryHT *ptagHT); 
   gdcmDicomDirImage(TagDocEntryHT *ptagHT); 

   ~gdcmDicomDirImage(void);

   virtual void Print(std::ostream &os = std::cout);      
};

//-----------------------------------------------------------------------------
#endif
