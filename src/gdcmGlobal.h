// gdcmGlobal.h
//-----------------------------------------------------------------------------
#ifndef GDCMGLOBAL_H
#define GDCMGLOBAL_H

#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmDictSet.h"
#include "gdcmDicomDirElement.h"

//-----------------------------------------------------------------------------
/**
 * \brief   This class contains all globals elements that might be
 *          instanciated only once (singletons).
 */
class GDCM_EXPORT gdcmGlobal {
public:
   gdcmGlobal(void);
   ~gdcmGlobal();

   static gdcmDictSet *GetDicts(void);
   static gdcmVR *GetVR(void);
   static gdcmTS *GetTS(void);
   static gdcmDicomDirElement *GetDicomDirElements(void);

private:
   static gdcmDictSet *Dicts; 
   static gdcmVR *VR;
   static gdcmTS *TS; 
   static gdcmDicomDirElement *ddElem;
};

//-----------------------------------------------------------------------------
#endif
