#include <ctype.h>
#include "gdcmUtil.h"

gdcmDebug::gdcmDebug(int level = 0) {
	DebugLevel = level;
}

void gdcmDebug::Verbose(int Level, const char * Msg1, const char * Msg2 = "") {
	if (Level > DebugLevel)
		return ;
	cerr << Msg1 << ' ' << Msg2 << '\n';
}

void gdcmDebug::Assert(int Level, bool Test,
                 const char * Msg1, const char * Msg2 = "") {
	if (Level > DebugLevel)
		return ;
	if (!Test)
		cerr << Msg1 << ' ' << Msg2 << '\n';
}

void gdcmDebug::Error( bool Test, const char * Msg1, const char * Msg2 = "") {
	if (!Test)
		return;
	std::cerr << Msg1 << ' ' << Msg2 << '\n';
	std::exit(1);
}

void gdcmDebug::Error(const char* Msg1, const char* Msg2 ="",
                      const char* Msg3 ="") {
	std::cerr << Msg1 << ' ' << Msg2 << ' ' << Msg3 << '\n';
	std::exit(1);
}

gdcmDebug dbg;

// Because is not yet available in g++2.06
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

