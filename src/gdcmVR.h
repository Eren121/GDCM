// gdcmVR.h

#ifndef GDCMVR_H
#define GDCMVR_H

#include <map>
#include <string>
#include "gdcmCommon.h"

typedef string VRKey;
typedef string VRAtr;
typedef map<VRKey, VRAtr> VRHT;    // Value Representation Hash Table

/*
 * \defgroup gdcmVR
 * \brief  Container for dicom Value Representation Hash Table
 * \note   This is a singleton
 */
class GDCM_EXPORT gdcmVR {
private:
   VRHT *dicom_vr;
public:
	gdcmVR();
   ~gdcmVR();
   int Count(VRKey);
};

#endif
