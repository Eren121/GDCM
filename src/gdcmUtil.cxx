// $Header: /cvs/public/gdcm/src/gdcmUtil.cxx,v 1.19 2003/09/24 11:37:10 jpr Exp $

//This is needed when compiling in debug mode
#ifdef _MSC_VER
// 'identifier' : class 'type' needs to have dll-interface to be used by
// clients of class 'type2'
#pragma warning ( disable : 4251 )
// 'identifier' : identifier was truncated to 'number' characters in the
// debug information
#pragma warning ( disable : 4786 )
#endif //_MSC_VER

#include <stdio.h>
#include <ctype.h>   // For isspace
#include <string.h>
#include "gdcmUtil.h"

// Library globals.
gdcmDebug dbg;

gdcmDebug::gdcmDebug(int level) {
   DebugLevel = level;
}

void gdcmDebug::Verbose(int Level, const char * Msg1, const char * Msg2) {
   if (Level > DebugLevel)
      return ;
   std::cerr << Msg1 << ' ' << Msg2 << '\n';
}

void gdcmDebug::Assert(int Level, bool Test,
                 const char * Msg1, const char * Msg2) {
   if (Level > DebugLevel)
      return ;
   if (!Test)
      std::cerr << Msg1 << ' ' << Msg2 << '\n';
}

void gdcmDebug::Error( bool Test, const char * Msg1, const char * Msg2) {
   if (!Test)
      return;
   std::cerr << Msg1 << ' ' << Msg2 << '\n';
   Exit(1);
}

void gdcmDebug::Error(const char* Msg1, const char* Msg2,
                      const char* Msg3) {
   std::cerr << Msg1 << ' ' << Msg2 << ' ' << Msg3 << '\n';
   Exit(1);
}

void gdcmDebug::Exit(int a) {
#ifdef __GNUC__
   std::exit(a);
#endif
#ifdef _MSC_VER
   exit(a);    // Found in #include <stdlib.h>
#endif
}

///////////////////////////////////////////////////////////////////////////
gdcmVR      * gdcmGlobal::VR    = (gdcmVR*)0;
gdcmTS      * gdcmGlobal::TS    = (gdcmTS*)0;
gdcmDictSet * gdcmGlobal::Dicts = (gdcmDictSet*)0;
gdcmGlobal gdcmGlob;

gdcmGlobal::gdcmGlobal(void) {
   if (VR || TS || Dicts)
      dbg.Verbose(0, "gdcmGlobal::gdcmGlobal : VR or TS or Dicts already allocated");
   VR = new gdcmVR();
   TS = new gdcmTS();
   Dicts = new gdcmDictSet();
}

gdcmGlobal::~gdcmGlobal() {
   delete VR;
   delete TS;
   delete Dicts;
}

gdcmVR * gdcmGlobal::GetVR(void) {
   return VR;
}

gdcmTS * gdcmGlobal::GetTS(void) {
   return TS;
}
gdcmDictSet * gdcmGlobal::GetDicts(void) {
   return Dicts;
}

///////////////////////////////////////////////////////////////////////////
// Because is not yet available in g++2.96
istream& eatwhite(istream& is) {
   char c;
   while (is.get(c)) {
      if (!isspace(c)) {
         is.putback(c);
         break;
      }
   }
   return is;
}

///////////////////////////////////////////////////////////////////////////
// Because is not  available in C++ (?)

void Tokenize (const string& str,
               vector<string>& tokens,
               const string& delimiters) {
   string::size_type lastPos = str.find_first_not_of(delimiters,0);
   string::size_type pos     = str.find_first_of    (delimiters,lastPos);
   while (string::npos != pos || string::npos != lastPos) {
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      lastPos = str.find_first_not_of(delimiters, pos);
      pos     = str.find_first_of    (delimiters, lastPos);
   }
}


///////////////////////////////////////////////////////////////////////////
// to prevent a flashing screen when non-printable character

char * _cleanString(char *v) {
   char *d;
   int i, l;
   l = strlen(v);
   for (i=0,d=v; 
        i<l ; 
        i++,d++) {
      if (!isprint(*d))
         *d = '.';
      }	
   return v;
}


///////////////////////////////////////////////////////////////////////////
// to prevent a flashing screen when non-printable character

std::string _CreateCleanString(std::string s) {
/*   char *d, *di, *v;
   int i, l;
   v=(char*)s.c_str();
   l = strlen(v);
   d = di = strdup(v);
   for (i=0; 
        i<l ; 
        i++,di++,v++) {
      if (!isprint(*v))
         *di = '.';
      }	
   return d;*/
  string str=s;
  for(int i=0;i<str.size();i++)
  {
    if(!isprint(str[i]))
      str[i]='.';
  }

  return(str);
}

///////////////////////////////////////////////////////////////////////////
//
// because it may not be associated to a dictionary ...

std::string TranslateToKey(guint16 group, guint16 element) {
	char trash[10];
	string key;
	// CLEAN ME: better call the iostream<< with the hex manipulator on.
	// This requires some reading of the stdlibC++ sources to make the
	// proper call (or copy).
	sprintf(trash, "%04x|%04x", group , element);
	key = trash;  // Convertion through assignement
	return key;
}



