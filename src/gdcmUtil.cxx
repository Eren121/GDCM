// gdcmUtil.cxx

#include <ctype.h>   // For isspace
#include "gdcmUtil.h"

gdcmDebug::gdcmDebug(int level) {
	DebugLevel = level;
}

void gdcmDebug::Verbose(int Level, const char * Msg1, const char * Msg2) {
	if (Level > DebugLevel)
		return ;
	cerr << Msg1 << ' ' << Msg2 << '\n';
}

void gdcmDebug::Assert(int Level, bool Test,
                 const char * Msg1, const char * Msg2) {
	if (Level > DebugLevel)
		return ;
	if (!Test)
		cerr << Msg1 << ' ' << Msg2 << '\n';
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

gdcmDebug dbg;

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

void Tokenize (const string& str,
               vector<string>& tokens,
               const string& delimiters = " ") {
   string::size_type lastPos = str.find_first_not_of(delimiters,0);
   string::size_type pos     = str.find_first_of(delimiters,lastPos);
   while (string::npos != pos || string::npos != lastPos) {
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      lastPos = str.find_first_not_of(delimiters, pos);
      pos = str.find_first_of(delimiters, lastPos);
   }
}
