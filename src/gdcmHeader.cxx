#include "gdcmlib.h"

/* =======================================================================

   _IdDcmCheckSwap
      La seule maniere sure que l'on aie pour determiner 
      si on est en   LITTLE_ENDIAN,       BIG-ENDIAN, 
                  BAD-LITTLE-ENDIAN, BAD-BIG-ENDIAN
      est de trouver l'element qui donne la longueur d'un 'GROUP'
      (on sait que la longueur de cet element vaut 0x00000004)
      et de regarder comment cette longueur est codee en memoire  

      Le probleme vient de ce que parfois, il n'y en a pas ...

      On fait alors le pari qu'on a a faire a du LITTLE_ENDIAN propre.
      (Ce qui est la norme -pas respectee- depuis ACR-NEMA)
      Si ce n'est pas le cas, on ne peut rien faire.

      (il faudrait avoir des fonctions auxquelles 
       on passe le code Swap en parametre, pour faire des essais 'manuels')

   ======================================================================= */

EndianType gdcmHeader::gdcmHeader()
{
	//guint32  s;
	guint32  x=4;  // x : pour ntohs
	bool net2host; // true when HostByteOrder is the same as NetworkByteOrder

	int sw;
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

   lgrLue = fread(deb,1,LGR_ENTETE_A_LIRE,e->fp);

   entCur = deb+128;
   if(memcmp(entCur, "DICM", (size_t)4) == 0) {
      filetype = TrueDicom;
      if (DEBUG) printf ("_IdDcmCheckSwap : C est du DICOM actuel \n");
   } else {
      filetype = Unknown;
      if (DEBUG) printf ("_IdDcmCheckSwap : Ce n'est PAS du DICOM actuel\n");
   }

   if(filetype == TrueDicom) {
	   // on saute le File Preamble (souvent a ZERO) : 128 Octets
      // + le DICM (4), et le (0002, 0000) soit 4 (136 = 128 + 4 + 4)
      entCur = deb+136;
      if(memcmp(entCur, "UL", (size_t)2) == 0) {
         // les 2 premiers octets de la lgr peuvent valoir UL --> Explicit VR
         filetype = ExplicitVR;
         if (DEBUG)  printf ("_IdDcmCheckSwap : Explicit VR\n");
      } else {
         filetype = ImplicitVR;
         if (DEBUG)  printf ("_IdDcmCheckSwap : PAS Explicit VR\n");
      }

      if (net2host) { // HostByteOrder is different from NetworkByteOrder
         sw = 0;    // on est sur PC ou DEC --> LITTLE-ENDIAN -> Rien a faire
         if (DEBUG) printf("HostByteOrder = NetworkByteOrder\n");

      } else {    /* on est sur une Sun ou une SGI */
         sw = 4321;
         if (DEBUG) printf("HostByteOrder != NetworkByteOrder\n");
      }

      rewind(e->fp);
      fseek (e->fp, 132L, SEEK_SET); //On se positionne sur le debut des info
      e->offsetCourant=132;
      return sw;

   } /* fin TrueDicom */

	// Pas du TrueDicom : permiere hypothese c'est de l'ACR 'propre', auquel
	// cas la lgr du premier element du groupe est FORCEMENT 4

   entCur=deb + 4;
   s=str2num(entCur,int);

   switch (s) {
      case 0x00040000 :
         sw=3412; if(DEBUG) printf("s : %08x sw : %d\n",s,sw);
			filetype = ACR;
         break;
      case 0x04000000 :
         sw=4321; if(DEBUG) printf("s : %08x sw : %d\n",s,sw);
			filetype = ACR;
         break;
      case 0x00000400 :
         sw=2143; if(DEBUG) printf("s : %08x sw : %d\n",s,sw);
			filetype = ACR;
         break;
      case 0x00000004 :
         sw=0;    if(DEBUG) printf("s : %08x sw : %d\n",s,sw);
			filetype = ACR;
         break;
      default :
         sw = -1;
         if (DEBUG) printf (" Pas trouve l info de Swap; On va parier\n");
	}

	// Deuxieme hypothese : c'est de l'ACR 'pas propre' i.e. il manque
	// la lgr du groupe

	if(sw==-1) {
      /* On n'a pas trouve l'info de swap    28/11/2000 JPR                */
      // Si c'est du VRAI ACR NEMA  si on est sur une DEC ou un PC swap=0,
	   // SUN ou SGI SWAP=4321
      /* si c'est du RAW, ca degagera + tard                      */

      if (DEBUG) printf("On force la chance \n");

      if (x!=ntohs(x)) // HostByteOrder is different from NetworkByteOrder
         sw = 0;    // on est sur PC ou DEC --> LITTLE-ENDIAN -> Rien a faire
      else
         sw = 4321; // on est sur Sun ou SGI
	}

   rewind(e->fp);    // les info commencent au debut
   e->offsetCourant=0;
   return (sw);
}

void  gdcmHeader::_setAcrLibido() {

   _ID_DCM_ELEM * ple;
   PLIST_ELEMENT plelem;
   PLIST pl;

   // Positionnement ACR_LIBIDO
   if(DEBUG) printf("Entree ds _setAcrLibido\n");

   filetype = ACR_LIBIDO = 0;
   if ( filetype != TrueDicom) {
      // Recognition Code  --> n'existe plus en DICOM V3 ...

      pl = e->plist;
      plelem = IdLstFirst(pl);
      while (plelem) {
         ple= IdLstPtrObj(plelem);
         if(DEBUG) printf("gr %04x Num %04x\n", ple->Gr, ple->Num);
         if(ple->Gr >  0x0008) break;     // On a depasse
         if(ple->Gr == 0x0008) {
            if(ple->Num >  0x0010) break; // On a depasse
            if(ple->Num == 0x0010) {
               if (  (memcmp(ple->valeurElem,"ACRNEMA_LIBIDO",14)==0)
                  // si c'est egal
                  || (memcmp(ple->valeurElem,"CANRME_AILIBOD",14)==0)) {
                  // en cas d'objet ACRLibido fait sr 1 autre machine) 
                  e->ACR_LIBIDO =1;
               }  // fin if memcmp
               break;
            } // fin if ple->Num==0x0010
         } // fin ple->Gr==0x0008
         plelem = IdLstNext(plelem);
      } // fin while 
   } // fin if TrueDicom

   return;
}

/* ======================================================================= 
*	_IdDcmRecupLgr
*
*	ACR-NEMA :	On a toujours 
*				GroupNumber   (2 Octets) 
*				ElementNumber (2 Octets) 
*				ElementSize   (4 Octets)
*
*
*	DICOM :		On peut avoir (implicit Value Representation)
*				GroupNumber   (2 Octets) 
*				ElementNumber (2 Octets) 
*				ElementSize   (4 Octets)
*
*			On peut avoir (explicit Value Representation)
*				GroupNumber         (2 Octets) 
*				ElementNumber       (2 Octets) 
*				ValueRepresentation (2 Octets) 
*				ElementSize         (2 Octets)
*
*			ATTENTION : dans le cas ou ValueRepresentation = OB, OW, SQ, UN
*				GroupNumber         (2 Octets) 
*				ElementNumber       (2 Octets) 
*				ValueRepresentation (2 Octets)
*				zone reservee       (2 Octets) 
*				ElementSize         (4 Octets)
*
*
*   ======================================================================= */
/**
 * \ingroup       dcm
 * \brief         recupere la longueur d'un champ DICOM.
 *			(le fichier doit deja avoir ete ouvert,
 *			 _IdAcrCheckSwap(ID_DCM_HDR *e) avoir ete appele)
 *			et la  partie 'group'  ainsi que la  partie 'elem' 
 * 		        de l'acr_element doivent avoir ete lues.
 * @param sw      			code swap
 * @param skippedLength     pointeur sur nombre d'octets que l'on a saute qd la lecture est finie
 * @param longueurLue       pointeur sur longueur (en nombre d'octets) effectivement lue

 * @return        			longueur retenue pour le champ 
 */

static guint32 _IdDcmRecupLgr(ID_DCM_HDR *e, int sw, int *skippedLength, int *longueurLue) {
guint32 l_gr; 
unsigned short int l_gr_2;
int i, trouve;
char VR[5];
int lgrLue;

/*
 *   ATTENTION :
*/

int nbCode=26; // nombre d'elements dans la table de type DICOM_VR definie dans dicom.c

/* ================ */

// ID_DCM_HDR *e sert uniquement de passe-plat pour  __ExplicitVR


if (e->__ExplicitVR == 1) {
	lgrLue=fread (&VR, (size_t)2,(size_t)1, e->fp);
	VR[2]=0;

	// ATTENTION :
	// Ce n'est pas parce qu'on a trouve UL la premiere fois qu'on respecte 
	// Explicit VR tout le temps
	// (cf e=film ...)

	for(i=0,trouve=0;i<nbCode;i++) {
		if(memcmp(_ID_dicom_vr[i].dicom_VR,VR,(size_t)2)==0) {
			(e->pleCourant)->VR=_ID_dicom_vr[i].dicom_VR;
			trouve=1;	
			break;
		}
	}

	if ( trouve == 0) {

		// On est mal : implicit VR repere
		// mais ce n'est pas un code connu ...
		// On reconstitue la longueur
		
		if(DEBUG) printf("IdDcmRecupLgr : Explicit VR, mais pas trouve de code connu\n");
		memcpy(&l_gr, VR,(size_t)2);

		lgrLue=fread ( ((char*)&l_gr)+2, (size_t)2, (size_t)1, e->fp);

		if(sw) l_gr = _IdDcmSWAP_LONG(((guint32)l_gr),sw);
		
		if(DEBUG) printf("IdDcmRecupLgr : lgr deduite : %08x , %d\n",l_gr,l_gr);
		
		*longueurLue=l_gr;
		if ( (int)l_gr == -1) {	
			l_gr=0;
		}
		*skippedLength = 4; 
		if (DEBUG) printf(" 1 : lgr %08x (%d )skippedLength %d\n",l_gr,l_gr, *skippedLength);
		return(l_gr);
	}

	// On repart dans la sequence 'sensee'

	if(DEBUG) printf("VR : [%01x , %01x] (%c%c) en position %d du tableau\n", VR[0],VR[1],VR[0],VR[1],i);
	//printf(" %d , %s\n", i,_ID_dicom_vr[i].dicom_VR);
	
	if ( 	
	   (!memcmp( VR,"OB",(size_t)2 )) || 
	   (!memcmp( VR,"OW",(size_t)2 )) || 
	   (!memcmp( VR,"SQ",(size_t)2 )) ||
	   (!memcmp( VR,"UN",(size_t)2 )) ) {

	// les 2 octets suivants sont reserves

		if(DEBUG) printf("IdDcmRecupLgr : les 2 octets suivants sont reserves\n");
				 	//on les saute		
		fseek(e->fp, 2L,SEEK_CUR);
		
					//on lit la lgr sur QUATRE octets

		lgrLue=fread (&l_gr, (size_t)4,(size_t)1, e->fp);

		if(sw) l_gr = _IdDcmSWAP_LONG(((guint32)l_gr),sw);
		*skippedLength = 8;

	} else {
 					//on lit la lgr sur DEUX octets

		lgrLue=fread (&l_gr_2, (size_t)2,(size_t)1, e->fp);

		if(sw) l_gr_2 = _IdDcmSWAP_SHORT((unsigned short)l_gr_2,sw);
		
		*longueurLue=l_gr_2;

		
		if ( l_gr_2 == 0xffff) {
			l_gr = 0;	
		} else {
			l_gr = l_gr_2;
		}
		*skippedLength = 4;
	}	
  } else {	// Explicit VR = 0	

 				//on lit la lgr sur QUATRE octets

	lgrLue=fread (&l_gr, (size_t)4,(size_t)1, e->fp);

	if(sw)l_gr=_IdDcmSWAP_LONG(((long)l_gr),sw);
	*skippedLength = 4;
  }
  
  *longueurLue=l_gr;
  
  // Traitement des curiosites sur la longueur
  
  if ( (int)l_gr == 0xffffffff)
	l_gr=0; 
	
  if(!memcmp( VR,"SQ",(size_t)2 )) { 	// ca annonce une SEQUENCE d'items ?!
 	l_gr=0;							// on lira donc les items de la sequence 
 	if (DEBUG) printf(" SQ trouve : lgr %d \n",l_gr);
  }
 	
if (DEBUG) printf(" 2 : lgr %08x (%d) skippedLength %d\n",l_gr,l_gr, *skippedLength);
  return(l_gr);
}
