// gdcmHeader.cxx

#include "gdcm.h"
#include <stdio.h>
// For nthos:
#ifdef _MSC_VER
#include <winsock.h>
#else
#include <netinet/in.h>
#endif
#include <cctype>		// for isalpha
#include <map>
#include <sstream>
#include "gdcmUtil.h"

#define HEADER_LENGTH_TO_READ 256 // on ne lit plus que le debut

#define DEBUG 1

namespace Error {
	struct FileReadError {
		FileReadError(FILE* fp, const char* Mesg) {
			if (feof(fp))
				dbg.Verbose(1, "EOF encountered :", Mesg);
			if (ferror(fp))
				dbg.Verbose(1, "Error on reading :", Mesg);
		}
	};
}

//FIXME: this looks dirty to me...

#define str2num(str, typeNum) *((typeNum *)(str))

// str est un pointeur dans un tableau de caractères, qui doit contenir, 
// à cet endroit la, la représentation binaire d'un entier (16 ou 32 bits)
// je veux récupérer ça ... dans un entier.
// s'il y a une autre solution, évitant des cast et les indirections,
// je suis preneur

VRHT * gdcmHeader::dicom_vr = (VRHT*)0;
gdcmDictSet* gdcmHeader::Dicts = new gdcmDictSet();

void gdcmHeader::Initialise(void) {
	if (!gdcmHeader::dicom_vr)
		InitVRDict();
	RefPubDict = gdcmHeader::Dicts->GetDefaultPublicDict();
	RefShaDict = (gdcmDict*)0;
}

gdcmHeader::gdcmHeader (const char* InFilename) {
	SetMaxSizeLoadElementValue(1024);
	filename = InFilename;
	Initialise();
	fp=fopen(InFilename,"rw");
	dbg.Error(!fp, "gdcmHeader::gdcmHeader cannot open file", InFilename);
	ParseHeader();
}

gdcmHeader::~gdcmHeader (void) {
	fclose(fp);
	return;
}

void gdcmHeader::InitVRDict (void) {
	if (dicom_vr) {
		dbg.Verbose(0, "gdcmHeader::InitVRDict:", "VR dictionary allready set");
		return;
	}
	VRHT *vr = new VRHT;
	(*vr)["AE"] = "Application Entity";       // At most 16 bytes
	(*vr)["AS"] = "Age String";               // Exactly 4 bytes
	(*vr)["AT"] = "Attribute Tag";            // 2 16-bit unsigned short integers
	(*vr)["CS"] = "Code String";              // At most 16 bytes
	(*vr)["DA"] = "Date";                     // Exactly 8 bytes
	(*vr)["DS"] = "Decimal String";           // At most 16 bytes
	(*vr)["DT"] = "Date Time";                // At most 26 bytes
	(*vr)["FL"] = "Floating Point Single";    // 32-bit IEEE 754:1985 float
	(*vr)["FD"] = "Floating Point Double";    // 64-bit IEEE 754:1985 double
	(*vr)["IS"] = "Integer String";           // At most 12 bytes
	(*vr)["LO"] = "Long String";              // At most 64 chars
	(*vr)["LT"] = "Long Text";                // At most 10240 chars
	(*vr)["OB"] = "Other Byte String";        // String of bytes (vr independant)
	(*vr)["OW"] = "Other Word String";        // String of 16-bit words (vr dep)
	(*vr)["PN"] = "Person Name";              // At most 64 chars
	(*vr)["SH"] = "Short String";             // At most 16 chars
	(*vr)["SL"] = "Signed Long";              // Exactly 4 bytes
	(*vr)["SQ"] = "Sequence of Items";        // Not Applicable
	(*vr)["SS"] = "Signed Short";             // Exactly 2 bytes
	(*vr)["ST"] = "Short Text";               // At most 1024 chars
	(*vr)["TM"] = "Time";                     // At most 16 bytes
	(*vr)["UI"] = "Unique Identifier";        // At most 64 bytes
	(*vr)["UL"] = "Unsigned Long ";           // Exactly 4 bytes
	(*vr)["UN"] = "Unknown";                  // Any length of bytes
	(*vr)["US"] = "Unsigned Short ";          // Exactly 2 bytes
	(*vr)["UT"] = "Unlimited Text";           // At most 2^32 -1 chars
   dicom_vr = vr;	
}

/**
 * \ingroup gdcmHeader
 * \brief   Discover what the swap code is (among little endian, big endian,
 *          bad little endian, bad big endian).
 *
 */
void gdcmHeader::CheckSwap()
{
	// The only guaranted way of finding the swap code is to find a
	// group tag since we know it's length has to be of four bytes i.e.
	// 0x00000004. Finding the swap code in then straigthforward. Trouble
	// occurs when we can't find such group...
	guint32  s;
	guint32  x=4;  // x : pour ntohs
	bool net2host; // true when HostByteOrder is the same as NetworkByteOrder
	 
	int lgrLue;
	char * entCur;
	char deb[HEADER_LENGTH_TO_READ];
	 
	// First, compare HostByteOrder and NetworkByteOrder in order to
	// determine if we shall need to swap bytes (i.e. the Endian type).
	if (x==ntohs(x))
		net2host = true;
	else
		net2host = false;
	
	// The easiest case is the one of a DICOM header, since it possesses a
	// file preamble where it suffice to look for the sting "DICM".
	lgrLue = fread(deb, 1, HEADER_LENGTH_TO_READ, fp);
	
	entCur = deb + 128;
	if(memcmp(entCur, "DICM", (size_t)4) == 0) {
		filetype = TrueDicom;
		dbg.Verbose(1, "gdcmHeader::CheckSwap:", "looks like DICOM Version3");
	} else {
		filetype = Unknown;
		dbg.Verbose(1, "gdcmHeader::CheckSwap:", "not a DICOM Version3 file");
	}

	if(filetype == TrueDicom) {
		// Next, determine the value representation (VR). Let's skip to the
		// first element (0002, 0000) and check there if we find "UL", in
		// which case we (almost) know it is explicit VR.
		// WARNING: if it happens to be implicit VR then what we will read
		// is the length of the group. If this ascii representation of this
		// length happens to be "UL" then we shall believe it is explicit VR.
		// FIXME: in order to fix the above warning, we could read the next
		// element value (or a couple of elements values) in order to make
		// sure we are not commiting a big mistake.
		// We need to skip :
		// * the 128 bytes of File Preamble (often padded with zeroes),
		// * the 4 bytes of "DICM" string,
		// * the 4 bytes of the first tag (0002, 0000),
		// i.e. a total of  136 bytes.
		entCur = deb + 136;
		if(memcmp(entCur, "UL", (size_t)2) == 0) {
			filetype = ExplicitVR;
			dbg.Verbose(1, "gdcmHeader::CheckSwap:",
			            "explicit Value Representation");
		} else {
			filetype = ImplicitVR;
			dbg.Verbose(1, "gdcmHeader::CheckSwap:",
			            "not an explicit Value Representation");
		}

		if (net2host) {
			sw = 4321;
			dbg.Verbose(1, "gdcmHeader::CheckSwap:",
			               "HostByteOrder != NetworkByteOrder");
		} else {
			sw = 0;
			dbg.Verbose(1, "gdcmHeader::CheckSwap:",
			               "HostByteOrder = NetworkByteOrder");
		}
		
		// Position the file position indicator at first tag (i.e.
		// after the file preamble and the "DICM" string).
		rewind(fp);
		fseek (fp, 132L, SEEK_SET);
		return;
	} // End of TrueDicom

	// Alas, this is not a DicomV3 file and whatever happens there is no file
	// preamble. We can reset the file position indicator to where the data
	// is (i.e. the beginning of the file).
	rewind(fp);

	// Our next best chance would be to be considering a 'clean' ACR/NEMA file.
	// By clean we mean that the length of the first tag is written down.
	// If this is the case and since the length of the first group HAS to be
	// four (bytes), then determining the proper swap code is straightforward.

	entCur = deb + 4;
	s = str2num(entCur, guint32);
	
	switch (s) {
	case 0x00040000 :
		sw = 3412;
		filetype = ACR;
		return;
	case 0x04000000 :
		sw = 4321;
		filetype = ACR;
		return;
	case 0x00000400 :
		sw = 2143;
		filetype = ACR;
		return;
	case 0x00000004 :
		sw = 0;
		filetype = ACR;
		return;
	default :
		dbg.Verbose(0, "gdcmHeader::CheckSwap:",
		               "ACR/NEMA unfound swap info (time to raise bets)");
	}

	// We are out of luck. It is not a DicomV3 nor a 'clean' ACR/NEMA file.
	// It is time for despaired wild guesses. So, let's assume this file
	// happens to be 'dirty' ACR/NEMA, i.e. the length of the group is
	// not present. Then the only info we have is the net2host one.
	if (! net2host )
		sw = 0;
	else
		sw = 4321;
	return;
}

void gdcmHeader::SwitchSwapToBigEndian(void) {
	dbg.Verbose(1, "gdcmHeader::SwitchSwapToBigEndian",
	               "Switching to BigEndian mode.");
	if ( sw == 0    ) {
		sw = 4321;
		return;
	}
	if ( sw == 4321 ) {
		sw = 0;
		return;
	}
	if ( sw == 3412 ) {
		sw = 2143;
		return;
	}
	if ( sw == 2143 )
		sw = 3412;
}

void gdcmHeader::GetPixels(size_t lgrTotale, void* _Pixels) {
	size_t pixelsOffset; 
	pixelsOffset = GetPixelOffset();
	printf("pixelsOffset %d\n",pixelsOffset);
	fseek(fp, pixelsOffset, SEEK_SET);
	fread(_Pixels, 1, lgrTotale, fp);
}



/**
 * \ingroup   gdcmHeader
 * \brief     Find the value representation of the current tag.
 *
 * @param sw  code swap
 * @param skippedLength  pointeur sur nombre d'octets que l'on a saute qd
 *                       la lecture est finie
 * @param longueurLue    pointeur sur longueur (en nombre d'octets) 
 *                       effectivement lue
 * @return               longueur retenue pour le champ 
 */
 
// -->
// --> Oops
// --> C'etait la description de quoi, ca?
// -->

void gdcmHeader::FindVR( ElValue *ElVal) {
	if (filetype != ExplicitVR)
		return;

	char VR[3];
	string vr;
	int lgrLue;
	long PositionOnEntry = ftell(fp);
	// Warning: we believe this is explicit VR (Value Representation) because
	// we used a heuristic that found "UL" in the first tag. Alas this
	// doesn't guarantee that all the tags will be in explicit VR. In some
	// cases (see e-film filtered files) one finds implicit VR tags mixed
	// within an explicit VR file. Hence we make sure the present tag
	// is in explicit VR and try to fix things if it happens not to be
	// the case.
	bool RealExplicit = true;
	
	lgrLue=fread (&VR, (size_t)2,(size_t)1, fp);
	VR[2]=0;
	vr = string(VR);
		
	// Assume we are reading a falsely explicit VR file i.e. we reached
	// a tag where we expect reading a VR but are in fact we read the
	// first to bytes of the length. Then we will interogate (through find)
	// the dicom_vr dictionary with oddities like "\004\0" which crashes
	// both GCC and VC++ implementations of the STL map. Hence when the
	// expected VR read happens to be non-ascii characters we consider
	// we hit falsely explicit VR tag.

	if ( (!isalpha(VR[0])) && (!isalpha(VR[1])) )
		RealExplicit = false;

	// CLEANME searching the dicom_vr at each occurence is expensive.
	// PostPone this test in an optional integrity check at the end
	// of parsing or only in debug mode.
	if ( RealExplicit && !dicom_vr->count(vr) )
		RealExplicit = false;

	if ( RealExplicit ) {
		if ( ElVal->IsVrUnknown() ) {
			// When not a dictionary entry, we can safely overwrite the vr.
			ElVal->SetVR(vr);
			return; 
		}
		if ( ElVal->GetVR() == vr ) {
			// The vr we just read and the dictionary agree. Nothing to do.
			return;
		}
		// The vr present in the file and the dictionary disagree. We assume
		// the file writer knew best and use the vr of the file. Since it would
		// be unwise to overwrite the vr of a dictionary (since it would
		// compromise it's next user), we need to clone the actual DictEntry
		// and change the vr for the read one.
		gdcmDictEntry* NewTag = new gdcmDictEntry(ElVal->GetGroup(),
		                           ElVal->GetElement(),
		                           vr,
		                           "FIXME",
		                           ElVal->GetName());
		ElVal->SetDictEntry(NewTag);
		return; 
	}
	
	// We thought this was explicit VR, but we end up with an
	// implicit VR tag. Let's backtrack.
	dbg.Verbose(1, "gdcmHeader::FindVR:", "Falsely explicit vr file");
	fseek(fp, PositionOnEntry, SEEK_SET);
	// When this element is known in the dictionary we shall use, e.g. for
	// the semantics (see  the usage of IsAnInteger), the vr proposed by the
	// dictionary entry. Still we have to flag the element as implicit since
	// we know now our assumption on expliciteness is not furfilled.
	// avoid  .
	if ( ElVal->IsVrUnknown() )
		ElVal->SetVR("Implicit");
	ElVal->SetImplicitVr();
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a ImplicitVRLittleEndian one.
 *
 * @return  True when ImplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmHeader::IsImplicitVRLittleEndianTransferSyntax(void) {
	ElValue* Element = PubElVals.GetElementByNumber(0x0002, 0x0010);
	if ( !Element )
		return false;
	LoadElementValueSafe(Element);
	string Transfer = Element->GetValue();
	if ( Transfer == "1.2.840.10008.1.2" )
		return true;
	return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a ExplicitVRLittleEndian one.
 *
 * @return  True when ExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmHeader::IsExplicitVRLittleEndianTransferSyntax(void) {
	ElValue* Element = PubElVals.GetElementByNumber(0x0002, 0x0010);
	if ( !Element )
		return false;
	LoadElementValueSafe(Element);
	string Transfer = Element->GetValue();
	if ( Transfer == "1.2.840.10008.1.2.1" )
		return true;
	return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a DeflatedExplicitVRLittleEndian one.
 *
 * @return  True when DeflatedExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmHeader::IsDeflatedExplicitVRLittleEndianTransferSyntax(void) {
	ElValue* Element = PubElVals.GetElementByNumber(0x0002, 0x0010);
	if ( !Element )
		return false;
	LoadElementValueSafe(Element);
	string Transfer = Element->GetValue();
	if ( Transfer == "1.2.840.10008.1.2.1.99" )
		return true;
	return false;
}


/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a Explicit VR Big Endian one.
 *
 * @return  True when big endian found. False in all other cases.
 */
bool gdcmHeader::IsExplicitVRBigEndianTransferSyntax(void) {
	ElValue* Element = PubElVals.GetElementByNumber(0x0002, 0x0010);
	if ( !Element )
		return false;
	LoadElementValueSafe(Element);
	string Transfer = Element->GetValue();
	if ( Transfer == "1.2.840.10008.1.2.2" )
		return true;
	return false;
}


/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a JPEGBaseLineProcess1 one.
 *
 * @return  True when JPEGBaseLineProcess1found. False in all other cases.
 */
bool gdcmHeader::IsJPEGBaseLineProcess1TransferSyntax(void) {
	ElValue* Element = PubElVals.GetElementByNumber(0x0002, 0x0010);
	if ( !Element )
		return false;
	LoadElementValueSafe(Element);
	string Transfer = Element->GetValue();
	if ( Transfer == "1.2.840.10008.1.2.4.50" )
		return true;
	return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a JPEGExtendedProcess2-4 one.
 *
 * @return  True when JPEGExtendedProcess2-4 found. False in all other cases.
 */
bool gdcmHeader::IsJPEGExtendedProcess2_4TransferSyntax(void) {
	ElValue* Element = PubElVals.GetElementByNumber(0x0002, 0x0010);
	if ( !Element )
		return false;
	LoadElementValueSafe(Element);
	string Transfer = Element->GetValue();
	if ( Transfer == "1.2.840.10008.1.2.4.51" )
		return true;
	return false;
}


/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a JPEGExtendeProcess3-5 one.
 *
 * @return  True when JPEGExtendedProcess3-5 found. False in all other cases.
 */
bool gdcmHeader::IsJPEGExtendedProcess3_5TransferSyntax(void) {
	ElValue* Element = PubElVals.GetElementByNumber(0x0002, 0x0010);
	if ( !Element )
		return false;
	LoadElementValueSafe(Element);
	string Transfer = Element->GetValue();
	if ( Transfer == "1.2.840.10008.1.2.4.52" )
		return true;
	return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a JPEGSpectralSelectionProcess6-8 one.
 *
 * @return  True when JPEGSpectralSelectionProcess6-8 found. False in all other cases.
 */
bool gdcmHeader::IsJPEGSpectralSelectionProcess6_8TransferSyntax(void) {
	ElValue* Element = PubElVals.GetElementByNumber(0x0002, 0x0010);
	if ( !Element )
		return false;
	LoadElementValueSafe(Element);
	string Transfer = Element->GetValue();
	if ( Transfer == "1.2.840.10008.1.2.4.53" )
		return true;
	return false;
}

//
// Euhhhhhhh
// Il y en a encore DIX-SEPT, comme ça.
// Il faudrait trouver qq chose + rusé ...
//


void gdcmHeader::FixFoundLength(ElValue * ElVal, guint32 FoundLength) {
	// Heuristic: a final fix.
	if ( FoundLength == 0xffffffff)
		FoundLength = 0;
	ElVal->SetLength(FoundLength);
}

guint32 gdcmHeader::FindLengthOB(void) {
	// See PS 3.5-2001, section A.4 p. 49 on encapsulation of encoded pixel data.
	guint16 g;
	guint16 n; 
	long PositionOnEntry = ftell(fp);
	bool FoundSequenceDelimiter = false;
	guint32 TotalLength = 0;
	guint32 ItemLength;

	while ( ! FoundSequenceDelimiter) {
		g = ReadInt16();
		n = ReadInt16();
		TotalLength += 4;  // We even have to decount the group and element 
		if ( g != 0xfffe ) {
			dbg.Verbose(1, "gdcmHeader::FindLengthOB: ",
			            "wrong group for an item sequence.");
			throw Error::FileReadError(fp, "gdcmHeader::FindLengthOB");
		}
		if ( n == 0xe0dd )
			FoundSequenceDelimiter = true;
		else if ( n != 0xe000) {
			dbg.Verbose(1, "gdcmHeader::FindLengthOB: ",
			            "wrong element for an item sequence.");
			throw Error::FileReadError(fp, "gdcmHeader::FindLengthOB");
		}
		ItemLength = ReadInt32();
		TotalLength += ItemLength + 4;  // We add 4 bytes since we just read
		                                // the ItemLength with ReadInt32
		SkipBytes(ItemLength);
	}
	fseek(fp, PositionOnEntry, SEEK_SET);
	return TotalLength;
}

void gdcmHeader::FindLength(ElValue * ElVal) {
	guint16 element = ElVal->GetElement();
	string  vr      = ElVal->GetVR();
	guint16 length16;
	
	if ( (filetype == ExplicitVR) && ! ElVal->IsImplicitVr() ) {

		if ( (vr=="OB") || (vr=="OW") || (vr=="SQ") || (vr=="UN") ) {
			// The following reserved two bytes (see PS 3.5-2001, section
			// 7.1.2 Data element structure with explicit vr p27) must be
			// skipped before proceeding on reading the length on 4 bytes.
			fseek(fp, 2L, SEEK_CUR);
			guint32 length32 = ReadInt32();
			if ( (vr == "OB") && (length32 == 0xffffffff) ) {
				ElVal->SetLength(FindLengthOB());
				return;
			}
			FixFoundLength(ElVal, length32);
			return;
		}

		// Length is encoded on 2 bytes.
		length16 = ReadInt16();
		
		// We can tell the current file is encoded in big endian (like
		// Data/US-RGB-8-epicard) when we find the "Transfer Syntax" tag
		// and it's value is the one of the encoding of a big endian file.
		// In order to deal with such big endian encoded files, we have
		// (at least) two strategies:
		// * when we load the "Transfer Syntax" tag with value of big endian
		//   encoding, we raise the proper flags. Then we wait for the end
		//   of the META group (0x0002) among which is "Transfer Syntax",
		//   before switching the swap code to big endian. We have to postpone
		//   the switching of the swap code since the META group is fully encoded
		//   in little endian, and big endian coding only starts at the next
		//   group. The corresponding code can be hard to analyse and adds
		//   many additional unnecessary tests for regular tags.
		// * the second strategy consists in waiting for trouble, that shall appear
		//   when we find the first group with big endian encoding. This is
		//   easy to detect since the length of a "Group Length" tag (the
		//   ones with zero as element number) has to be of 4 (0x0004). When we
		//   encouter 1024 (0x0400) chances are the encoding changed and we
		//   found a group with big endian encoding.
		// We shall use this second strategy. In order make sure that we
		// can interpret the presence of an apparently big endian encoded
		// length of a "Group Length" without committing a big mistake, we
		// add an additional check: we look in the allready parsed elements
		// for the presence of a "Transfer Syntax" whose value has to be "big
		// endian encoding". When this is the case, chances are we got our
		// hands on a big endian encoded file: we switch the swap code to
		// big endian and proceed...
		if ( (element  == 0x000) && (length16 == 0x0400) ) {
			if ( ! IsExplicitVRBigEndianTransferSyntax() )
				throw Error::FileReadError(fp, "gdcmHeader::FindLength");
			length16 = 4;
			SwitchSwapToBigEndian();
			// Restore the unproperly loaded values i.e. the group, the element
			// and the dictionary entry depending on them.
			guint16 CorrectGroup   = SwapShort(ElVal->GetGroup());
			guint16 CorrectElem    = SwapShort(ElVal->GetElement());
			gdcmDictEntry * NewTag = IsInDicts(CorrectGroup, CorrectElem);
			if (!NewTag) {
				// This correct tag is not in the dictionary. Create a new one.
				NewTag = new gdcmDictEntry(CorrectGroup, CorrectElem);
			}
			// FIXME this can create a memory leaks on the old entry that be
			// left unreferenced.
			ElVal->SetDictEntry(NewTag);
		}
		 
		// Heuristic: well some files are really ill-formed.
		if ( length16 == 0xffff) {
			length16 = 0;
			dbg.Verbose(0, "gdcmHeader::FindLength",
			            "Erroneous element length fixed.");
		}
		FixFoundLength(ElVal, (guint32)length16);
		return;
	}

	// Either implicit VR or a non DICOM conformal (see not below) explicit
	// VR that ommited the VR of (at least) this element. Farts happen.
	// [Note: according to the part 5, PS 3.5-2001, section 7.1 p25
	// on Data elements "Implicit and Explicit VR Data Elements shall
	// not coexist in a Data Set and Data Sets nested within it".]
	// Length is on 4 bytes.
	FixFoundLength(ElVal, ReadInt32());
}

/**
 * \ingroup gdcmHeader
 * \brief   Swaps back the bytes of 4-byte long integer accordingly to
 *          processor order.
 *
 * @return  The suggested integer.
 */
guint32 gdcmHeader::SwapLong(guint32 a) {
	// FIXME: il pourrait y avoir un pb pour les entiers negatifs ...
	switch (sw) {
	case    0 :
		break;
	case 4321 :
		a=(   ((a<<24) & 0xff000000) | ((a<<8)  & 0x00ff0000)    | 
		      ((a>>8)  & 0x0000ff00) | ((a>>24) & 0x000000ff) );
		break;
	
	case 3412 :
		a=(   ((a<<16) & 0xffff0000) | ((a>>16) & 0x0000ffff) );
		break;
	
	case 2143 :
		a=(    ((a<<8) & 0xff00ff00) | ((a>>8) & 0x00ff00ff)  );
		break;
	default :
		dbg.Error(" gdcmHeader::SwapLong : unset swap code");
		a=0;
	}
	return(a);
}

/**
 * \ingroup gdcmHeader
 * \brief   Swaps the bytes so they agree with the processor order
 * @return  The properly swaped 16 bits integer.
 */
guint16 gdcmHeader::SwapShort(guint16 a) {
	if ( (sw==4321)  || (sw==2143) )
		a =(((a<<8) & 0x0ff00) | ((a>>8)&0x00ff));
	return (a);
}

void gdcmHeader::SkipBytes(guint32 NBytes) {
	//FIXME don't dump the returned value
	(void)fseek(fp, (long)NBytes, SEEK_CUR);
}

void gdcmHeader::SkipElementValue(ElValue * ElVal) {
	SkipBytes(ElVal->GetLength());
}

void gdcmHeader::SetMaxSizeLoadElementValue(long NewSize) {
	if (NewSize < 0)
		return;
	if ((guint32)NewSize >= (guint32)0xffffffff) {
		MaxSizeLoadElementValue = 0xffffffff;
		return;
	}
	MaxSizeLoadElementValue = NewSize;
}

/**
 * \ingroup       gdcmHeader
 * \brief         Loads the element if it's size is not to big.
 * @param ElVal   Element whose value shall be loaded. 
 * @param MaxSize Size treshold above which the element value is not
 *                loaded in memory. The element value is allways loaded
 *                when MaxSize is equal to UINT32_MAX.
 * @return  
 */
void gdcmHeader::LoadElementValue(ElValue * ElVal) {
	size_t item_read;
	guint16 group  = ElVal->GetGroup();
	guint16 elem   = ElVal->GetElement();
	string  vr     = ElVal->GetVR();
	guint32 length = ElVal->GetLength();
	bool SkipLoad  = false;

	fseek(fp, (long)ElVal->GetOffset(), SEEK_SET);
	
	// Sequences not treated yet !
	//
	// Ne faudrait-il pas au contraire trouver immediatement
	// une maniere 'propre' de traiter les sequences (vr = SQ)
	// car commencer par les ignorer risque de conduire a qq chose
	// qui pourrait ne pas etre generalisable
	//
	if( vr == "SQ" )
		SkipLoad = true;

	// Heuristic : a sequence "contains" a set of tags (called items). It looks
	// like the last tag of a sequence (the one that terminates the sequence)
	// has a group of 0xfffe (with a dummy length).
	if( group == 0xfffe )
		SkipLoad = true;

	// The group length doesn't represent data to be loaded in memory, since
	// each element of the group shall be loaded individualy.
	if( elem == 0 )
 		SkipLoad = true;

	if ( SkipLoad ) {
			  // FIXME the following skip is not necessary
		SkipElementValue(ElVal);
		ElVal->SetLength(0);
		ElVal->SetValue("gdcm::Skipped");
		return;
	}

	// When the length is zero things are easy:
	if ( length == 0 ) {
		ElVal->SetValue("");
		return;
	}

	// Values bigger than specified are not loaded.
	//
	// En fait, c'est les elements dont la longueur est superieure 
	// a celle fixee qui ne sont pas charges
	//
	if (length > MaxSizeLoadElementValue) {
		ostringstream s;
		s << "gdcm::NotLoaded.";
		s << " Address:" << (long)ElVal->GetOffset();
		s << " Length:"  << ElVal->GetLength();
		//mesg += " Length:"  + ElVal->GetLength();
		ElVal->SetValue(s.str());
		return;
	}
	
	// When an integer is expected, read and convert the following two or
	// four bytes properly i.e. as an integer as opposed to a string.
	if ( IsAnInteger(ElVal) ) {
		guint32 NewInt;
		if( length == 2 ) {
			NewInt = ReadInt16();
		} else if( length == 4 ) {
			NewInt = ReadInt32();
		} else
			dbg.Error(true, "LoadElementValue: Inconsistency when reading Int.");
		
		//FIXME: make the following an util fonction
		ostringstream s;
		s << NewInt;
		ElVal->SetValue(s.str());
		return;
	}
	
	// FIXME The exact size should be length if we move to strings or whatever
	
	//
	// QUESTION : y a-t-il une raison pour ne pas utiliser g_malloc ici ?
	//
	
	char* NewValue = (char*)malloc(length+1);
	if( !NewValue) {
		dbg.Verbose(1, "LoadElementValue: Failed to allocate NewValue");
		return;
	}
	NewValue[length]= 0;
	
	item_read = fread(NewValue, (size_t)length, (size_t)1, fp);
	if ( item_read != 1 ) {
		free(NewValue);
		Error::FileReadError(fp, "gdcmHeader::LoadElementValue");
		ElVal->SetValue("gdcm::UnRead");
		return;
	}
	ElVal->SetValue(NewValue);
}

/**
 * \ingroup       gdcmHeader
 * \brief         Loads the element while preserving the current
 *                underlying file position indicator as opposed to
 *                to LoadElementValue that modifies it.
 * @param ElVal   Element whose value shall be loaded. 
 * @return  
 */
void gdcmHeader::LoadElementValueSafe(ElValue * ElVal) {
	long PositionOnEntry = ftell(fp);
	LoadElementValue(ElVal);
	fseek(fp, PositionOnEntry, SEEK_SET);
}


guint16 gdcmHeader::ReadInt16(void) {
	guint16 g;
	size_t item_read;
	item_read = fread (&g, (size_t)2,(size_t)1, fp);
	if ( item_read != 1 )
		throw Error::FileReadError(fp, "gdcmHeader::ReadInt16");
	g = SwapShort(g);
	return g;
}

guint32 gdcmHeader::ReadInt32(void) {
	guint32 g;
	size_t item_read;
	item_read = fread (&g, (size_t)4,(size_t)1, fp);
	if ( item_read != 1 )
		throw Error::FileReadError(fp, "gdcmHeader::ReadInt32");
	g = SwapLong(g);
	return g;
}

/**
 * \ingroup gdcmHeader
 * \brief   Read the next tag without loading it's value
 * @return  On succes the newly created ElValue, NULL on failure.      
 */

ElValue * gdcmHeader::ReadNextElement(void) {
	guint16 g;
	guint16 n;
	ElValue * NewElVal;
	
	try {
		g = ReadInt16();
		n = ReadInt16();
	}
	catch ( Error::FileReadError ) {
		// We reached the EOF (or an error occured) and header parsing
		// has to be considered as finished.
		return (ElValue *)0;
	}

	// Find out if the tag we encountered is in the dictionaries:
	gdcmDictEntry * NewTag = IsInDicts(g, n);
	if (!NewTag)
		NewTag = new gdcmDictEntry(g, n);

	NewElVal = new ElValue(NewTag);
	if (!NewElVal) {
		dbg.Verbose(1, "ReadNextElement: failed to allocate ElValue");
		return (ElValue*)0;
	}

	FindVR(NewElVal);
	try { FindLength(NewElVal); }
	catch ( Error::FileReadError ) { // Call it quits
		return (ElValue *)0;
	}
	NewElVal->SetOffset(ftell(fp));
	return NewElVal;
}

bool gdcmHeader::IsAnInteger(ElValue * ElVal) {
	guint16 group   = ElVal->GetGroup();
	guint16 element = ElVal->GetElement();
	string  vr      = ElVal->GetVR();
	guint32 length  = ElVal->GetLength();

	// When we have some semantics on the element we just read, and if we
	// a priori know we are dealing with an integer, then we shall be
	// able to swap it's element value properly.
	if ( element == 0 )  {  // This is the group length of the group
		if (length == 4)
			return true;
		else
			dbg.Error("gdcmHeader::IsAnInteger",
			          "Erroneous Group Length element length.");
	}
	
	if ( group % 2 != 0 )
		// We only have some semantics on documented elements, which are
		// the even ones.
		return false;
	
	if ( (length != 4) && ( length != 2) )
		// Swapping only make sense on integers which are 2 or 4 bytes long.
		return false;
	
	if ( (vr == "UL") || (vr == "US") || (vr == "SL") || (vr == "SS") )
		return true;
	
	if ( (group == 0x0028) && (element == 0x0005) )
		// This tag is retained from ACR/NEMA
		// CHECKME Why should "Image Dimensions" be a single integer ?
		//
		// "Image Dimensions", c'est en fait le 'nombre de dimensions'
		// de l'objet ACR-NEMA stocké
		// 1 : Signal
		// 2 : Image
		// 3 : Volume
		// 4 : Sequence
		//
		// DICOM V3 ne retient pas cette information
		// Par defaut, tout est 'Image',
		// C'est a l'utilisateur d'explorer l'ensemble des entetes
		// pour savoir à quoi il a a faire
		//
		// Le Dicom Multiframe peut etre utilise pour stocker,
		// dans un seul fichier, une serie temporelle (cardio vasculaire GE, p.ex)
		// ou un volume (medecine Nucleaire, p.ex)
		//
		return true;
	
	if ( (group == 0x0028) && (element == 0x0200) )
		// This tag is retained from ACR/NEMA
		return true;
	
	return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Recover the offset (from the beginning of the file) of the pixels.
 */
size_t gdcmHeader::GetPixelOffset(void) {
	// If this file complies with the norm we should encounter the
	// "Image Location" tag (0x0028,  0x0200). This tag contains the
	// the group that contains the pixel data (hence the "Pixel Data"
	// is found by indirection through the "Image Location").
	// Inside the group pointed by "Image Location" the searched element
	// is conventionally the element 0x0010 (when the norm is respected).
	//    When the "Image Location" is absent we default to group 0x7fe0.
	guint16 grPixel;
	guint16 numPixel;
	string ImageLocation = GetPubElValByName("Image Location");
	if ( ImageLocation == "gdcm::Unfound" ) {
		grPixel = 0x7fe0;
	} else {
		grPixel = (guint16) atoi( ImageLocation.c_str() );
	}
	if (grPixel != 0x7fe0)
		// FIXME is this still necessary ?
		// Now, this looks like an old dirty fix for Philips imager
		numPixel = 0x1010;
	else
		numPixel = 0x0010;
	ElValue* PixelElement = PubElVals.GetElementByNumber(grPixel, numPixel);
	if (PixelElement)
		return PixelElement->GetOffset();
	else
		return 0;
}

gdcmDictEntry * gdcmHeader::IsInDicts(guint32 group, guint32 element) {
	//
	// Y a-t-il une raison de lui passer des guint32
	// alors que group et element sont des guint16?
	//
	gdcmDictEntry * found = (gdcmDictEntry*)0;
	if (!RefPubDict && !RefShaDict) {
		//FIXME build a default dictionary !
		printf("FIXME in gdcmHeader::IsInDicts\n");
	}
	if (RefPubDict) {
		found = RefPubDict->GetTag(group, element);
		if (found)
			return found;
	}
	if (RefShaDict) {
		found = RefShaDict->GetTag(group, element);
		if (found)
			return found;
	}
	return found;
}

list<string> * gdcmHeader::GetPubTagNames(void) {
	list<string> * Result = new list<string>;
	TagHT entries = RefPubDict->GetEntries();

	for (TagHT::iterator tag = entries.begin(); tag != entries.end(); ++tag){
      Result->push_back( tag->second->GetName() );
	}
	return Result;
}

map<string, list<string> > * gdcmHeader::GetPubTagNamesByCategory(void) {
	map<string, list<string> > * Result = new map<string, list<string> >;
	TagHT entries = RefPubDict->GetEntries();

	for (TagHT::iterator tag = entries.begin(); tag != entries.end(); ++tag){
		(*Result)[tag->second->GetFourth()].push_back(tag->second->GetName());
	}
	return Result;
}

string gdcmHeader::GetPubElValByNumber(guint16 group, guint16 element) {
	return PubElVals.GetElValueByNumber(group, element);
}

string gdcmHeader::GetPubElValRepByNumber(guint16 group, guint16 element) {
	ElValue* elem =  PubElVals.GetElementByNumber(group, element);
	if ( !elem )
		return "gdcm::Unfound";
	return elem->GetVR();
}

string gdcmHeader::GetPubElValByName(string TagName) {
	return PubElVals.GetElValueByName(TagName);
}

string gdcmHeader::GetPubElValRepByName(string TagName) {
	ElValue* elem =  PubElVals.GetElementByName(TagName);
	if ( !elem )
		return "gdcm::Unfound";
	return elem->GetVR();
}

string gdcmHeader::GetShaElValByNumber(guint16 group, guint16 element) {
	return ShaElVals.GetElValueByNumber(group, element);
}

string gdcmHeader::GetShaElValRepByNumber(guint16 group, guint16 element) {
	ElValue* elem =  ShaElVals.GetElementByNumber(group, element);
	if ( !elem )
		return "gdcm::Unfound";
	return elem->GetVR();
}

string gdcmHeader::GetShaElValByName(string TagName) {
	return ShaElVals.GetElValueByName(TagName);
}

string gdcmHeader::GetShaElValRepByName(string TagName) {
	ElValue* elem =  ShaElVals.GetElementByName(TagName);
	if ( !elem )
		return "gdcm::Unfound";
	return elem->GetVR();
}


string gdcmHeader::GetElValByNumber(guint16 group, guint16 element) {
	string pub = GetPubElValByNumber(group, element);
	if (pub.length())
		return pub;
	return GetShaElValByNumber(group, element);
}

string gdcmHeader::GetElValRepByNumber(guint16 group, guint16 element) {
	string pub = GetPubElValRepByNumber(group, element);
	if (pub.length())
		return pub;
	return GetShaElValRepByNumber(group, element);
}

string gdcmHeader::GetElValByName(string TagName) {
	string pub = GetPubElValByName(TagName);
	if (pub.length())
		return pub;
	return GetShaElValByName(TagName);
}

string gdcmHeader::GetElValRepByName(string TagName) {
	string pub = GetPubElValRepByName(TagName);
	if (pub.length())
		return pub;
	return GetShaElValRepByName(TagName);
}

/**
 * \ingroup gdcmHeader
 * \brief   Modifie la valeur d'un ElValue déja existant
 * \	dans le PubElVals du gdcmHeader,
 * \	accédé par ses numero de groupe et d'element.
 */
int gdcmHeader::SetPubElValByNumber(string content, guint16 group, guint16 element) {
	//TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	//PubElVals.tagHt[key]->SetValue(content);
	
	return (  PubElVals.SetElValueByNumber (content, group, element) );
}


/**
 * \ingroup gdcmHeader
 * \brief   Modifie la valeur d'un ElValue déja existant
 * \	dans le PubElVals du gdcmHeader,
 * \	accédé par son nom
 */
int gdcmHeader::SetPubElValByName(string content, string TagName) {
	//TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	//PubElVals.tagHt[key]->SetValue(content);
	
	return (  PubElVals.SetElValueByName (content, TagName) );
}


/**
 * \ingroup gdcmHeader
 * \brief   Modifie la valeur d'un ElValue déja existant
 * \	dans le ShaElVals du gdcmHeader,
 * \	accédé par ses numero de groupe et d'element.
 */
int gdcmHeader::SetShaElValByNumber(string content, guint16 group, guint16 element) {
	
	return (  ShaElVals.SetElValueByNumber (content, group, element) );
}


/**
 * \ingroup gdcmHeader
 * \brief   Modifie la valeur d'un ElValue déja existant
 * \	dans le ShaElVals du gdcmHeader,
 * \	accédé par son nom
 */
int gdcmHeader::SetShaElValByName(string content, string TagName) {
	
	return (  ShaElVals.SetElValueByName (content, TagName) );
}
/**
 * \ingroup gdcmHeader
 * \brief   Parses the header of the file but does NOT load element values.
 */
void gdcmHeader::ParseHeader(void) {
	ElValue * newElValue = (ElValue *)0;
	
	rewind(fp);
	CheckSwap();
	while ( (newElValue = ReadNextElement()) ) {
		SkipElementValue(newElValue);
		PubElVals.Add(newElValue);
	}
}

/**
 * \ingroup gdcmHeader
 * \brief   Loads the element values of all the elements present in the
 *          public tag based hash table.
 */
void gdcmHeader::LoadElements(void) {

	if (DEBUG) printf("LoadElements : Entree\n");

	rewind(fp);   
 	if (DEBUG) printf("LoadElements : rewind\n");

	TagElValueHT ht = PubElVals.GetTagHt();
	
	if (DEBUG) printf("LoadElements : GetTagHt\n");

	for (TagElValueHT::iterator tag = ht.begin(); tag != ht.end(); ++tag) {
		LoadElementValue(tag->second);
		}
}

void gdcmHeader::PrintPubElVal(ostream & os) {
	PubElVals.Print(os);
}

void gdcmHeader::PrintPubDict(ostream & os) {
	RefPubDict->Print(os);
}
