// gdcmHeaderIdo.cxx

#include "gdcmIdo.h"
#include "gdcmUtil.h"

gdcmHeaderIdo::gdcmHeaderIdo (char* InFilename)
	: gdcmHeader(InFilename)
{ }

void gdcmHeaderIdo::BuildHeader(void) {
	 gdcmHeaderIdo::BuildHeader();
	setAcrLibido();
}

/**
 * \ingroup gdcmHeaderIdo
 * \brief   Si le recognition code (0008,0010) s'avere etre "ACR_LIBIDO",
 *          alors valide la reconnaissance du fichier en positionnant
 *          filetype.
 */
void gdcmHeaderIdo::setAcrLibido(void) {
	string RecCode;
	
	RecCode = GetPubElValByNumber(0x0008, 0x0010);
	if (RecCode == "ACRNEMA_LIBIDO" ||
	    RecCode == "CANRME_AILIBOD" )
		filetype = ACR_LIBIDO;
	return;
}
