// gdcmFile.cxx

#include "gdcm.h"

static void _Swap(void* im, int swap, int lgr, int nb);

/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief Constructor dedicated to writing a new DICOMV3 part10 compliant
 * \file (see SetFileName, SetDcmTag and Write)
 * \Opens (in read only and when possible) an existing file and checks
 * \for DICOM compliance. Returns NULL on failure.
 * \Note: the in-memory representation of all available tags found in
 * \the DICOM header is post-poned to first header information access.
 * \This avoid a double parsing of public part of the header when
 * \one sets an a posteriori shadow dictionary (efficiency can be
 * \seen as a side effect).   
 *
 * @param 
 *
 * @return	
 */
 
gdcmFile::gdcmFile(string & filename)
	:gdcmHeader(filename.c_str())
{
}


/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief     Renvoie la longueur A ALLOUER pour recevoir les pixels de l'image
 * \		ou DES images dans le cas d'un multiframe
 * \		ATTENTION : il ne s'agit PAS de la longueur du groupe des Pixels	
 * \		(dans le cas d'images compressees, elle n'a pas de sens).
 *
 * @param void	Rien en entree
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
	
	if (str_nbFrames == "UNFOUND" ) {
		nbFrames = 1;
	} else {
		nbFrames = atoi(str_nbFrames.c_str() );
	}
	
	// Nombre de Bits Alloues pour le stockage d'un Pixel	
	str_nb=gdcmHeader::GetPubElValByNumber(0x0028,0x0100);

	if (str_nb == "UNFOUND" ) {
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
 * \brief amene en mémoire les Pixels d'une image NON COMPRESSEE
 * \Aucun test n'est fait pour le moment sur le caractere compresse ou non de l'image
 *
 * @param rien
 *
 * @return	Pointeur sur la zone mémoire contenant les Pixels lus
 */

void * gdcmFile::GetImageData (void) {
	
	char* Pixels;
	int nbLignes, nbCol;

	int nbFrames, nb, nbu, highBit, signe;
	string str_nbFrames, str_nb, str_nbu, str_highBit, str_signe;
	
	unsigned short int mask = 0xffff;
		
	// Nombre de Lignes	
	nbLignes=atoi(GetPubElValByNumber(0x0028,0x0010).c_str());
	// Nombre de Colonnes	
	nbCol   =atoi(GetPubElValByNumber(0x0028,0x0011).c_str());

	// Nombre de Frames	
	str_nbFrames=GetPubElValByNumber(0x0028,0x0008);

	
	if (str_nbFrames == "UNFOUND" ) {
		nbFrames = 1;
	} else {
		nbFrames = atoi(str_nbFrames.c_str() );
	}
	
	// Nombre de Bits Alloues	
	str_nb=GetPubElValByNumber(0x0028,0x0100);

	if (str_nb == "UNFOUND" ) {
		nb = 16;
	} else {
		nb = atoi(str_nb.c_str() );
	}
	
	// Nombre de Bits Utilises	
	str_nbu=GetPubElValByNumber(0x0028,0x0101);

	if (str_nbu == "UNFOUND" ) {
		nbu = nb;
	} else {
		nbu = atoi(str_nbu.c_str() );
	}	
	
	// Position du Bit de Poids Fort	
	str_highBit=GetPubElValByNumber(0x0028,0x0102);

	if (str_highBit == "UNFOUND" ) {
		highBit = nb - 1;
	} else {
		highBit = atoi(str_highBit.c_str() );
	}
		
	// Signe des Pixels 
	str_signe=GetPubElValByNumber(0x0028,0x0103);

	if (str_signe == "UNFOUND" ) {
		signe = 1;
	} else {
		signe = atoi(str_signe.c_str() );
	}
	
	// Longueur en Octets des Pixels a lire
	size_t lgrTotale = nbFrames*nbLignes*nbCol*(nb/8);
	
	//Pixels = (char *) g_malloc(lgrTotale);
	Pixels = (char *) malloc(lgrTotale);
	
	GetPixels(lgrTotale, Pixels);

	// On remet les Octets dans le bon ordre si besoin est
	if (nb != 8) {
		int _sw = GetSwapCode();

		_Swap (Pixels, _sw, lgrTotale, nb);   						 // A REMETTRE
	}
	
	// On remet les Bits des Octets dans le bon ordre si besoin est
	//
	// ATTENTION :  Jamais confronté a des pixels stockes sur 32 bits 
	//			avec moins de 32 bits utilises
	//			et dont le bit de poids fort ne serait pas la ou on l'attend ...
	// 			--> ne marchera pas dans ce cas 
	if (nbu!=nb){
		mask = mask >> (nb-nbu);
		int l=(int)lgrTotale/(nb/8);
		unsigned short *deb = (unsigned short *)Pixels;
		for(int i=0;i<l;i++) {
				*deb = (*deb >> (nbu-highBit-1)) & mask;
				deb ++;
		}
	}
		
	printf ("on est sorti\n");
	
	// VOIR s'il ne faudrait pas l'affecter à un champ du dcmHeader
	
	return (Pixels);		
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



