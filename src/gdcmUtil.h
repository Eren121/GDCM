// gdcmUtil.h

#ifndef GDCMUTIL_H
#define GDCMUTIL_H

#include <iostream>
#include <vector>
#include <string>
#include "gdcmVR.h"
using namespace std;

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
public:
   gdcmGlobal(void);
   ~gdcmGlobal(void);
   static gdcmVR * GetVR(void);
};

istream & eatwhite(istream & is);

void Tokenize (const string& str,
               vector<string>& tokens,
               const string& delimiters = " ");

extern gdcmDebug dbg;

#endif
