#include <iostream>
class gdcmDebug {
private:
	int DebugLevel;
public:
	gdcmDebug(int);
	void Verbose(int, const char*, const char* ="");
	void Error(bool, const char*, const char*);
	void Error(const char*, const char* ="", const char* ="");
	void Assert(int, bool, const char*, const char*);
};

istream& eatwhite(istream& is);

extern gdcmDebug dbg;
