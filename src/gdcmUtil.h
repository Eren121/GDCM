// gdcmUtil.h

#include <iostream>
#include <vector>
#include <string>
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

istream & eatwhite(istream & is);

void Tokenize (const string& str,
               vector<string>& tokens,
               const string& delimiters = " ");

extern gdcmDebug dbg;
