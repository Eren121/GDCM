// gdcmIdo.h

#include "gdcmHeader.h"

/*
 * A gdcmHeaderIdo is a specific gdcmHeader that recognize
 * the creation's source of the file : the libIdo library.
 * Thus the file type is modified in consequence.
 *
 * \see gdcmHeader
 */
class gdcmHeaderIdo: protected gdcmHeader {
private:
	void setAcrLibido(void);
	void BuildHeader(void);

public:
	gdcmHeaderIdo(char* filename);
};
