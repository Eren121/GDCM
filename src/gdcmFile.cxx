// gdcmFile.cxx

#include "gdcmFile.h"
#include "gdcmUtil.h"
#include "iddcmjpeg.h" // for the 'LibIDO' Jpeg LossLess

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
 
gdcmFile::gdcmFile(std::string & filename) 
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
   std::string str_nb;

   str_nb=gdcmHeader::GetPubElValByNumber(0x0028,0x0100);
   if (str_nb == GDCM_UNFOUND ) {
      nb = 16;
   } else {
      nb = atoi(str_nb.c_str() );
      if (nb == 12) nb =16;
   }
   lgrTotale =  GetXSize() *  GetYSize() *  GetZSize() * (nb/8)* GetSamplesPerPixel();
   std::string str_PhotometricInterpretation = gdcmHeader::GetPubElValByNumber(0x0028,0x0004);
   if ( str_PhotometricInterpretation == "PALETTE COLOR " ) { 
      lgrTotale*=3;
   }
}
   // see PS 3.3-2003 : C.7.6.3.2.1  
   // 
   //   MONOCHROME1
   //   MONOCHROME2
   //   PALETTE COLOR
   //   RGB
   //   HSV  (Retired)
   //   ARGB (Retired)
   //   CMYK (Retired)
   //   YBR_FULL
   //   YBR_FULL_422 (no LUT, no Palette)
   //   YBR_PARTIAL_422
   //   YBR_ICT
   //   YBR_RCT

  // LUT's
  // ex : gdcm-US-ALOKA-16.dcm
  // 0028|1221 [OW]   [Segmented Red Palette Color Lookup Table Data]
  // 0028|1222 [OW]   [Segmented Green Palette Color Lookup Table Data]  
  // 0028|1223 [OW]   [Segmented Blue Palette Color Lookup Table Data]

  // ex : US-PAL-8-10x-echo.dcm, 8BitsRunLengthColor.dcm
  // 0028|1201 [OW]   [Red Palette Color Lookup Table Data]
  // 0028|1202 [OW]   [Green Palette Color Lookup Table Data]
  // 0028|1203 [OW]   [Blue Palette Color Lookup Table Data]

  // ex  : OT-PAL-8-face.dcm
  // 0028|1201 [US]   [Red Palette Color Lookup Table Data]
  // 0028|1202 [US]   [Green Palette Color Lookup Table Data]
  // 0028|1203 [US]   [Blue Palette Color Lookup Table Data]



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
 * @param   destination where the pixel data should be stored.
 *
 */
bool gdcmFile::ReadPixelData(void* destination) {

   if ( !OpenFile())
      return false;
      
    if ( fseek(fp, GetPixelOffset(), SEEK_SET) == -1 ) {
      CloseFile();
      return false;
   }     

// -------------------------  Uncompressed File
    
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
    
 // ------------------------  Compressed File .
       
      int nb;
      std::string str_nb=gdcmHeader::GetPubElValByNumber(0x0028,0x0100);
      if (str_nb == GDCM_UNFOUND ) {
         nb = 16;
      } else {
         nb = atoi(str_nb.c_str() );
         if (nb == 12) nb =16;
      }
      int nBytes= nb/8;
      
      int taille = GetXSize() *  GetYSize()  * GetSamplesPerPixel(); 
          
                
  // ------------------------------- JPEG LossLess : call to Jpeg Libido
   
   if (IsJPEGLossless() /*&& GetZSize() == 1*/) {
   
      int ln; //  Position on begining of Jpeg Pixels
      fseek(fp,4,SEEK_CUR);  // skipping (fffe,e000) : Basic Offset Table Item
      fread(&ln,4,1,fp); 
      if(GetSwapCode()) 
         ln=SwapLong(ln);    // Item length
      fseek(fp,ln,SEEK_CUR); // skipping Basic Offset Table ('ln' bytes) 
      fseek(fp,4,SEEK_CUR);  // skipping (fffe,e000) : First fragment Item Tag
      fread(&ln,4,1,fp);     // First fragment length (just to know)
      if(GetSwapCode()) 
         ln=SwapLong(ln);      
 
      ClbJpeg* jpg = _IdDcmJpegRead(fp); // TODO : find a 'full' one.
                                         // (We use the LibIDO one :-(
      if(jpg == NULL) {
         CloseFile();
         return false;
      }      
      int * dataJpg = jpg->DataImg;
      
      switch (nBytes) {   
         case 1:
         {
            unsigned short *dest = (unsigned short *)destination;
            for (int i=0; i<taille; i++) {
               *((unsigned char *)dest+i) = *(dataJpg +i);   
            }
         }
         break;        
         
         case 2:
         {
            unsigned short *dest = (unsigned short *)destination;
            for (int i=0; i<taille; i++) {           
               *((unsigned short *)dest+i) = *(dataJpg +i);    
            }
         }
         break;       
     }
      _IdDcmJpegFree (jpg);
      return true;
   } 
 
  // ------------------------------- RLE

      if (gdcmHeader::IsRLELossLessTransferSyntax()) {
            int res = (bool)gdcm_read_RLE_file (destination);
            return res; 
      }

  // ------------------------------- JPEG Lossy : call to IJG 6b
    
      long fragmentBegining; // for ftell, fseek
      bool b = gdcmHeader::IsJPEG2000();
       
      bool res;
      guint16 ItemTagGr,ItemTagEl;
      int ln;  //  Position on begining of Jpeg Pixels
      
      fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
      fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
      if(GetSwapCode()) {
         ItemTagGr=SwapShort(ItemTagGr); 
         ItemTagEl=SwapShort(ItemTagEl);            
      }
      fread(&ln,4,1,fp); 
      if(GetSwapCode()) 
         ln=SwapLong(ln);    // Basic Offset Table Item length
         
      if (ln != 0) {
         // What is it used for ?!?
         char *BasicOffsetTableItemValue = (char *)malloc(ln+1);        
         fread(BasicOffsetTableItemValue,ln,1,fp); 
      }
      
      // first Fragment initialisation
      fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
      fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
      if(GetSwapCode()) {
         ItemTagGr=SwapShort(ItemTagGr); 
         ItemTagEl=SwapShort(ItemTagEl);            
      }
              
      // parsing fragments until Sequence Delim. Tag found
       //unsigned short *dest = (unsigned short *)destination;
         
      while (  ( ItemTagGr == 0xfffe) && (ItemTagEl != 0xe0dd) ) {
         fread(&ln,4,1,fp); 
         if(GetSwapCode()) 
            ln=SwapLong(ln);    // Fragment Item length
      
         // FIXME : multi fragments 
         fragmentBegining=ftell(fp);
                       
 
         if (b)
            res = (bool)gdcm_read_JPEG2000_file (destination);  // Reading Fragment pixels 
            
         else if (IsJPEGLossless()) {  
                  // ------------- call to LibIDO Jpeg for each Frame/fragment
                  
                  // Warning : Works only if there is one fragment per frame
                  //           (Or a single fragment for the multiframe file)
            ClbJpeg* jpg = _IdDcmJpegRead(fp); // TODO : find a 'full' one.
                                               // (We use the LibIDO one :-(
            if(jpg == NULL) {
               CloseFile();
               return false;
            }      
            int * dataJpg = jpg->DataImg;
            unsigned short *dest = (unsigned short *)destination;
            switch (nBytes) {   
               case 1:
               {
                  for (int i=0; i<taille; i++) {
                     *((unsigned char *)dest+i) = *(dataJpg +i);   
                  }
               break;
               }        
         
               case 2:
               {
                  for (int i=0; i<taille; i++) {        
                     *((unsigned short *)dest+i) = *(dataJpg +i);    
                  }
               break;
               }       
           } 
           _IdDcmJpegFree (jpg);
     
         } // ------------------------------------- endif (IsJPEGLossless())
                  
         else
            if  (GetBitsStored() == 8) {
            	res = (bool)gdcm_read_JPEG_file (destination);  // Reading Fragment pixels         
            } else {
            	res = (bool)gdcm_read_JPEG_file12 (destination);// Reading Fragment pixels  
            }       
            
         if (!res) break;
         
         // FIXME : will work only when each fragment corresponds to a Frame :-(
         
         destination = (char *)destination + taille * nBytes; // location in user's memory 
                                                  // for next fragment (if any) 
         // TODO : find a suitable file (multifragment/single Frame Jpeg file) to check
         
         fseek(fp,fragmentBegining,SEEK_SET); // To be sure we start 
         fseek(fp,ln,SEEK_CUR);               // at the begining of next fragment
         
         ItemTagGr = ItemTagEl =0;
         fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
         fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
         if(GetSwapCode()) {
            ItemTagGr=SwapShort(ItemTagGr); 
            ItemTagEl=SwapShort(ItemTagEl);            
         } 
         
      //(char *) destination += taille * nBytes;
      //std::cout << "destination" << destination << std::endl;
      }
                
      return res;
}   

/**
 * \ingroup gdcmFile
 * \brief   Allocates necessary memory, copies the pixel data
 *          (image[s]/volume[s]) to newly allocated zone.
 * @return  Pointer to newly allocated pixel data.
 * \        NULL if alloc fails 
 */
void * gdcmFile::GetImageData (void) {
   PixelData = (void *) malloc(lgrTotale);
   if (PixelData)
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
   std::string str_nbFrames, str_nb, str_nbu, str_highBit, str_signe;
 
   if ( lgrTotale > MaxSize ) {
      dbg.Verbose(0, "gdcmFile::GetImageDataIntoVector: pixel data bigger"
                     "than caller's expected MaxSize");
      return (size_t)0; 
   }
	
   (void)ReadPixelData(destination);
			
	// Nombre de Bits Alloues pour le stockage d'un Pixel
   str_nb = GetPubElValByNumber(0x0028,0x0100);
   if (str_nb == GDCM_UNFOUND ) {
      nb = 16;
   } else {
      nb = atoi(str_nb.c_str() );
   }
	
	// Nombre de Bits Utilises
   str_nbu=GetPubElValByNumber(0x0028,0x0101);
   if (str_nbu == GDCM_UNFOUND ) {
      nbu = nb;
   } else {
      nbu = atoi(str_nbu.c_str() );
   }	
	
	// Position du Bit de Poids Fort
   str_highBit=GetPubElValByNumber(0x0028,0x0102);
   if (str_highBit == GDCM_UNFOUND ) {
      highBit = nb - 1;
   } else {
      highBit = atoi(str_highBit.c_str() );
   }
		
	// Signe des Pixels 
   str_signe=GetPubElValByNumber(0x0028,0x0103);
   if (str_signe == GDCM_UNFOUND ) {
      signe = 1;
   } else {
      signe = atoi(str_signe.c_str() );
   }

   // re arange bytes inside the integer
   if (nb != 8)
     SwapZone(destination, GetSwapCode(), lgrTotale, nb);
 
   // re arange bits inside the bytes
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

// Just to 'see' was was actually read on disk :-(
// Some troubles expected

FILE *  fpSpurious;
fpSpurious=fopen("SpuriousFile.raw","w"); 
fwrite(destination,lgrTotale, 1,fpSpurious);
fclose(fpSpurious);


   // *Try* to deal with the color
   // ----------------------------

   // Planar configuration = 0 : Pixels are already RGB
   // Planar configuration = 1 : 3 planes : R, G, B
   // Planar configuration = 2 : 1 gray Plane + 3 LUT

   // Well ... supposed to be !
   // See US-PAL-8-10x-echo.dcm: PlanarConfiguration=0,PhotometricInterpretation=PALETTE COLOR
   // and heuristic has to be found :-( 

      std::string str_PhotometricInterpretation = gdcmHeader::GetPubElValByNumber(0x0028,0x0004);
   
      if ( (str_PhotometricInterpretation == "MONOCHROME1 ") 
        || (str_PhotometricInterpretation == "MONOCHROME2 ") 
        || (str_PhotometricInterpretation == "RGB")) {
         return lgrTotale; 
      }
      int planConf=GetPlanarConfiguration();
 
      switch (planConf) {
      case 0:                              
         //       Pixels are already RGB
         break;
    
      case 1:

         {
         if (str_PhotometricInterpretation == "YBR_FULL") { // Warning : YBR_FULL_422 acts as RGB (?!)

         //       need to make RGB Pixels from Planes Y,cB,cR
         // see http://lestourtereaux.free.fr/papers/data/yuvrgb.pdf
         // for code optimisation

            int l = GetXSize()*GetYSize();
            int nbFrames = GetZSize();

            unsigned char * newDest = (unsigned char*) malloc(lgrTotale);
            unsigned char *x  = newDest;
            unsigned char * a = (unsigned char *)destination;
            unsigned char * b = a + l;
            unsigned char * c = b + l;

            double R,G,B;

            // TODO : Replace by the 'well known' 
            //        integer computation counterpart
            for (int i=0;i<nbFrames;i++) {
               for (int j=0;j<l; j++) {
                  R= 1.164 *( *a-16) + 1.596 *( *c -128) + 0.5;
                  G= 1.164 *( *a-16) - 0.813 *( *c -128) - 0.392 *(*b -128) + 0.5;
                  B= 1.164 *( *a-16) + 2.017 *( *b -128) + 0.5;

                  if (R<0.0)   R=0.0;
                  if (G<0.0)   G=0.0;
                  if (B<0.0)   B=0.0;
                  if (R>255.0) R=255.0;
                  if (G>255.0) G=255.0;
                  if (B>255.0) B=255.0;

                  *(x++) = (unsigned char)R;
                  *(x++) = (unsigned char)G;
                  *(x++) = (unsigned char)B;
                  a++; b++; c++;  
               }
           }
            memmove(destination,newDest,lgrTotale);
            free(newDest);

        } else {
         
         //       need to make RGB Pixels from Planes R,G,B

            int l = GetXSize()*GetYSize();
            int nbFrames = GetZSize();

            char * newDest = (char*) malloc(lgrTotale);
            char *x  = newDest;
            char * a = (char *)destination;
            char * b = a + l;
            char * c = b + l;

               // TODO :
               // any trick not to have to allocate temporary buffer is welcome ...

            for (int i=0;i<nbFrames;i++) {
               for (int j=0;j<l; j++) {
                  *(x++) = *(a++);
                  *(x++) = *(b++);
                  *(x++) = *(c++);  
               }
            }
            memmove(destination,newDest,lgrTotale);
            free(newDest);
        }

            // now, it's an RGB image
            // Lets's write it in the Header
         std::string spp = "3";
         gdcmHeader::SetPubElValByNumber(spp,0x0028,0x0002);
         std::string rgb="RGB";
         gdcmHeader::SetPubElValByNumber(rgb,0x0028,0x0004);
         break;
       }
     
       case 2:                      
         //       from Lut R + Lut G + Lut B
       
         // we no longer use gdcmHeader::GetLUTRGB
         // since a lot of images have strange info 
         // in the Lookup Table Descriptors (0028,1101),...
         {
         unsigned char * newDest = (unsigned char*) malloc(lgrTotale);
         unsigned char * a = (unsigned char *)destination;

         unsigned char *lutR =(unsigned char *)GetPubElValVoidAreaByNumber(0x0028,0x1201);
         unsigned char *lutG =(unsigned char *)GetPubElValVoidAreaByNumber(0x0028,0x1202);
         unsigned char *lutB =(unsigned char *)GetPubElValVoidAreaByNumber(0x0028,0x1203);

         if (lutR && lutG && lutB ) { // need to make RGB Pixels 
                                      // from grey Pixels 
                                      // and Lut R,Lut G,Lut B

            unsigned char * newDest = (unsigned char*) malloc(lgrTotale);
            int l = lgrTotale/3;
            memmove(newDest, destination, l);// move Gray pixels to temp area

            unsigned char * x = newDest;

               int j;
               // See PS 3.3-2003 C.11.1.1.2 p 619
               // 
               int mult;
               if ( GetLUTNbits()==16 && nb==8) mult=2; // See PS 3.3 
               else mult=1;
       
               for (int i=0;i<l; i++) {
                  j=newDest[i]*mult;
                  *a++ = lutR[j]; 
                  *a++ = lutG[j];
                  *a++ = lutB[j];
               }

               free(newDest);
        
               // now, it's an RGB image      
           std::string spp = "3";
           gdcmHeader::SetPubElValByNumber(spp,0x0028,0x0002); 
           std::string rgb="RGB";
           gdcmHeader::SetPubElValByNumber(rgb,0x0028,0x0004);
               
         } else { // need to make RGB Pixels (?)
                  // from grey Pixels (?!)
                  // and Gray Lut  (!?!) 
               unsigned char *lutGray =(unsigned char *)GetPubElValVoidAreaByNumber(0x0028,0x1200);
                    // Well . I'll wait till I find such an image 
         }
         break;
         }
   } 
    
   return lgrTotale; 
}

//
// Je laisse le code integral, au cas ça puisse etre reutilise ailleurs
//

/**
 * \ingroup gdcmFile
 * \brief   Swap the bytes, according to swap code.
 * \warning not end user intended
 * @param   im area to deal with
 * @param   swap swap code
 * @param   lgr Area Length
 * @param   nb Pixels Bit number 
 */

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
         printf("valeur de SWAP (16 bits) not allowed : %d\n", swap);
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
         printf(" SWAP value (32 bits) not allowed : %d\n", swap);
   } 
return;
}

/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief TODO JPR
 * \warning doit-etre etre publique ?  
 * TODO : y a-t-il un inconvenient à fusioner ces 2 fonctions
 *
 * @param inData 
 * @param ExpectedSize 
 *
 * @return integer acts as a boolean	
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
 * \brief Sets the Pixel Area size in the Header
 *        --> not-for-rats function
 * 
 * \warning WARNING doit-etre etre publique ? 
 * TODO : y aurait il un inconvenient à fusionner ces 2 fonctions
 *
 * @param ImageDataSize new Pixel Area Size
 *        warning : nothing else is checked
 */

void gdcmFile::SetImageDataSize(size_t ImageDataSize) {
   std::string content1;
   char car[20];	
   // Assumes ElValue (0x7fe0, 0x0010) exists ...	
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
 *        (Equivalent a IdImaWriteRawFile) 
 *
 * @param fileName 
 * @return	
 */

int gdcmFile::WriteRawData (std::string fileName) {
   FILE * fp1;
   fp1 = fopen(fileName.c_str(),"wb");
   if (fp1 == NULL) {
      printf("Echec ouverture (ecriture) Fichier [%s] \n",fileName.c_str());
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
 *         (Equivalent a IdDcmWrite) 
 *
 * @param fileName 
 * @return int acts as a boolean
 */

int gdcmFile::WriteDcmImplVR (std::string fileName) {
   return WriteBase(fileName, ImplicitVR);
}

/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief  
 * @param  fileName 
 * @return int acts as a boolean
 */
 
int gdcmFile::WriteDcmImplVR (const char* fileName) {
   return WriteDcmImplVR (std::string (fileName));
}
	
/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 * \brief  
 * @param  fileName
 * @return int acts as a boolean
 */

int gdcmFile::WriteDcmExplVR (std::string fileName) {
   return WriteBase(fileName, ExplicitVR);
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
 * @param fileName
 * @return int acts as a boolean	
 */

int gdcmFile::WriteAcr (std::string fileName) {
   return WriteBase(fileName, ACR);
}

/////////////////////////////////////////////////////////////////
/**
 * \ingroup   gdcmFile
 *
 * @param  FileName
 * @param  type 
 *
 * @return int acts as a boolean
 */
int gdcmFile::WriteBase (std::string FileName, FileType type) {

   FILE * fp1;
   fp1 = fopen(FileName.c_str(),"wb");
   if (fp1 == NULL) {
      printf("Echec ouverture (ecriture) Fichier [%s] \n",FileName.c_str());
      return (0);
   }

   if ( (type == ImplicitVR) || (type == ExplicitVR) ) {
      char * filePreamble;
      // writing Dicom File Preamble
      filePreamble=(char*)calloc(128,1);
      fwrite(filePreamble,128,1,fp1);
      fwrite("DICM",4,1,fp1);
   }

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to re-write ACR-LibIDO formated images
   //
   // if recognition code tells us we dealt with a LibIDO image
   // we reproduce on disk the switch between lineNumber and columnNumber
   // just before writting ...

   std::string rows, columns; 
   if ( filetype == ACR_LIBIDO){
         rows    = GetPubElValByNumber(0x0028, 0x0010);
         columns = GetPubElValByNumber(0x0028, 0x0011);
         SetPubElValByNumber(columns,  0x0028, 0x0010);
         SetPubElValByNumber(rows   ,  0x0028, 0x0011);
   }	
   // ----------------- End of Special Patch ----------------

   gdcmHeader::Write(fp1, type);

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to re-write ACR-LibIDO formated images
   // 
   // ...and we restore the Header to be Dicom Compliant again 
   // just after writting

   if (filetype == ACR_LIBIDO){
         SetPubElValByNumber(rows   , 0x0028, 0x0010);
         SetPubElValByNumber(columns, 0x0028, 0x0011);
   }	
   // ----------------- End of Special Patch ----------------

   fwrite(PixelData, lgrTotale, 1, fp1);
   fclose (fp1);
   return(1);
}
