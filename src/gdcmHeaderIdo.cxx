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
 * \brief   Pour les fichiers non TrueDicom, si le recognition
 *          code (0008,0010) s'avere etre "ACR_LIBIDO", alors
 *          valide la reconnaissance du fichier en positionnant
 *          filetype.
 */
void gdcmHeaderIdo::setAcrLibido(void) {
	string RecCode;
	
	if (   (filetype != TrueDicom)
		 && (filetype != ExplicitVR)
		 && (filetype != ImplicitVR) ) {
		printf("_setAcrLibido expects a presumably ACR file\n");
		// Recognition Code  --> n'existe plus en DICOM V3 ...
		RecCode = GetPubElValByNumber(0x0008, 0x0010);
		// FIXME NOW
		if (RecCode == "ACRNEMA_LIBIDO" ||
		    RecCode == "CANRME_AILIBOD" )
			filetype = ACR_LIBIDO;
		else
			filetype = ACR;
	}
	return;
}
