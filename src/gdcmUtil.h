// $Header: /cvs/public/gdcm/src/gdcmUtil.h,v 1.10 2003/05/21 16:26:28 regrain Exp $

#ifndef GDCMUTIL_H
#define GDCMUTIL_H

#include <iostream>
#include <vector>
#include <string>
#include "gdcmVR.h"
#include "gdcmDictSet.h"

class gdcmDebug {
private:
	int DebugLevel;
public:
	gdcmDebug(int  = 0);
	void Verbose(int, const char*, const char* ="");
	void Error(bool, const char*,  const char* ="");
	void Error(const char*, const char* ="", const char* ="");
	void Assert(int, bool, const char*, const char*);
	void Exit(int);
};

class gdcmGlobal {
private:
   static gdcmVR *VR; 
   static gdcmDictSet *Dicts; 
public:
   gdcmGlobal(void);
   ~gdcmGlobal();
   static gdcmVR * GetVR(void);
   static gdcmDictSet * GetDicts(void);
};

std::istream & eatwhite(std::istream & is);

void Tokenize (const std::string& str,
               std::vector<std::string>& tokens,
               const std::string& delimiters = " ");

extern gdcmDebug dbg;

#endif
