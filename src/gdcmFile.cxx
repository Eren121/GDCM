// gdcmFile.cxx

#include "gdcmFile.h"

static void _Swap(void* im, int swap, int lgr, int nb);

/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief Constructor dedicated to writing a new DICOMV3 part10 compliant
 *        file (see SetFileName, SetDcmTag and Write)
 *        Opens (in read only and when possible) an existing file and checks
 *        for DICOM compliance. Returns NULL on failure.
 * \Note  the in-memory representation of all available tags found in
 *        the DICOM header is post-poned to first header information access.
 *        This avoid a double parsing of public part of the header when
 *        one sets an a posteriori shadow dictionary (efficiency can be
 *        seen as a side effect).   
 *
 * @param filename file to be opened for parsing
 *
 * @return	
 */
 
gdcmFile::gdcmFile(string & filename) 
	:gdcmHeader(filename.c_str())	
{
}

gdcmFile::gdcmFile(const char * filename) 
	:gdcmHeader(filename)	
{
}


/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief     Renvoie la longueur A ALLOUER pour recevoir les pixels de l'image
 *  		ou DES images dans le cas d'un multiframe
 *  		ATTENTION : il ne s'agit PAS de la longueur du groupe des Pixels	
 *  		(dans le cas d'images compressees, elle n'a pas de sens).
 *
 * @return	longueur a allouer 
 */

size_t gdcmFile::GetImageDataSize(void) {
	int nbLignes, nbCol, nbFrames, nb;
	string str_nbFrames, str_nb;
	// Nombre de Lignes	
	nbLignes=atoi(gdcmHeader::GetPubElValByNumber(0x0028,0x0010).c_str());
	// Nombre de Colonnes	
	nbCol   =atoi(gdcmHeader::GetPubElValByNumber(0x0028,0x0011).c_str());

	// Nombre de Frames	
	str_nbFrames=gdcmHeader::GetPubElValByNumber(0x0028,0x0008);
	
	if (str_nbFrames == "gdcm::Unfound" ) {
		nbFrames = 1;
	} else {
		nbFrames = atoi(str_nbFrames.c_str() );
	}
	
	// Nombre de Bits Alloues pour le stockage d'un Pixel	
	str_nb=gdcmHeader::GetPubElValByNumber(0x0028,0x0100);

	if (str_nb == "gdcm::Unfound" ) {
		nb = 16;
	} else {
		nb = atoi(str_nb.c_str() );
	}

	size_t lgrTotale = nbFrames*nbLignes*nbCol*(nb/8);
	return (lgrTotale);
}



/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief TODO
 * \warning WARNING
 *
 */
void * gdcmFile::GetImageData (void) {
	char * _Pixels;
	// Longueur en Octets des Pixels a lire
	size_t taille = GetImageDataSize();// ne faudrait-il pas la stocker?
	_Pixels = (char *) malloc(taille);
	GetImageDataIntoVector(_Pixels, taille);
	
		// On l'affecte à un champ du dcmFile	
	Pixels =    _Pixels;
	lgrTotale = taille;
	
	// ca fait double emploi, il faudra nettoyer ça
	
	return(_Pixels);
}



/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief amene en mémoire dans une zone précisee par l'utilisateur
 *        les Pixels d'une image NON COMPRESSEE
 * \Warning Aucun test n'est fait pour le moment sur le caractere compresse ou non de l'image
 *
 * @param destination
 * @param MaxSize
 *
 * @return TODO JPR	
 */

int gdcmFile::GetImageDataIntoVector (void* destination, size_t MaxSize) {

// Question :
//	dans quel cas la MaxSize sert-elle a quelque chose?
// 	que fait-on si la taille de l'image est + gde    que Maxize?
// 	que fait-on si la taille de l'image est + petite que Maxize?

	
	void * Pixels = destination;  // pour garder le code identique avec GetImageData

	int nb, nbu, highBit, signe;
	string str_nbFrames, str_nb, str_nbu, str_highBit, str_signe;
	
	unsigned short int mask = 0xffff;
	
	// Longueur en Octets des Pixels a lire
	size_t _lgrTotale = GetImageDataSize();	// ne faudrait-il pas la stocker?
	
	// si lgrTotale < MaxSize ==> Gros pb 
	// -> on résoud à la goret
	
	if ( _lgrTotale < MaxSize ) MaxSize = _lgrTotale;
	
	GetPixels(MaxSize, destination);
			
	// Nombre de Bits Alloues pour le stockage d'un Pixel	
	str_nb=gdcmHeader::GetPubElValByNumber(0x0028,0x0100);

	if (str_nb == "gdcm::Unfound" ) {
		nb = 16;
	} else {
		nb = atoi(str_nb.c_str() );
	}
	
	// Nombre de Bits Utilises	
	str_nbu=GetPubElValByNumber(0x0028,0x0101);

	if (str_nbu == "gdcm::Unfound" ) {
		nbu = nb;
	} else {
		nbu = atoi(str_nbu.c_str() );
	}	
	
	// Position du Bit de Poids Fort	
	str_highBit=GetPubElValByNumber(0x0028,0x0102);

	if (str_highBit == "gdcm::Unfound" ) {
		highBit = nb - 1;
	} else {
		highBit = atoi(str_highBit.c_str() );
	}
		
	// Signe des Pixels 
	str_signe=GetPubElValByNumber(0x0028,0x0103);

	if (str_signe == "gdcm::Unfound" ) {
		signe = 1;
	} else {
		signe = atoi(str_signe.c_str() );
	}

	// On remet les Octets dans le bon ordre si besoin est
	if (nb != 8) {
		int _sw = GetSwapCode();

		_Swap (destination, _sw, _lgrTotale, nb);
	}
	
	// On remet les Bits des Octets dans le bon ordre si besoin est
	//
	// ATTENTION :  Jamais confronté a des pixels stockes sur 32 bits 
	//			avec moins de 32 bits utilises
	//			et dont le bit de poids fort ne serait pas la ou on l'attend ...
	// 			--> ne marchera pas dans ce cas 
	if (nbu!=nb){
		mask = mask >> (nb-nbu);
		int l=(int)MaxSize/(nb/8);
		unsigned short *deb = (unsigned short *)Pixels;
		for(int i=0;i<l;i++) {
				*deb = (*deb >> (nbu-highBit-1)) & mask;
				deb ++;
		}
	}
			
	// VOIR s'il ne faudrait pas l'affecter à un champ du dcmHeader
	
	return 1; 
}


//
// Je laisse le code integral, au cas ça puisse etre reutilise ailleurs
//

static void _Swap(void* im, int swap, int lgr, int nb) {                     
guint32 s32;
guint16 fort,faible;
int i;

if(nb == 16)
    
	switch(swap) {
		case 0:
		case 12:
 		case 1234:
			break;
		
 		case 21:
 		case 3412:
 		case 2143:
 		case 4321:

 			for(i=0;i<lgr;i++)
  				((unsigned short int*)im)[i]= ((((unsigned short int*)im)[i])>>8)
                            			| ((((unsigned short int*)im)[i])<<8);
 			break;
 			
 		default:
 			printf("valeur de SWAP (16 bits) non autorisee : %d\n", swap);
	} 
 
if( nb == 32 )

	switch (swap) {
    		case 0:
     		case 1234:
    			 break;

    		case 4321:
    			 for(i=0;i<lgr;i++) {
        			faible=  ((unsigned long int*)im)[i]&0x0000ffff;    /* 4321 */
       				fort  =((unsigned long int*)im)[i]>>16;
        			fort=  (fort>>8)   | (fort<<8);
        			faible=(faible>>8) | (faible<<8);
        			s32=faible;
        			((unsigned long int*)im)[i]=(s32<<16)|fort;
       			}
    			break;

    		case 2143:
    			for(i=0;i<lgr;i++) {
       				faible=  ((unsigned long int*)im)[i]&0x0000ffff;    /* 2143 */
       				fort=((unsigned long int*)im)[i]>>16;
       				fort=  (fort>>8)   | (fort<<8);
       				faible=(faible>>8) | (faible<<8);
       				s32=fort; 
       				((unsigned long int*)im)[i]=(s32<<16)|faible;
      			}
    			break;
  
    		case 3412:
    			for(i=0;i<lgr;i++) {
      				faible=  ((unsigned long int*)im)[i]&0x0000ffff;    /* 3412 */
       				fort=((unsigned long int*)im)[i]>>16;                  
       				s32=faible; 
       				((unsigned long int*)im)[i]=(s32<<16)|fort;
      			}                 
    			break; 
    			        
   		default:
   			printf("valeur de SWAP (32 bits) non autorisee : %d\n", swap);
	} 
return;
}

/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief TODO JPR
 * \warning doit-etre etre publique ?  FIXME JPR
 *
 * @param Data TODO JPR
 * @param ExpectedSize TODO JPR
 *
 * @return TODO JPR	
 */
int gdcmFile::SetImageData(void * Data, size_t ExpectedSize) {
	
	SetImageDataSize(ExpectedSize);
	
	Pixels =    Data;
	lgrTotale = ExpectedSize;
	
	return(1);
}


/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief TODO JPR
 * \
 * \warning WARNING doit-etre etre publique ? FIXME JPR
 *
 * @param ImageDataSize TODO JPR
 *
 */

void gdcmFile::SetImageDataSize(size_t ImageDataSize) {

 	string content1;
 	string content2;
 	char car[20];
 	
 	// suppose que le ElValue (0x7fe0, 0x0010) existe ...
 	
 	sprintf(car,"%d",ImageDataSize);
 	content2=car;
 	SetPubElValByNumber(content2, 0x7fe0, 0x0010);
 	
 	ImageDataSize+=8;
 	sprintf(car,"%d",ImageDataSize);
 	content1=car;	
 	SetPubElValByNumber(content1, 0x7fe0, 0x0000);
}


/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief Ecrit sur disque les pixels d'UNE image
 *        Aucun test n'est fait sur l'"Endiannerie" du processeur.
 *        Ca sera à l'utilisateur d'appeler son Reader correctement
 *        (Equivalent a IdImaWriteRawFile) FIXME JPR
 *
 * @param nomFichier TODO JPR
 *
 * @return TODO JPR	
 */

int gdcmFile::WriteRawData (string nomFichier) {

	FILE * fp1;
	fp1 = fopen(nomFichier.c_str(),"wb");
	if (fp1 == NULL) {
		printf("Echec ouverture (ecriture) Fichier [%s] \n",nomFichier.c_str());
		return (0);
	} 
	
	fwrite (Pixels,lgrTotale, 1, fp1);
	fclose (fp1);
	return(1);
}



/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief Ecrit sur disque UNE image Dicom
 *        Aucun test n'est fait sur l'"Endiannerie" du processeur.
 *         Ca fonctionnera correctement (?) sur processeur Intel
 *         (Equivalent a IdDcmWrite) FIXME JPR 
 *
 * @param nomFichier TODO JPR
 *
 * @return	TODO JPR
 */

int gdcmFile::WriteDcm (string nomFichier) {

// ATTENTION : fonction non terminée (commitée a titre de precaution)

	FILE * fp1;
	char* filePreamble;
	fp1 = fopen(nomFichier.c_str(),"wb");
	if (fp1 == NULL) {
		printf("Echec ouverture (ecriture) Fichier [%s] \n",nomFichier.c_str());
		return (0);
	} 
	
	//	Ecriture Dicom File Preamble
	filePreamble=(char*)calloc(128,1);
	fwrite(filePreamble,128,1,fp1);
	fwrite("DICM",4,1,fp1);

	// un accesseur de + est obligatoire ???
	// pourtant le gdcmElValSet contenu dans le gdcmHeader 
	// ne devrait pas être visible par l'utilisateur final (?)
	
	GetPubElValSet().Write(fp1);
		
	fwrite(Pixels, lgrTotale, 1, fp1);

	fclose (fp1);
	return(1);
}

int gdcmFile::WriteDcm (const char* nomFichier) {
   WriteDcm (string (nomFichier));
}
	
	
/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 *
 * @param  nomFichier TODO JPR
 *
 * @return TODO JPR
 */

int gdcmFile::WriteDcmExplVR (string nomFichier) {

// ATTENTION : fonction non terminée (commitée a titre de precaution)

	FILE * fp1;
	char* filePreamble;
	fp1 = fopen(nomFichier.c_str(),"wb");
	if (fp1 == NULL) {
		printf("Echec ouverture (ecriture) Fichier [%s] \n",nomFichier.c_str());
		return (0);
	} 
	
	//	Ecriture Dicom File Preamble
	filePreamble=(char*)calloc(128,1);
	fwrite(filePreamble,128,1,fp1);
	fwrite("DICM",4,1,fp1);

	// un accesseur de + est obligatoire ???
	// pourtant le gdcmElValSet contenu dans le gdcmHeader 
	// ne devrait pas être visible par l'utilisateur final (?)
	
	GetPubElValSet().WriteExplVR(fp1);
		
	fwrite(Pixels, lgrTotale, 1, fp1);

	fclose (fp1);
	return(1);
}
	
/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief  Ecrit sur disque UNE image ACR-NEMA 
 *        (a l'attention des logiciels cliniques 
 *        qui ne prennent en entrée QUE des images ACR ...
 *        si un header DICOM est fourni en entree,
 *        les groupes < 0x0008 et les groupes impairs sont ignores)
 *        Aucun test n'est fait sur l'"Endiannerie" du processeur.
 *        Ca fonctionnera correctement (?) sur processeur Intel
 *        (Equivalent a IdDcmWrite) 
 *
 * @param nomFichier TODO JPR
 *
 * @return TODO JPR	
 */

int gdcmFile::WriteAcr (string nomFichier) {

// ATTENTION : fonction non terminée (commitée a titre de precaution)

	FILE * fp1;
	fp1 = fopen(nomFichier.c_str(),"wb");
	if (fp1 == NULL) {
		printf("Echec ouverture (ecriture) Fichier [%s] \n",nomFichier.c_str());
		return (0);
	} 

	// un accesseur de + est obligatoire ???
	// pourtant le gdcmElValSet contenu dans le gdcmHeader 
	// ne devrait pas être visible par l'utilisateur final (?)
	
	GetPubElValSet().WriteAcr(fp1);
		
	fwrite(Pixels, lgrTotale, 1, fp1);

	fclose (fp1);
	return(1);
}
