// gdcmVR.h

#ifndef GDCMVR_H
#define GDCMVR_H

#include <map>
#include <string>
#include "gdcmCommon.h"

typedef std::string VRKey;
typedef std::string VRAtr;
typedef std::map<VRKey, VRAtr> VRHT;    // Value Representation Hash Table

/// Container for dicom Value Representation Hash Table
/// \note   This is a singleton
class GDCM_EXPORT gdcmVR {
private:
   VRHT vr;
public:
	gdcmVR(void);
   ~gdcmVR();
   int Count(VRKey key);
};

#endif
