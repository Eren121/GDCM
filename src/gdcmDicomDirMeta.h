// gdcmDicomDirMeta.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMDIRMETA_H
#define GDCMDICOMDIRMETA_H

#include "gdcmObject.h"

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirMeta : public gdcmObject 
{
public:
   gdcmDicomDirMeta(ListTag::iterator begin,ListTag::iterator end,
                    TagHeaderEntryHT *ptagHT, ListTag *plistEntries); 

   ~gdcmDicomDirMeta(void);

   virtual void Print(std::ostream &os = std::cout);
};

//-----------------------------------------------------------------------------
#endif
