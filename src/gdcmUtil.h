// gdcmUtil.h
//-----------------------------------------------------------------------------
#ifndef GDCMUTIL_H
#define GDCMUTIL_H

#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmDictSet.h"
#include <iostream>
#include <vector>
#include <string>

//-----------------------------------------------------------------------------
/*
 * gdcmDebug is an object for debugging in program.
 * It has 2 debugging modes :
 *  - error : for bad library use
 *  - debug : for debugging messages
 * 
 * A debugging message has a level of priority and is 
 * Shown only when the debug level is higher than the 
 * message level.
 */
class gdcmDebug {
public:
	gdcmDebug(int  = 0);
	void SetDebug (int i) {DebugLevel = i;}

   void Verbose(int, const char*, const char* ="");
	void Error(bool, const char*,  const char* ="");
	void Error(const char*, const char* ="", const char* ="");

	void Assert(int, bool, const char*, const char*);
	void Exit(int);

private:
	int DebugLevel;
};

//-----------------------------------------------------------------------------
/*
 * This class contains all globals elements that might be
 * instanciated only one time
 */
class gdcmGlobal {
public:
   gdcmGlobal(void);
   ~gdcmGlobal();

   static gdcmVR * GetVR(void);
   static gdcmTS * GetTS(void);
   static gdcmDictSet * GetDicts(void);

private:
   static gdcmVR *VR;
   static gdcmTS *TS; 
   static gdcmDictSet *Dicts; 
};

//-----------------------------------------------------------------------------
std::istream & eatwhite(std::istream & is);

void Tokenize (const std::string& str,
               std::vector<std::string>& tokens,
               const std::string& delimiters = " ");

extern gdcmDebug dbg;

char * _cleanString(char *v);
std::string _CreateCleanString(std::string s);

//-----------------------------------------------------------------------------
#endif

