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

#define LGR_ENTETE_A_LIRE 256 // on ne lit plus que le debut

//FIXME: this looks dirty to me...
#define str2num(str, typeNum) *((typeNum *)(str))

VRHT * gdcmHeader::dicom_vr = (VRHT*)0;
gdcmDictSet* gdcmHeader::Dicts = new gdcmDictSet();

void gdcmHeader::Initialise(void) {
	if (!gdcmHeader::dicom_vr)
		InitVRDict();
	bool grPixelTrouve = false;
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
	char deb[LGR_ENTETE_A_LIRE];
	 
	// On teste le processeur
	if (x==ntohs(x)) {
		net2host = true;
	} else {
		net2host = false;
	}
	
	// On commence par verifier si c'est du DICOM 'actuel'
	//                                      -------------
	lgrLue = fread(deb,1,LGR_ENTETE_A_LIRE, fp);
	
	entCur = deb+128;
	if(memcmp(entCur, "DICM", (size_t)4) == 0) {
		filetype = TrueDicom;
		dbg.Verbose(0, "gdcmHeader::CheckSwap:", "looks like DICOM Version3");
	} else {
		filetype = Unknown;
		dbg.Verbose(0, "gdcmHeader::CheckSwap:", "not a DICOM Version3 file");
	}

	if(filetype == TrueDicom) {
		// on saute le File Preamble (souvent a ZERO) : 128 Octets
		// + le DICM (4), et le (0002, 0000) soit 4 (136 = 128 + 4 + 4)
		entCur = deb+136;
		if(memcmp(entCur, "UL", (size_t)2) == 0) {
			// les 2 premiers octets de la lgr peuvent valoir UL --> Explicit VR
			filetype = ExplicitVR;
			dbg.Verbose(0, "gdcmHeader::CheckSwap:",
			            "explicit Value Representation");
		} else {
			filetype = ImplicitVR;
			dbg.Verbose(0, "gdcmHeader::CheckSwap:",
			            "not an explicit Value Representation");
		}
		
		if (net2host) { // HostByteOrder is different from NetworkByteOrder
			sw = 0;    // on est sur PC ou DEC --> LITTLE-ENDIAN -> Rien a faire
			dbg.Verbose(0, "gdcmHeader::CheckSwap:",
			               "HostByteOrder = NetworkByteOrder");
		} else {    /* on est sur une Sun ou une SGI */
			sw = 4321;
			dbg.Verbose(0, "gdcmHeader::CheckSwap:",
			               "HostByteOrder != NetworkByteOrder");
		}
		
		rewind(fp);
		fseek (fp, 132L, SEEK_SET); //On se positionne sur le debut des info
		offsetCourant=132;
	} // End of TrueDicom

	// Pas du TrueDicom : permiere hypothese c'est de l'ACR 'propre', auquel
	// cas la lgr du premier element du groupe est FORCEMENT 4
	entCur=deb + 4;
	s=str2num(entCur,int);
	
	switch (s) {
	case 0x00040000 :
		sw=3412;
		filetype = ACR;
		break;
	case 0x04000000 :
		sw=4321;
		filetype = ACR;
		break;
	case 0x00000400 :
		sw=2143;
			filetype = ACR;
		break;
	case 0x00000004 :
		sw=0;
		filetype = ACR;
		break;
	default :
		sw = -1;
		dbg.Verbose(0, "gdcmHeader::CheckSwap:",
		               "unfound swap info (time to raise bets)");
	}
	if(sw!=-1) {
		rewind(fp);    // les info commencent au debut
		offsetCourant=0;
		return;
	}

	// Deuxieme hypothese : c'est de l'ACR 'pas propre' i.e. il manque
	// la lgr du groupe
	
	// On n'a pas trouve l'info de swap.
	// Si c'est du VRAI ACR NEMA et
	//  * si on est sur une DEC ou un PC alors swap=0,
	//  * si on est sur SUN ou SGI,      alors swap=4321
	// Si c'est du RAW, ca degagera + tard
	dbg.Verbose(0, "gdcmHeader::CheckSwap:",
	               "time for wild guesses...");
	
	if (x!=ntohs(x)) // HostByteOrder is different from NetworkByteOrder
		// on est sur PC ou DEC --> LITTLE-ENDIAN -> Rien a faire
		sw = 0;
	else
		// on est sur Sun ou SGI
		sw = 4321;
	rewind(fp);    // les info commencent au debut
	offsetCourant=0;
	return;
}

/**
 * \ingroup gdcmHeader
 * \brief   Pour les fichiers non TrueDicom, si le recognition
 *          code (0008,0010) s'avere etre "ACR_LIBIDO", alors
 *          valide la reconnaissance du fichier en positionnant
 *          filetype.
 */
void gdcmHeader::setAcrLibido() {
	string RecCode;
	
	if ( filetype != TrueDicom) {
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

long int gdcmHeader::RecupLgr( ElValue *pleCourant, int *skippedLength) {
	guint32 l_gr; 
	unsigned short int l_gr_2;
	int i;
	char VR[5];
	int lgrLue;
	
	//  FIX ME
	//   ATTENTION : nbCode correspond au nombre d'elements dans la table
	//               de type DICOM_VR. A nettoyer.
	//
	int nbCode=26;
	
	if (filetype == ExplicitVR) {
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
		if (FoundVR.empty()) {
			pleCourant->SetVR(FoundVR);
		} else {
			
			// On est mal : implicit VR repere
			// mais ce n'est pas un code connu ...
			// On reconstitue la longueur
			
			dbg.Verbose(1, "gdcmHeader::RecupLgr:",
							"Explicit VR, but no known code");
			memcpy(&l_gr, VR,(size_t)2);
			lgrLue=fread ( ((char*)&l_gr)+2, (size_t)2, (size_t)1, fp);
			l_gr = SwapLong((guint32)l_gr);
			pleCourant->SetLgrLue(l_gr);
			if ( (int)l_gr == -1)
				l_gr=0;
			 
			*skippedLength = 4; 
			return(l_gr);
		}
		
		// On repart dans la sequence 'sensee'
		if ( (!memcmp( VR,"OB",(size_t)2 )) || 
		     (!memcmp( VR,"OW",(size_t)2 )) || 
		     (!memcmp( VR,"SQ",(size_t)2 )) ||
		     (!memcmp( VR,"UN",(size_t)2 )) ) {
			
			// les 2 octets suivants sont reserves: on les saute
			fseek(fp, 2L,SEEK_CUR);
			
			//on lit la lgr sur QUATRE octets
			lgrLue=fread (&l_gr, (size_t)4,(size_t)1, fp);
			l_gr = SwapLong((guint32)l_gr);
			*skippedLength = 8;
		
		} else {
			//on lit la lgr sur DEUX octets
			lgrLue=fread (&l_gr_2, (size_t)2,(size_t)1, fp);
			
			if(sw) l_gr_2 = SwapShort((unsigned short)l_gr_2);
			
			pleCourant->SetLgrLue(l_gr_2);
			
			if ( l_gr_2 == 0xffff) {
				l_gr = 0;
			} else {
				l_gr = l_gr_2;
			}
			*skippedLength = 4;
		}
	} else {
		// Explicit VR = 0
		//on lit la lgr sur QUATRE octets
		
		lgrLue=fread (&l_gr, (size_t)4,(size_t)1, fp);
		
		l_gr= SwapLong((long)l_gr);
		*skippedLength = 4;
	}
	
	pleCourant->SetLgrLue(l_gr);
	
	// Traitement des curiosites sur la longueur
	
	if ( (int)l_gr == 0xffffffff)
		l_gr=0; 
	
	if(!memcmp( VR,"SQ",(size_t)2 )) { // ca annonce une SEQUENCE d'items ?!
		l_gr=0;                         // on lira donc les items de la sequence 
	}
	
	return(l_gr);
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
	unsigned short g;
	unsigned short n;
	guint32 l;
	long int posFich;
	int skL;
	size_t lgrLue;
	//CLEANME DICOM_ELEMENTS *t;
	ElValue * nouvDcmElem;
	
	// FIXME la probabilte pour depasser sans s'en rendre compte
	// est grande avec le test d'egalite' suivant !
	if(offsetCourant == taille_fich) { // On a atteint la fin du fichier
		dbg.Verbose(1, "ReadNextElement: EOF reached");
		return(NULL);
	}
	
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

	// ------------------------- Lecture longueur element : l
	
	l = RecupLgr(nouvDcmElem, &skL);
	
	if(g==0xfffe) l=0;  // pour sauter les indicateurs de 'SQ'
	
	nouvDcmElem->LgrElem=l;
	
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
	
	offsetCourant +=  2 + 2 + skL; // gr +  num + lgr
	nouvDcmElem->Offset = offsetCourant;
	offsetCourant += l;            // debut elem suivant

	// ------------------------- Doit-on le Swapper ?
	
	if ((n==0) && sw)  {  // n=0 : lgr du groupe : guint32
		*(guint32 *) NewValue = SwapLong ((*(guint32 *) NewValue));  
	} else {
		if(sw) {
			if ( (g/2)*2-g==0) { /* on ne teste pas les groupes impairs */
				
				if ((l==4)||(l==2)) {  // pour eviter de swapper les chaines 
					                    // de lgr 2 ou 4
					
					// FIXME make reference to nouvDcmElem->GetTag
					string VR = NewTag->GetVR();
					if (   (VR == "UL") || (VR == "US")
					    || (VR == "SL") || (VR == "SS")
					    || (g == 0x0028 && ( n == 0x0005 || n == 0x0200) )) {
						// seuls (28,5) de vr RET et (28,200) sont des entiers
						// ... jusqu'a preuve du contraire
						
						if(l==4) { 
							*(guint32 *) NewValue =
							    SwapLong  ((*(guint32 *) NewValue)); 
						} else {
							if(l==2) 
						    *(unsigned short *) NewValue =
						    	SwapShort ((*(unsigned short *)NewValue));
						 }
					}
				} /* fin if l==2 ==4 */
			} /* fin if g pair */
		} /* fin sw */	
	}
	nouvDcmElem->value = NewValue;
	// CLEAN ME: simply trash the following line and postpone the process !
	// SetAsidePixelData(nouvDcmElem);
	return nouvDcmElem;
}

/**
 * \ingroup gdcmHeader
 * \brief   If we encountered the offset of the pixels in the file
 *          (Pixel Data) then keep the info aside.
 */
void gdcmHeader::SetAsidePixelData(ElValue* elem) {
	  /// FIXME this wall process is bizarre:
	  // on peut pas lire pixel data et pixel location puis
	  // a la fin de la lecture aller interpreter ce qui existe ?
	  // penser a nettoyer les variables globales associes genre
	  // PixelsTrouve ou grPixelTrouve...
	  //
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
	//     it exists) what the offset it.
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
					char* NewValue = (char*)g_malloc(elem->GetLgrElem()+1);
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

string gdcmHeader::GetPubElValByNumber(unsigned short, unsigned short) {
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
	fseek(fp, 0L, SEEK_END);
	/*
	 * obtains the current value of the file-position    
	 * indicator for the stream pointed to by stream      
	 */
	taille_fich = ftell(fp);
	rewind(fp);
	CheckSwap();
	while ( (newElValue = ReadNextElement()) ) {
		PubElVals.Add(newElValue);
	}
	setAcrLibido();
}

int gdcmHeader::PrintPubElVal(ostream & os) {
	return PubElVals.Print(os);
}

void gdcmHeader::PrintPubDict(ostream & os) {
	RefPubDict->Print(os);
}
