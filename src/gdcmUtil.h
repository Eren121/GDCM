// $Header: /cvs/public/gdcm/src/gdcmUtil.h,v 1.16 2003/10/02 11:26:16 malaterre Exp $

#ifndef GDCMUTIL_H
#define GDCMUTIL_H

#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmDictSet.h"
#include <iostream>
#include <vector>
#include <string>

class gdcmDebug {
private:
	int DebugLevel;
public:
	gdcmDebug(int  = 0);
	void Verbose(int, const char*, const char* ="");
//	void Verbose(int, char*, char*);
	void Error(bool, const char*,  const char* ="");
	void Error(const char*, const char* ="", const char* ="");
	void Assert(int, bool, const char*, const char*);
	void Exit(int);
	void SetDebug (int i) {DebugLevel = i;}
};

class gdcmGlobal {
private:
   static gdcmVR *VR;
   static gdcmTS *TS; 
   static gdcmDictSet *Dicts; 
public:
   gdcmGlobal(void);
   ~gdcmGlobal();
   static gdcmVR * GetVR(void);
   static gdcmTS * GetTS(void);
   static gdcmDictSet * GetDicts(void);
};

std::istream & eatwhite(std::istream & is);

void Tokenize (const std::string& str,
               std::vector<std::string>& tokens,
               const std::string& delimiters = " ");

extern gdcmDebug dbg;

char * _cleanString(char *v);
std::string _CreateCleanString(std::string s);

std::string TranslateToKey(guint16 group, guint16 element);

#endif

