// gdcmUtil.h
//-----------------------------------------------------------------------------
#ifndef GDCMUTIL_H
#define GDCMUTIL_H

#include "gdcmCommon.h"
#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmDictSet.h"
#include "gdcmDicomDirElement.h"
#include <vector>
#include <string>

//-----------------------------------------------------------------------------
/*
 * This class contains all globals elements that might be
 * instanciated only one time
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
std::istream & eatwhite(std::istream & is);

void Tokenize (const std::string& str,
               std::vector<std::string>& tokens,
               const std::string& delimiters = " ");

char *_cleanString(char *v);
std::string _CreateCleanString(std::string s);

void NormalizePath (std::string &name);
std::string GetPath(std::string &fullName);
std::string GetName(std::string &fullName);
#endif
