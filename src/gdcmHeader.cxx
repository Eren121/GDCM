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

//FIXME: this looks dirty to me...
#define str2num(str, typeNum) *((typeNum *)(str))

VRHT * gdcmHeader::dicom_vr = (VRHT*)0;
gdcmDictSet* gdcmHeader::Dicts = new gdcmDictSet();

void gdcmHeader::Initialise(void) {
	if (!gdcmHeader::dicom_vr)
		InitVRDict();
	PixelPosition = (size_t)0;
	PixelsTrouves = false;
	RefPubDict = gdcmHeader::Dicts->GetDefaultPublicDict();
	RefShaDict = (gdcmDict*)0;
}

gdcmHeader::gdcmHeader (char* InFilename) {
	filename = InFilename;
	Initialise();
	fp=fopen(InFilename,"rw");
	dbg.Error(!fp, "gdcmHeader::gdcmHeader cannot open file", InFilename);
	BuildHeader();
	fclose(fp);
}

gdcmHeader::~gdcmHeader (void) {
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

void gdcmHeader::FindVR( ElValue *pleCourant) {
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
		pleCourant->SetVR(FoundVR);
		return; 
	}
	
	// We thought this was explicit VR, but we end up with an
	// implicit VR tag. Let's backtrack.
	pleCourant->SetVR("Implicit");
	fseek(fp, PositionOnEntry, SEEK_SET);
}

void gdcmHeader::FindLength( ElValue *pleCourant) {
	int lgrLue;
	guint32 l_gr;
	unsigned short int l_gr_2;
	
	string vr = pleCourant->GetVR();
	
	if ( (filetype == ExplicitVR) && (vr != "Implicit") ) {
		if (   ( vr == "OB" ) || ( vr == "OW" )
			 || ( vr == "SQ" ) || ( vr == "UN" ) ) {
			
			// The following two bytes are reserved, so we skip them,
			// and we proceed on reading the length on 4 bytes.
			fseek(fp, 2L,SEEK_CUR);
			lgrLue=fread (&l_gr, (size_t)4,(size_t)1, fp);
			l_gr = SwapLong((guint32)l_gr);
			
		} else {
			// Length is encoded on 2 bytes.
			lgrLue=fread (&l_gr_2, (size_t)2,(size_t)1, fp);
			
			l_gr_2 = SwapShort((unsigned short)l_gr_2);
			
			if ( l_gr_2 == 0xffff) {
				l_gr = 0;
			} else {
				l_gr = l_gr_2;
			}
		}
	} else {
		// Either implicit VR or an explicit VR that (at least for this
		// element) lied a little bit. Length is on 4 bytes.
		lgrLue=fread (&l_gr, (size_t)4,(size_t)1, fp);
		l_gr= SwapLong((long)l_gr);
	}

	// Traitement des curiosites sur la longueur
	if ( l_gr == 0xffffffff)
		l_gr=0; 
	
	pleCourant->SetLength(l_gr);
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
short int gdcmHeader::SwapShort(short int a) {
	if ( (sw==4321)  || (sw==2143) )
		a =(((a<<8) & 0x0ff00) | ((a>>8)&0x00ff));
	return (a);
}

/**
 * \ingroup       gdcmHeader
 * \brief         lit le dicom_element suivant.
 *			(le fichier doit deja avoir ete ouvert,
 *			 _IdAcrCheckSwap(ID_DCM_HDR *e) avoir ete appele)
 * @param e      ID_DCM_HDR  dans lequel effectuer la recherche.
 * @param sw    	code swap.
 * @return        	En cas de succes, 1 
 *               	0 en cas d'echec.
 */

ElValue * gdcmHeader::ReadNextElement(void) {
	guint16 g;
	guint16 n;
	guint32 l;
	size_t lgrLue;
	ElValue * nouvDcmElem;
	
	// ------------------------- Lecture Num group : g
	lgrLue=fread (&g, (size_t)2,(size_t)1, fp);
	
	if (feof(fp))  {
		dbg.Verbose(1, "ReadNextElement: EOF encountered");
		return (NULL);
	}
	if (ferror(fp)){
		dbg.Verbose(1, "ReadNextElement: failed to read NumGr");
		return (NULL);
	}
	
	if (sw) g= SwapShort(((short)g));
	
	// ------------------------- Lecture Num Elem : n
	lgrLue=fread (&n, (size_t)2,(size_t)1, fp);
	
	if (feof(fp))  {
		dbg.Verbose(1, "ReadNextElement: EOF encountered");
		return (NULL);
	}
	if (ferror(fp)){
		dbg.Verbose(1, "ReadNextElement: failed to read NumElem");
		return (NULL);
	}
	
	if(sw) n= SwapShort(((short)n));

	// Find out if the tag we encountered is in the dictionaries:
	gdcmDictEntry * NewTag = IsInDicts(g, n);
	if (!NewTag)
		NewTag = new gdcmDictEntry(g, n, "Unknown", "Unknown", "Unkown");

	nouvDcmElem = new ElValue(NewTag);
	if (!nouvDcmElem) {
		dbg.Verbose(1, "ReadNextElement: failed to allocate ElValue");
		return(NULL);
	}

	FindVR(nouvDcmElem);
	FindLength(nouvDcmElem);
	nouvDcmElem->SetOffset(ftell(fp));
	l = nouvDcmElem->GetLength();

	//FIXMEif(!memcmp( VR,"SQ",(size_t)2 )) { // ca annonce une SEQUENCE d'items ?!
	//FIXME	l_gr=0;                         // on lira donc les items de la sequence 
	//FIXME}
	//FIXMEreturn(l_gr);

   // Une sequence contient un ensemble de group element repetes n fois
	// et g=fffe indique la fin (contient une longueur bidon).
	if(g==0xfffe) l=0;  // pour sauter les indicateurs de 'SQ'
	
	
	// ------------------------- Lecture Valeur element 
	
	// FIXME The exact size should be l if we move to strings or whatever
	// CLEAN ME NEWValue used to be nouvDcmElem->valeurElem
	char* NewValue = (char*)g_malloc(l+1);
	if(NewValue) {
		NewValue[l]= 0;
	} else {
		return (NULL);
	}
	
	// FIXME les elements trop long (seuil a fixer a la main) ne devraient
	// pas etre charge's !!!! Voir TODO.
	lgrLue=fread (NewValue, (size_t)l,(size_t)1, fp);
	
	if ( IsAnInteger(g, n, NewTag->GetVR(), l) ) {
		// CLEANME THe following is really UGLY ! 
		if( l == 4 ) {
			*(guint32 *) NewValue = SwapLong  ((*(guint32 *) NewValue)); 
		} else {
			if( l == 2 )
				*(guint16 *) NewValue = SwapShort ((*(guint16 *)NewValue));
		}
		//FIXME: don't we have to distinguish guin16 and guint32
		//FIXME: make the following an util fonction
		ostringstream s;
		s << *(guint32 *) NewValue;
		nouvDcmElem->value = s.str();
		g_free(NewValue);
	} else
		nouvDcmElem->value = NewValue;
	return nouvDcmElem;
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
 * \brief   If we encountered the offset of the pixels in the file
 *          (Pixel Data) then keep the info aside.
 */
void gdcmHeader::SetAsidePixelData(ElValue* elem) {
	// They are two cases :
	// * the pixel data (i.e. the image or the volume) is pointed by it's
	//   default official tag (0x7fe0,0x0010),
	// * the writer of this file decided to put the image "address" (i.e the
	//   offset from the begining of the file) at a different tag.
	//   Then the "Pixel Data" offset might be found by indirection through
	//   the "Image Location" tag (0x0028,  0x0200). In other terms the Image
	//   Location tag contains the group where the "Pixel Data" offset is and
	//   inside this group the element is conventionally at element 0x0010
	//   (when the norm is respected).
	// 
	// Hence getting our hands on the Pixel Data is a two stage process:
	//  1/ * find if the "Pixel Data" tag exists.
	//     * if it does not exist, look for the "Pixel Location" tag.
	//  2/ look at the proper tag ("Pixel Data" or "Pixel Location" when
	//     it exists) what the offset is.
	cout << "aaaaaaaaaaaaaaaaaaaaa";
	// PubElVals.PrintByName(cout);
	ostringstream val;
	val << hex << GetPubElValByName("Image Location");
	cout << GetPubElValByName("Image Location") << endl;
	cout <<hex << GetPubElValByName("Image Location") << dec << endl;
	cout << "aaaa" << hex << val << dec << endl;
	if (val)
		//grPixel  = val.hex().str();
		grPixel = 0;
	else
		grPixel  = 0x7FE0;
	return;

	guint16 g;
	guint16 n;
	g = elem->GetGroup();
	n = elem->GetElement();
	if (!grPixelTrouve) {   // on n a pas encore trouve les pixels
		if (g > 0x0028) {
			if (n > 0x0200 || g == 0x7FE0 ) {  // on a depasse (28,200)
				grPixel  = 0x7FE0;
				numPixel = 0x0010;
				grPixelTrouve = true;
			}
		} else {         // on est sur (28,200)
			if (g == 0x0028) {
				if (n == 0x0200) {
					grPixelTrouve = 1;
					char* NewValue = (char*)g_malloc(elem->GetLength()+1);
					// FIXME: not very elegant conversion
					for(int i=0;i<4;i++)
						*((char*)(&grPixel)+i) = *(NewValue+i); 
					elem->SetValue(NewValue);
					
					if (grPixel != 0x7FE0)   // Vieux pb Philips
						numPixel = 0x1010;    // encore utile ??
					else
						numPixel = 0x0010;
				}
			}
		}
	} else {     // on vient de trouver les pixels
		if (g == grPixel) {
			if (n == numPixel) {
				PixelPosition = elem->Offset; 
				PixelsTrouves = true;
				dbg.Verbose(0, "gdcmHeader::SetAsidePixelData:",
				            "Pixel data found");
			}
		}
	}
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
 * \ingroup       gdcmHeader
 * \brief         renvoie un pointeur sur le ID_DCM_HDR correspondant au fichier
 * @param filename      Nom du fichier ACR / LibIDO / DICOM
 * @return       le ID_DCM_HDR 
 */
 
void gdcmHeader::BuildHeader(void) {
	ElValue * newElValue = (ElValue *)0;
	
	rewind(fp);
	CheckSwap();
	while ( (newElValue = ReadNextElement()) ) {
		PubElVals.Add(newElValue);
	}
	SetAsidePixelData((ElValue*)0);
}

void gdcmHeader::PrintPubElVal(ostream & os) {
	PubElVals.Print(os);
}

void gdcmHeader::PrintPubDict(ostream & os) {
	RefPubDict->Print(os);
}
