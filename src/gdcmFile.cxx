// gdcmFile.cxx

#include "gdcmFile.h"
#include "gdcmUtil.h"


// TODO : remove DEBUG
#define DEBUG 0


#include "iddcmjpeg.h"
using namespace std;

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
   SetPixelDataSizeFromHeader();
}

gdcmFile::gdcmFile(const char * filename) 
	:gdcmHeader(filename)	
{
   SetPixelDataSizeFromHeader();
}

/**
 * \ingroup   gdcmFile
 * \brief     calcule la longueur (in bytes) A ALLOUER pour recevoir les
 *        	pixels de l'image
 *  		ou DES images dans le cas d'un multiframe
 *  		ATTENTION : il ne s'agit PAS de la longueur du groupe des Pixels	
 *  		(dans le cas d'images compressees, elle n'a pas de sens).
 *
 * @return	longueur a allouer 
 */
void gdcmFile::SetPixelDataSizeFromHeader(void) {
   int nb;
   string str_nb;

   str_nb=gdcmHeader::GetPubElValByNumber(0x0028,0x0100);
   if (str_nb == "gdcm::Unfound" ) {
      nb = 16;
   } else {
      nb = atoi(str_nb.c_str() );
      if (nb == 12) nb =16;
   }
   lgrTotale =  GetXSize() *  GetYSize() *  GetZSize() * (nb/8)* GetSamplesPerPixel();;
}

/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief     Returns the size (in bytes) of required memory to hold
 *            the pixel data represented in this file.
 * @return    The size of pixel data in bytes.
 */
size_t gdcmFile::GetImageDataSize(void) {
   return (lgrTotale);
}


/////////////////////////////////////////////////////////////////
/**
 * \ingroup gdcmFile
 * \brief   Read pixel data from disk (optionaly decompressing) into the
 *          caller specified memory location.
 * @param   destination Where the pixel data should be stored.
 *
 */
bool gdcmFile::ReadPixelData(void* destination) {
   if ( !OpenFile())
      return false;
      
      printf("GetPixelOffset() %d\n",GetPixelOffset() );
      
    if ( fseek(fp, GetPixelOffset(), SEEK_SET) == -1 ) {
      CloseFile();
      return false;
   }     
    
   if ( !IsDicomV3()                             ||
        IsImplicitVRLittleEndianTransferSyntax() ||
        IsExplicitVRLittleEndianTransferSyntax() ||
        IsExplicitVRBigEndianTransferSyntax()    ||
        IsDeflatedExplicitVRLittleEndianTransferSyntax() ) { 
                    
      size_t ItemRead = fread(destination, lgrTotale, 1, fp);
      if ( ItemRead != 1 ) {
         CloseFile();
         return false;
      } else {
         CloseFile();
         return true;
      }
   } 
     
  // ------------------------------- Position on begining of Jpeg Pixels
  
       int ln;
      fseek(fp,4,SEEK_CUR);
      fread(&ln,4,1,fp); 
      if(GetSwapCode()) 
         ln=SwapLong(ln);
      if (DEBUG) 
         printf ("ln %d\n",ln);
      fseek(fp,ln,SEEK_CUR);
      fseek(fp,4,SEEK_CUR);
      fread(&ln,4,1,fp); 
      if(GetSwapCode()) 
         ln=SwapLong(ln);
      if (DEBUG) 
         printf ("ln image comprimée %d\n",ln);
         
          
  // ------------------------------- JPEG LossLess : call to Jpeg Libido
   
   if (IsJPEGLossless()) {
 
       ClbJpeg* jpg = _IdDcmJpegRead(fp);
      if(jpg == NULL) {
         CloseFile();
         return false;
      } 
      // Gros soucis : 
      //  jpg->DataImg est alloue en int32 systematiquement :
      //  il faut le copier pixel par pixel dans destination ...
      //  ... qui est un void *
      // --> du CAST sportif a faire ... 
      
     // memcpy(destination,jpg->DataImg,lgrTotale);
     
     // Bon ...
     // ... y'a p't etre + ruse (?)
          
      int nb;
      string str_nb=gdcmHeader::GetPubElValByNumber(0x0028,0x0100);
      if (str_nb == "gdcm::Unfound" ) {
         nb = 16;
      } else {
         nb = atoi(str_nb.c_str() );
         if (nb == 12) nb =16;
      }
      int nBytes= nb/8;
      int * dataJpg = jpg->DataImg;
      int taille = GetXSize() *  GetYSize() *  GetZSize() * GetSamplesPerPixel();
          
      switch (nBytes) {
      
         case 1:
         {
            unsigned short *dest = (unsigned short *)destination;
            for (int i=0; i<taille; i++) {
               if (DEBUG) 
                  if (*(dataJpg +i) >255) printf("data %d\n",*(dataJpg +i) );
               *((unsigned char *)dest+i) = *(dataJpg +i);    
            }
         }
         break;        
         
         case 2:
         {
            unsigned short *dest = (unsigned short *)destination;
            
            // etonnant (?)
            // la ligne commentee ci-dessous fait passer l'exec de 0.15 sec a 5.5 sec
            // pour la meme image 512*512 ...
            // optimiseur pas mis en oeuvre (?)
            //for (int i=0; i<GetXSize() *  GetYSize() *  GetZSize(); i++) { 
            
             for (int i=0; i<taille; i++) {           
               *((unsigned short *)dest+i) = *(dataJpg +i);    
            }
         }
         break;
            
          case 4:
          {
            unsigned int *dest=(unsigned int *)destination;
            for (int i=0;i<taille; i++) {
               *((unsigned int *)dest+i) = *(dataJpg +i);    
            }
          }
         break;          
     }
      
      _IdDcmJpegFree (jpg);
      return true;
   }  
 
  // ------------------------------- JPEG Lossy : call to IJG 6b
  
  // TODO : faire qq chose d'intelligent pour limiter 
  // la duplication du code JPEG <bits=8 / bits=12>
  // TODO : eplucher icelui pour traiter *egalement* bits=16
      int nBS;        
      if  ((nBS = GetBitsStored()) != 12) {
         printf("Sorry, Bits Stored = %d not yet taken into account\n",nBS);
         return false;
      }
      
      int res = gdcm_read_JPEG_file (destination);
         if (DEBUG) 
            printf ("res : %d\n",res);
      return res;
 
}   

/**
 * \ingroup gdcmFile
 * \brief   Allocates necessary memory, copies the pixel data
 *          (image[s]/volume[s]) to newly allocated zone.
 * @return  Pointer to newly allocated pixel data. 
 */
void * gdcmFile::GetImageData (void) {
   PixelData = (void *) malloc(lgrTotale);
   GetImageDataIntoVector(PixelData, lgrTotale);
   return(PixelData);
}

/**
 * \ingroup gdcmFile
 * \brief   Copies at most MaxSize bytes of pixel data to caller's
 *          memory space.
 * @param   destination Address (in caller's memory space) at which the
 *          pixel data should be copied
 * @param   MaxSize Maximum number of bytes to be copied. When MaxSize
 *          is not sufficient to hold the pixel data the copy is not
 *          executed (i.e. no partial copy).
 * @return  On success, the number of bytes actually copied. Zero on
 *          failure e.g. MaxSize is lower than necessary.
 */

size_t gdcmFile::GetImageDataIntoVector (void* destination, size_t MaxSize) {

   int nb, nbu, highBit, signe;
   string str_nbFrames, str_nb, str_nbu, str_highBit, str_signe;
 
   if ( lgrTotale > MaxSize ) {
      dbg.Verbose(0, "gdcmFile::GetImageDataIntoVector: pixel data bigger"
                     "than caller's expected MaxSize");
      return (size_t)0; 
   }
	
	(void)ReadPixelData(destination);
			
	// Nombre de Bits Alloues pour le stockage d'un Pixel
	str_nb = GetPubElValByNumber(0x0028,0x0100);
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
   if (nb != 8)
     SwapZone(destination, GetSwapCode(), lgrTotale, nb);
 
   // On remet les Bits des Octets dans le bon ordre si besoin est
   if (nbu != nb){
      int l = (int)lgrTotale / (nb/8);
      if (nb == 16) {
         guint16 mask = 0xffff;
         mask = mask >> (nb-nbu);
         guint16 *deb = (guint16 *)destination;
         for(int i = 0; i<l; i++) {
            *deb = (*deb >> (nbu-highBit-1)) & mask;
            deb ++;
         }
      } else if (nb == 32 ) {
         guint32 mask = 0xffffffff;
         mask = mask >> (nb-nbu);
         guint32 *deb = (guint32 *)destination;
         for(int i = 0; i<l; i++) {
            *deb = (*deb >> (nbu-highBit-1)) & mask;
            deb ++;
         }
      } else {
         dbg.Verbose(0, "gdcmFile::GetImageDataIntoVector: wierd image");
         return (size_t)0; 
      }
   }
   return lgrTotale; 
}


//
// Je laisse le code integral, au cas ça puisse etre reutilise ailleurs
//

void gdcmFile::SwapZone(void* im, int swap, int lgr, int nb) {
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
 * TODO : y a-t-il un inconvenient à fusioner ces 2 fonctions
 *
 * @param inData TODO JPR
 * @param ExpectedSize TODO JPR
 *
 * @return TODO JPR	
 */
int gdcmFile::SetImageData(void * inData, size_t ExpectedSize) {
   SetImageDataSize(ExpectedSize);
   PixelData = inData;
   lgrTotale = ExpectedSize;
   
   
   return(1);
}


/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief TODO JPR
 * \
 * \warning WARNING doit-etre etre publique ? FIXME JPR
 * TODO : y aurait il un inconvenient à fusionner ces 2 fonctions
 *
 * @param ImageDataSize TODO JPR
 *
 */

void gdcmFile::SetImageDataSize(size_t ImageDataSize) {

 	string content1;
 	char car[20];
 	
 	// suppose que le ElValue (0x7fe0, 0x0010) existe ...
 	
 	sprintf(car,"%d",ImageDataSize);
 
 	gdcmElValue*a = GetElValueByNumber(0x7fe0, 0x0010);
 	a->SetLength(ImageDataSize);
 		
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
	
	fwrite (PixelData,lgrTotale, 1, fp1);
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

int gdcmFile::WriteDcmImplVR (string nomFichier) {
   return WriteBase(nomFichier, ImplicitVR);
}

/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 *
 * @param  nomFichier TODO JPR
 *
 * @return TODO JPR
 */
 
int gdcmFile::WriteDcmImplVR (const char* nomFichier) {
   return WriteDcmImplVR (string (nomFichier));
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
   return WriteBase(nomFichier, ExplicitVR);
}
	
/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief  Ecrit au format ACR-NEMA sur disque l'entete et les pixels
 *        (a l'attention des logiciels cliniques 
 *        qui ne prennent en entrée QUE des images ACR ...
 * \warning si un header DICOM est fourni en entree,
 *        les groupes < 0x0008 et les groupes impairs sont ignores)
 * \warning Aucun test n'est fait sur l'"Endiannerie" du processeur.
 *        Ca fonctionnera correctement (?) sur processeur Intel
 *        (Equivalent a IdDcmWrite) 
 *
 * @param nomFichier TODO JPR
 *
 * @return TODO JPR	
 */

int gdcmFile::WriteAcr (string nomFichier) {
   return WriteBase(nomFichier, ACR);
}

/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 *
 * @param  nomFichier TODO JPR
 * @param  type TODO JPR
 *
 * @return TODO JPR
 */
int gdcmFile::WriteBase (string nomFichier, FileType type) {

   FILE * fp1;
   fp1 = fopen(nomFichier.c_str(),"wb");
   if (fp1 == NULL) {
      printf("Echec ouverture (ecriture) Fichier [%s] \n",nomFichier.c_str());
      return (0);
   }

   if ( (type == ImplicitVR) || (type == ExplicitVR) ) {
      char * filePreamble;
      // Ecriture Dicom File Preamble
      filePreamble=(char*)calloc(128,1);
      fwrite(filePreamble,128,1,fp1);
      fwrite("DICM",4,1,fp1);
   }

   gdcmHeader::Write(fp1, type);
   fwrite(PixelData, lgrTotale, 1, fp1);
   fclose (fp1);
   return(1);
}
