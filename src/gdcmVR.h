// gdcmVR.h
//-----------------------------------------------------------------------------
#ifndef GDCMVR_H
#define GDCMVR_H

#include "gdcmCommon.h"
#include <map>
#include <string>
#include <iostream>

//-----------------------------------------------------------------------------
typedef std::string VRKey;
typedef std::string VRAtr;
typedef std::map<VRKey, VRAtr> VRHT;    // Value Representation Hash Table

//-----------------------------------------------------------------------------
/*
 * Container for dicom Value Representation Hash Table
 * \note   This is a singleton
 */
class GDCM_EXPORT gdcmVR 
{
public:
	gdcmVR(void);
   ~gdcmVR();

   void Print(std::ostream &os = std::cout);

   int Count(VRKey key);

private:
   VRHT vr;
};

//-----------------------------------------------------------------------------
#endif
