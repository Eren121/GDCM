#include "gdcmlib.h"

class gdcmHeaderIdo: protected gdcmHeader {
private:
	void setAcrLibido(void);
	void BuildHeader(void);
public:
	gdcmHeaderIdo(char* filename);
};
