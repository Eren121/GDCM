#include "gdcmlib.h"
extern "C" {
#include "glib.h"
}
#include <stdio.h>
// For nthos:
#ifdef _MSC_VER
#include <winsock.h>
#else
#include <netinet/in.h>
#endif
#include <map>
#include <sstream>
#include "gdcmUtil.h"

#define HEADER_LENGHT_TO_READ 256 // on ne lit plus que le debut

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

VRHT * gdcmHeader::dicom_vr = (VRHT*)0;
gdcmDictSet* gdcmHeader::Dicts = new gdcmDictSet();

void gdcmHeader::Initialise(void) {
	if (!gdcmHeader::dicom_vr)
		InitVRDict();
	RefPubDict = gdcmHeader::Dicts->GetDefaultPublicDict();
	RefShaDict = (gdcmDict*)0;
}

gdcmHeader::gdcmHeader (char* InFilename) {
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
	(*vr)["AE"] = "Application Entity";       // 16 car max
	(*vr)["AS"] = "Age String";               // 4 car fixe
	(*vr)["AT"] = "Attribute Tag";            // 2 unsigned short int
	(*vr)["CS"] = "Code String";              // 16 car max
	(*vr)["DA"] = "Date";                     // 8 car fixe
	(*vr)["DS"] = "Decimal String";           // Decimal codé Binaire 16 max
	(*vr)["DT"] = "Date Time";                // 26 car max
	(*vr)["FL"] = "Floating Point Single";    // 4 octets IEEE 754:1985
	(*vr)["FD"] = "Floating Point Double";    // 8 octets IEEE 754:1985
	(*vr)["IS"] = "Integer String";           // en format externe 12 max
	(*vr)["LO"] = "Long String";              // 64 octets max
	(*vr)["LT"] = "Long Text";                // 10240 max
	(*vr)["OB"] = "Other Byte String";
	(*vr)["OW"] = "Other Word String";
	(*vr)["PN"] = "Person Name";
	(*vr)["SH"] = "Short String";             // 16 car max
	(*vr)["SL"] = "Signed Long";
	(*vr)["SQ"] = "Sequence of Items";        // Not Applicable
	(*vr)["SS"] = "Signed Short";             // 2 octets
	(*vr)["ST"] = "Short Text";               // 1024 car max
	(*vr)["TM"] = "Time";                     // 16 car max
	(*vr)["UI"] = "Unique Identifier";        // 64 car max
	(*vr)["UN"] = "Unknown";
	(*vr)["UT"] = "Unlimited Text";           //  2 puissance 32 -1 car max
	(*vr)["UL"] = "Unsigned Long ";           // 4 octets fixe
	(*vr)["US"] = "Unsigned Short ";          // 2 octets fixe
   dicom_vr = vr;	
}

/**
 * \ingroup gdcmHeader
 * \brief   La seule maniere sure que l'on aie pour determiner 
 *          si on est en   LITTLE_ENDIAN,       BIG-ENDIAN, 
 *          BAD-LITTLE-ENDIAN, BAD-BIG-ENDIAN
 *          est de trouver l'element qui donne la longueur d'un 'GROUP'
 *          (on sait que la longueur de cet element vaut 0x00000004)
 *          et de regarder comment cette longueur est codee en memoire  
 *          
 *          Le probleme vient de ce que parfois, il n'y en a pas ...
 *          
 *          On fait alors le pari qu'on a a faire a du LITTLE_ENDIAN propre.
 *          (Ce qui est la norme -pas respectee- depuis ACR-NEMA)
 *          Si ce n'est pas le cas, on ne peut rien faire.
 *
 *          (il faudrait avoir des fonctions auxquelles 
 *          on passe le code Swap en parametre, pour faire des essais 'manuels')
 */
void gdcmHeader::CheckSwap()
{
	guint32  s;
	guint32  x=4;  // x : pour ntohs
	bool net2host; // true when HostByteOrder is the same as NetworkByteOrder
	 
	int lgrLue;
	char * entCur;
	char deb[HEADER_LENGHT_TO_READ];
	 
	// First, compare HostByteOrder and NetworkByteOrder in order to
	// determine if we shall need to swap bytes (i.e. the Endian type).
	if (x==ntohs(x))
		net2host = true;
	else
		net2host = false;
	
	// The easiest case is the one of a DICOM header, since it possesses a
	// file preamble where it suffice to look for the sting "DICM".
	lgrLue = fread(deb, 1, HEADER_LENGHT_TO_READ, fp);
	
	entCur = deb + 128;
	if(memcmp(entCur, "DICM", (size_t)4) == 0) {
		filetype = TrueDicom;
		dbg.Verbose(0, "gdcmHeader::CheckSwap:", "looks like DICOM Version3");
	} else {
		filetype = Unknown;
		dbg.Verbose(0, "gdcmHeader::CheckSwap:", "not a DICOM Version3 file");
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
			dbg.Verbose(0, "gdcmHeader::CheckSwap:",
			            "explicit Value Representation");
		} else {
			filetype = ImplicitVR;
			dbg.Verbose(0, "gdcmHeader::CheckSwap:",
			            "not an explicit Value Representation");
		}

		if (net2host) {
			sw = 4321;
			dbg.Verbose(0, "gdcmHeader::CheckSwap:",
			               "HostByteOrder != NetworkByteOrder");
		} else {
			sw = 0;
			dbg.Verbose(0, "gdcmHeader::CheckSwap:",
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
	s = str2num(entCur, int);
	
	switch (s) {
	case 0x00040000 :
		sw=3412;
		filetype = ACR;
		return;
	case 0x04000000 :
		sw=4321;
		filetype = ACR;
		return;
	case 0x00000400 :
		sw=2143;
		filetype = ACR;
		return;
	case 0x00000004 :
		sw=0;
		filetype = ACR;
		return;
	default :
		dbg.Verbose(0, "gdcmHeader::CheckSwap:",
		               "ACE/NEMA unfound swap info (time to raise bets)");
	}

	// We are out of luck. It is not a DicomV3 nor a 'clean' ACR/NEMA file.
	// It is time for despaired wild guesses. So, let's assume this file
	// happens to be 'dirty' ACR/NEMA, i.e. the length of the group it
	// not present. Then the only info we have is the net2host one.
	//FIXME  Si c'est du RAW, ca degagera + tard
	
	if (! net2host )
		sw = 0;
	else
		sw = 4321;
	return;
}

/**
 * \ingroup   gdcmHeader
 * \brief     recupere la longueur d'un champ DICOM.
 *            Preconditions:
 *            1/ le fichier doit deja avoir ete ouvert,
 *            2/ CheckSwap() doit avoir ete appele
 *            3/ la  partie 'group'  ainsi que la  partie 'elem' 
 *               de l'acr_element doivent avoir ete lues.
 *
 *            ACR-NEMA : we allways get
 *                 GroupNumber   (2 Octets) 
 *                 ElementNumber (2 Octets) 
 *                 ElementSize   (4 Octets)
 *            DICOM en implicit Value Representation :
 *                 GroupNumber   (2 Octets) 
 *                 ElementNumber (2 Octets) 
 *                 ElementSize   (4 Octets)
 *
 *            DICOM en explicit Value Representation :
 *                 GroupNumber         (2 Octets) 
 *                 ElementNumber       (2 Octets) 
 *                 ValueRepresentation (2 Octets) 
 *                 ElementSize         (2 Octets)
 *
 *            ATTENTION : dans le cas ou ValueRepresentation = OB, OW, SQ, UN
 *                 GroupNumber         (2 Octets) 
 *                 ElementNumber       (2 Octets) 
 *                 ValueRepresentation (2 Octets)
 *                 zone reservee       (2 Octets) 
 *                 ElementSize         (4 Octets)
 *
 * @param sw  code swap
 * @param skippedLength  pointeur sur nombre d'octets que l'on a saute qd
 *                       la lecture est finie
 * @param longueurLue    pointeur sur longueur (en nombre d'octets) 
 *                       effectivement lue
 * @return               longueur retenue pour le champ 
 */

void gdcmHeader::FindVR( ElValue *ElVal) {
	char VR[3];
	int lgrLue;
	long PositionOnEntry = ftell(fp);
	
	if (filetype != ExplicitVR)
		return;

	lgrLue=fread (&VR, (size_t)2,(size_t)1, fp);
	VR[2]=0;
		
	// Warning: we believe this is explicit VR (Value Representation) because
	// we used a heuristic that found "UL" in the first tag. Alas this
	// doesn't guarantee that all the tags will be in explicit VR. In some
	// cases (see e-film filtered files) one finds implicit VR tags mixed
	// within an explicit VR file. Hence we make sure the present tag
	// is in explicit VR and try to fix things if it happens not to be
	// the case.

	// FIXME There should be only one occurence returned. Avoid the
	// first extraction by calling proper method.
	VRAtr FoundVR = dicom_vr->find(string(VR))->first;
	if ( ! FoundVR.empty()) {
		ElVal->SetVR(FoundVR);
		return; 
	}
	
	// We thought this was explicit VR, but we end up with an
	// implicit VR tag. Let's backtrack.
	ElVal->SetVR("Implicit");
	fseek(fp, PositionOnEntry, SEEK_SET);
}

void gdcmHeader::FindLength( ElValue * ElVal) {
	guint32 length32;
	guint16 length16;
	
	string vr = ElVal->GetVR();
	
	if ( (filetype == ExplicitVR) && (vr != "Implicit") ) {
		if (   ( vr == "OB" ) || ( vr == "OW" )
			 || ( vr == "SQ" ) || ( vr == "UN" ) ) {
			
			// The following two bytes are reserved, so we skip them,
			// and we proceed on reading the length on 4 bytes.
			fseek(fp, 2L,SEEK_CUR);
			length32 = ReadInt32();
			
		} else {
			// Length is encoded on 2 bytes.
			length16 = ReadInt16();
			 
			if ( length16 == 0xffff) {
				length32 = 0;
			} else {
				length32 = length16;
			}
		}
	} else {
		// Either implicit VR or an explicit VR that (at least for this
		// element) lied a little bit. Length is on 4 bytes.
		length32 = ReadInt32();
	}
	
	// Traitement des curiosites sur la longueur
	if ( length32 == 0xffffffff)
		length32=0;
	
	ElVal->SetLength(length32);
}


/**
 * \ingroup gdcmHeader
 * \brief   remet les octets dans un ordre compatible avec celui du processeur

 * @return  longueur retenue pour le champ 
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
 * \brief   remet les octets dans un ordre compatible avec celui du processeur

 * @return  longueur retenue pour le champ 
 */
guint16 gdcmHeader::SwapShort(guint16 a) {
	if ( (sw==4321)  || (sw==2143) )
		a =(((a<<8) & 0x0ff00) | ((a>>8)&0x00ff));
	return (a);
}

void gdcmHeader::SkipElementValue(ElValue * ElVal) {
	//FIXME don't dump the returned value
	(void)fseek(fp, (long)ElVal->GetLength(), SEEK_CUR);
}

void gdcmHeader::LoadElementValue(ElValue * ElVal) {
	size_t item_read;
	guint16 group  = ElVal->GetGroup();
	guint16 elem   = ElVal->GetElement();
	string  vr     = ElVal->GetVR();
	guint32 length = ElVal->GetLength();
	fseek(fp, (long)ElVal->GetOffset(), SEEK_SET);
	
	// Sequences not treated yet !
	if( vr == "SQ" ) {
		SkipElementValue(ElVal);
		ElVal->SetLength(0);
		return;
	}
	// A sequence "contains" a set of tags (called items). It looks like
	// the last tag of a sequence (the one that terminates the sequence)
	// has a group of 0xfffe (with a dummy length).
	if( group == 0xfffe) {
		SkipElementValue(ElVal);
		ElVal->SetLength(0);
		return;
	}
	
	if ( IsAnInteger(group, elem, vr, length) ) {
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
	char* NewValue = (char*)g_malloc(length+1);
	if( !NewValue) {
		dbg.Verbose(1, "LoadElementValue: Failed to allocate NewValue");
		return;
	}
	NewValue[length]= 0;
	
	// FIXME les elements trop long (seuil a fixer a la main) ne devraient
	// pas etre charge's !!!! Voir TODO.
	item_read = fread(NewValue, (size_t)length, (size_t)1, fp);
	if ( item_read != 1 ) {
		g_free(NewValue);
		Error::FileReadError(fp, "gdcmHeader::LoadElementValue");
		ElVal->SetValue("gdcm::UnRead");
		return;
	}
	ElVal->SetValue(NewValue);
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
		NewTag = new gdcmDictEntry(g, n, "Unknown", "Unknown", "Unkown");

	NewElVal = new ElValue(NewTag);
	if (!NewElVal) {
		dbg.Verbose(1, "ReadNextElement: failed to allocate ElValue");
		return (ElValue*)0;
	}

	FindVR(NewElVal);
	FindLength(NewElVal);
	NewElVal->SetOffset(ftell(fp));
	return NewElVal;
}

bool gdcmHeader::IsAnInteger(guint16 group, guint16 element,
	                             string vr, guint32 length ) {
	// When we have some semantics on the element we just read, and we
	// a priori now we are dealing with an integer, then we can swap it's
	// element value properly.
	if ( element == 0 )  {  // This is the group length of the group
		if (length != 4)
			dbg.Error("gdcmHeader::ShouldBeSwaped", "should be four");
		return true;
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
	if ( ImageLocation == "UNFOUND" ) {
		grPixel = 0x7FE0;
	} else {
		grPixel = (guint16) atoi( ImageLocation.c_str() );
	}
	if (grPixel != 0x7fe0)
		// FIXME is this still necessary ?
		// Now, this looks like an old dirty fix for Philips imager
		numPixel = 0x1010;
	else
		numPixel = 0x0010;
	ElValue* PixelElement = PubElVals.GetElement(grPixel, numPixel);
	if (PixelElement)
		return PixelElement->GetOffset();
	else
		return 0;
}

gdcmDictEntry * gdcmHeader::IsInDicts(guint32 group, guint32 element) {
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

string gdcmHeader::GetPubElValByNumber(guint16 group, guint16 element) {
	return PubElVals.GetElValue(group, element);
}

string gdcmHeader::GetPubElValByName(string TagName) {
	return PubElVals.GetElValue(TagName);
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
	rewind(fp);    
	TagElValueHT ht = PubElVals.GetTagHt();
	for (TagElValueHT::iterator tag = ht.begin(); tag != ht.end(); ++tag)
		LoadElementValue(tag->second);
}

void gdcmHeader::PrintPubElVal(ostream & os) {
	PubElVals.Print(os);
}

void gdcmHeader::PrintPubDict(ostream & os) {
	RefPubDict->Print(os);
}
