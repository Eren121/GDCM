// gdcmHeaderIdo.cxx

//This is needed when compiling in debug mode
#ifdef _MSC_VER
// 'identifier' : class 'type' needs to have dll-interface to be used by
// clients of class 'type2'
#pragma warning ( disable : 4251 )
// 'identifier' : identifier was truncated to 'number' characters in the
// debug information
#pragma warning ( disable : 4786 )
#endif //_MSC_VER

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
	std::string RecCode;
	
	RecCode = GetPubElValByNumber(0x0008, 0x0010);
	if (RecCode == "ACRNEMA_LIBIDO" ||
	    RecCode == "CANRME_AILIBOD" )
		filetype = ACR_LIBIDO;
	return;
}
