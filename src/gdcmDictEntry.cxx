// gdcmDictEntry.cxx
//-----------------------------------------------------------------------------
#include "gdcmDictEntry.h"

#include <stdio.h>    // FIXME For sprintf
#include "gdcmUtil.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDictEntry
 * \brief   Constructor
 * @param   InGroup    DICOM-Group Number
 * @param   InElement  DICOM-Element Number
 * @param   InVr       Value Representatiion
 * @param   InFourth  // DO NOT use any longer; 
 *                       NOT part of the Dicom Standard
 * @param   InName    description of the element
*/

gdcmDictEntry::gdcmDictEntry(guint16 InGroup, guint16 InElement,
                             std::string  InVr, std::string InFourth,
                             std::string  InName) {
	group 	= InGroup;
	element = InElement;
	vr      = InVr;
	fourth 	= InFourth;
	name    = InName;
	key     = TranslateToKey(group, element);
}

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup gdcmDictEntry
 * \brief   concatenates 2 guint16 (supposed to be a Dicom group number 
 *                                             and a Dicom element number)
 * @param  group the Dicom group   number used to build the tag
 * @param  element the Dicom element number used to build the tag
 * @return the built tag
 */

TagKey gdcmDictEntry::TranslateToKey(guint16 group, guint16 element) {
	char trash[10];
	TagKey key;
	// CLEAN ME: better call the iostream<< with the hex manipulator on.
	// This requires some reading of the stdlibC++ sources to make the
	// proper call (or copy).
	sprintf(trash, "%04x|%04x", group , element);
	key = trash;  // Convertion through assignement
	return key;
}

/**
 * \ingroup     gdcmDictEntry
 * \brief       If-and only if-the V(alue) R(epresentation)
 * \            is unset then overwrite it.
 * @param NewVr New V(alue) R(epresentation) to be set.
 */
void gdcmDictEntry::SetVR(std::string NewVr) 
{
	if ( IsVRUnknown() )
		vr = NewVr;
	else 
   {
		dbg.Error(true, "gdcmDictEntry::SetVR",
		          "Overwriting vr might compromise a dictionary");
	}
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

