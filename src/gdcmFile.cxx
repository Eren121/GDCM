// gdcmFile.cxx
//-----------------------------------------------------------------------------
#include "gdcmFile.h"
#include "gdcmUtil.h"
#include "jpeg/ljpg/jpegless.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
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
 * @param header file to be opened for reading datas
 *
 * @return	
 */
 
gdcmFile::gdcmFile(gdcmHeader *header) {
   Header=header;
   SelfHeader=false;

   if (Header->IsReadable())
      SetPixelDataSizeFromHeader();
}

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
 * @param filename file to be opened for parsing
 * @return	
 */
gdcmFile::gdcmFile(std::string & filename) {
   Header=new gdcmHeader(filename.c_str());
   SelfHeader=true;

   if (Header->IsReadable())
      SetPixelDataSizeFromHeader();
}

gdcmFile::gdcmFile(const char * filename) {
   Header=new gdcmHeader(filename);
   SelfHeader=true;

   if (Header->IsReadable())
      SetPixelDataSizeFromHeader();
}

/**
 * \ingroup   gdcmFile
 * \brief canonical destructor
 * \Note  If the gdcmHeader is created by the gdcmFile, it is destroyed
 *        by the gdcmFile
 */
gdcmFile::~gdcmFile(void) {
   if(SelfHeader)
      delete Header;
   Header=NULL;
}

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup   gdcmFile
 * \brief     
 * @return	
 */
gdcmHeader *gdcmFile::GetHeader(void) {
   return(Header);
}

/**
 * \ingroup   gdcmFile
 * \brief     computes the length (in bytes) to ALLOCATE to receive the
 *            image(s) pixels (multiframes taken into account) 		
 * \warning : it is NOT the group 7FE0 length
 *          (no interest for compressed images).
 * @return length to allocate
 */
void gdcmFile::SetPixelDataSizeFromHeader(void) {
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

   // ex  : OT-PAL-8-face.dcm
   // 0028|1201 [US]   [Red Palette Color Lookup Table Data]
   // 0028|1202 [US]   [Green Palette Color Lookup Table Data]
   // 0028|1203 [US]   [Blue Palette Color Lookup Table Data]

   int nb;
   std::string str_nb;
   str_nb=Header->GetEntryByNumber(0x0028,0x0100);
   if (str_nb == GDCM_UNFOUND ) {
      nb = 16;
   } else {
      nb = atoi(str_nb.c_str() );
      if (nb == 12) nb =16;
   }
   lgrTotale =  lgrTotaleRaw = Header->GetXSize() * Header->GetYSize() 
              * Header->GetZSize() * (nb/8)* Header->GetSamplesPerPixel();
   std::string str_PhotometricInterpretation = 
                             Header->GetEntryByNumber(0x0028,0x0004);
			     
   /*if ( str_PhotometricInterpretation == "PALETTE COLOR " )*/
   // pb when undealt Segmented Palette Color
   
    if (Header->HasLUT()) { 
      lgrTotale*=3;
   }
}

/**
 * \ingroup   gdcmFile
 * \brief     Returns the size (in bytes) of required memory to hold
 *            the pixel data represented in this file.
 * @return    The size of pixel data in bytes.
 */
size_t gdcmFile::GetImageDataSize(void) {
   return (lgrTotale);
}

/**
 * \ingroup   gdcmFile
 * \brief     Returns the size (in bytes) of required memory to hold
 * \          the pixel data represented in this file, when user DOESN'T want 
 * \          to get RGB pixels image when it's stored as a PALETTE COLOR image
 * \          - the (vtk) user is supposed to know how deal with LUTs - 
 * \warning   to be used with GetImagePixelsRaw()
 * @return    The size of pixel data in bytes.
 */
size_t gdcmFile::GetImageDataSizeRaw(void) {
   return (lgrTotaleRaw);
}

/**
 * \ingroup gdcmFile
 * \brief   Allocates necessary memory, copies the pixel data
 *          (image[s]/volume[s]) to newly allocated zone.
 *          Transforms YBR pixels into RGB pixels if any
 *          Transforms 3 planes R, G, B into a single RGB Plane
 *          Transforms single Grey plane + 3 Palettes into a RGB Plane
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
 * \warning This function was designed to avoid people that want to build
 *          a volume from an image stack to need first to get the image pixels 
 *          and then move them to the volume area.
 *          It's absolutely useless for any VTK user since vtk chooses 
 *          to invert the lines of an image, that is the last line comes first
 *          (for some axis related reasons?). Hence he will have 
 *          to load the image line by line, starting from the end.
 *          VTK users have to call GetImageData
 *     
 * @param   destination Address (in caller's memory space) at which the
 *          pixel data should be copied
 * @param   MaxSize Maximum number of bytes to be copied. When MaxSize
 *          is not sufficient to hold the pixel data the copy is not
 *          executed (i.e. no partial copy).
 * @return  On success, the number of bytes actually copied. Zero on
 *          failure e.g. MaxSize is lower than necessary.
 */
size_t gdcmFile::GetImageDataIntoVector (void* destination, size_t MaxSize) {
   size_t l = GetImageDataIntoVectorRaw (destination, MaxSize);
   
   if (!Header->HasLUT())
      return lgrTotale; 
                            
   // from Lut R + Lut G + Lut B
   unsigned char * newDest = (unsigned char *)malloc(lgrTotale);
   unsigned char * a       = (unsigned char *)destination;	 
   unsigned char * lutRGBA =                  Header->GetLUTRGBA();
   if (lutRGBA) { 	    
      int l = lgrTotaleRaw;
      memmove(newDest, destination, l);// move Gray pixels to temp area	    
      int j;	 
      for (int i=0;i<l; i++) {         // Build RGB Pixels
         j=newDest[i]*4;
         *a++ = lutRGBA[j]; 
         *a++ = lutRGBA[j+1];
         *a++ = lutRGBA[j+2];
      }
      free(newDest);
    
   // now, it's an RGB image
   // Lets's write it in the Header

         // CreateOrReplaceIfExist ?
	 
   std::string spp = "3";        // Samples Per Pixel
   Header->SetEntryByNumber(spp,0x0028,0x0002);
   std::string rgb= "RGB ";      // Photometric Interpretation
   Header->SetEntryByNumber(rgb,0x0028,0x0004);
   std::string planConfig = "0"; // Planar Configuration
   Header->SetEntryByNumber(planConfig,0x0028,0x0006);

   } else { 
	     // need to make RGB Pixels (?)
             //    from grey Pixels (?!)
             //     and Gray Lut  (!?!) 
	     //    or Segmented xxx Palette Color Lookup Table Data and so on
		  		  		  
         // Oops! I get one (gdcm-US-ALOKA-16.dcm)
         // No idea how to manage such an image 
         // It seems that *no Dicom Viewer* has any idea :-(
         // Segmented xxx Palette Color are *more* than 65535 long ?!?
		   
      std::string rgb= "MONOCHROME1 ";      // Photometric Interpretation
      Header->SetEntryByNumber(rgb,0x0028,0x0004);		   		   
   }      	 
   // TODO : Drop Palette Color out of the Header? 	     
   return lgrTotale; 
}

/**
 * \ingroup gdcmFile
 * \brief   Allocates necessary memory, copies the pixel data
 *          (image[s]/volume[s]) to newly allocated zone.
 *          Transforms YBR pixels into RGB pixels if any
 *          Transforms 3 planes R, G, B into a single RGB Plane
 *	    DOES NOT transform Grey plane + 3 Palettes into a RGB Plane
 * @return  Pointer to newly allocated pixel data.
 * \        NULL if alloc fails 
 */
void * gdcmFile::GetImageDataRaw (void) {
   if (Header->HasLUT())
      lgrTotale /= 3;  // TODO Let gdcmHeadar user a chance 
                       // to get the right value
		       // Create a member lgrTotaleRaw ???
   PixelData = (void *) malloc(lgrTotale);
   if (PixelData)
      GetImageDataIntoVectorRaw(PixelData, lgrTotale);
   return(PixelData);
}

/**
 * \ingroup gdcmFile
 * \brief   Copies at most MaxSize bytes of pixel data to caller's
 *          memory space.
 * \warning This function was designed to avoid people that want to build
 *          a volume from an image stack to need first to get the image pixels 
 *          and then move them to the volume area.
 *          It's absolutely useless for any VTK user since vtk chooses 
 *          to invert the lines of an image, that is the last line comes first
 *          (for some axis related reasons?). Hence he will have 
 *          to load the image line by line, starting from the end.
 *          VTK users hace to call GetImageData
 * \warning DOES NOT transform the Grey Plane + Palette Color (if any) 
 *                   into a single RGB Pixels Plane
 *          the (VTK) user will manage the palettes
 *     
 * @param   destination Address (in caller's memory space) at which the
 *          pixel data should be copied
 * @param   MaxSize Maximum number of bytes to be copied. When MaxSize
 *          is not sufficient to hold the pixel data the copy is not
 *          executed (i.e. no partial copy).
 * @return  On success, the number of bytes actually copied. Zero on
 *          failure e.g. MaxSize is lower than necessary.
 */
size_t gdcmFile::GetImageDataIntoVectorRaw (void* destination, size_t MaxSize) {

   int nb, nbu, highBit, signe;
   std::string str_nbFrames, str_nb, str_nbu, str_highBit, str_signe;
 
   if ( lgrTotale > MaxSize ) {
      dbg.Verbose(0, "gdcmFile::GetImageDataIntoVector: pixel data bigger"
                     "than caller's expected MaxSize");
      return (size_t)0; 
   }
	
   (void)ReadPixelData(destination);
			
	// Number of Bits Allocated for storing a Pixel
   str_nb = Header->GetEntryByNumber(0x0028,0x0100);
   if (str_nb == GDCM_UNFOUND ) {
      nb = 16;
   } else {
      nb = atoi(str_nb.c_str() );
   }	
	// Number of Bits actually used
   str_nbu=Header->GetEntryByNumber(0x0028,0x0101);
   if (str_nbu == GDCM_UNFOUND ) {
      nbu = nb;
   } else {
      nbu = atoi(str_nbu.c_str() );
   }		
	// High Bit Position
   str_highBit=Header->GetEntryByNumber(0x0028,0x0102);
   if (str_highBit == GDCM_UNFOUND ) {
      highBit = nb - 1;
   } else {
      highBit = atoi(str_highBit.c_str() );
   }		
	// Pixel sign
	// 0 = Unsigned
	// 1 = Signed
   str_signe=Header->GetEntryByNumber(0x0028,0x0103);
   if (str_signe == GDCM_UNFOUND ) {
      signe = 0;  // default is unsigned
   } else {
      signe = atoi(str_signe.c_str() );
   }

   // re arange bytes inside the integer (processor endianity)
   if (nb != 8)
     SwapZone(destination, Header->GetSwapCode(), lgrTotale, nb);
     
   // to avoid pb with some xmedcon breakers images 
   if (nb==16 && nbu<nb && signe==0) {
     int l = (int)lgrTotale / (nb/8);
     guint16 *deb = (guint16 *)destination;
     for(int i = 0; i<l; i++) {
        if(*deb == 0xffff) 
	   *deb=0; 
	   deb++;   
         }
    }

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
// DO NOT remove this code commented out.
// Nobody knows what's expecting you ...
// Just to 'see' what was actually read on disk :-(
//   FILE * f2;
//   f2 = fopen("SpuriousFile.raw","wb");
//   fwrite(destination,lgrTotale,1,f2);
//   fclose(f2);

   // Deal with the color
   // -------------------
   
       std::string str_PhotometricInterpretation = 
                 Header->GetEntryByNumber(0x0028,0x0004);
		   
      if ( (str_PhotometricInterpretation == "MONOCHROME1 ") 
        || (str_PhotometricInterpretation == "MONOCHROME2 ") ) {
         return lgrTotale; 
      }
      
   // Planar configuration = 0 : Pixels are already RGB
   // Planar configuration = 1 : 3 planes : R, G, B
   // Planar configuration = 2 : 1 gray Plane + 3 LUT

   // Well ... supposed to be !
   // See US-PAL-8-10x-echo.dcm: PlanarConfiguration=0,
   //                            PhotometricInterpretation=PALETTE COLOR
   // and heuristic has to be found :-( 

      int planConf=Header->GetPlanarConfiguration();  // 0028,0006

      // Whatever Planar Configuration is, 
      // "PALETTE COLOR " implies that we deal with the palette. 
      if (str_PhotometricInterpretation == "PALETTE COLOR ")
         planConf=2;

      switch (planConf) {
      case 0:                              
         //       Pixels are already RGB
         break;
    
      case 1:

         {
         if (str_PhotometricInterpretation == "YBR_FULL") { 
	 
   // Warning : YBR_FULL_422 acts as RGB
   //         : we need to make RGB Pixels from Planes Y,cB,cR
	 
	 // to see the tricks about YBR_FULL, YBR_FULL_422, 
         // YBR_PARTIAL_422, YBR_ICT, YBR_RCT have a look at :
	 //   ftp://medical.nema.org/medical/dicom/final/sup61_ft.pdf
	 // and be *very* affraid
	 //
            int l = Header->GetXSize()*Header->GetYSize();
            int nbFrames = Header->GetZSize();

            unsigned char * newDest = (unsigned char*) malloc(lgrTotale);
            unsigned char *x  = newDest;
            unsigned char * a = (unsigned char *)destination;
            unsigned char * b = a + l;
            unsigned char * c = b + l;
            double R,G,B;

            // TODO : Replace by the 'well known' 
            //        integer computation counterpart
	    // see http://lestourtereaux.free.fr/papers/data/yuvrgb.pdf
            // for code optimisation
	    
            for (int i=0;i<nbFrames;i++) {
               for (int j=0;j<l; j++) {
                  R= 1.164 *(*a-16) + 1.596 *(*c -128) + 0.5;
                  G= 1.164 *(*a-16) - 0.813 *(*c -128) - 0.392 *(*b -128) + 0.5;
                  B= 1.164 *(*a-16) + 2.017 *(*b -128) + 0.5;

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
         
         //       need to make RGB Pixels from R,G,B Planes
         //       (all the Frames at a time)

            int l = Header->GetXSize()*Header->GetYSize()*Header->GetZSize();

            char * newDest = (char*) malloc(lgrTotale);
            char * x = newDest;
            char * a = (char *)destination;
            char * b = a + l;
            char * c = b + l;

            for (int j=0;j<l; j++) {
               *(x++) = *(a++);
               *(x++) = *(b++);
               *(x++) = *(c++);  
            }           
            memmove(destination,newDest,lgrTotale);
            free(newDest);
        }	  
         break;
       }     
       case 2:                      
         //       Palettes were found
         //       Let the user deal with them !
         return lgrTotale;        
   } 
   // now, it's an RGB image
   // Lets's write it in the Header

   // CreateOrReplaceIfExist ?

   std::string spp = "3";        // Samples Per Pixel
   Header->SetEntryByNumber(spp,0x0028,0x0002);
   std::string rgb="RGB ";   // Photometric Interpretation
   Header->SetEntryByNumber(rgb,0x0028,0x0004);

   std::string planConfig = "0"; // Planar Configuration
   Header->SetEntryByNumber(planConfig,0x0028,0x0006);
	 
	 // TODO : Drop Palette Color out of the Header? 
   return lgrTotale; 
}

/**
 * \ingroup   gdcmFile
 * \brief TODO JPR
 * \warning doit-etre etre publique ?  
 * TODO : y a-t-il un inconvenient à fusioner ces 2 fonctions
 *
 * @param inData 
 * @param ExpectedSize 
 *
 * @return boolean	
 */
bool gdcmFile::SetImageData(void * inData, size_t ExpectedSize) {
   Header->SetImageDataSize(ExpectedSize);
   PixelData = inData;
   lgrTotale = ExpectedSize;
   return(true);
}

/**
 * \ingroup   gdcmFile
 * \brief Ecrit sur disque les pixels d'UNE image
 *        Aucun test n'est fait sur l'"Endiannerie" du processeur.
 *        Ca sera à l'utilisateur d'appeler son Reader correctement
 *        (Equivalent a IdImaWriteRawFile) 
 *
 * @param fileName name of the file to be created
 *                 (any already existing file is over written)
 * @return false if write fails	
 */

bool gdcmFile::WriteRawData (std::string fileName) {
   FILE * fp1;
   fp1 = fopen(fileName.c_str(),"wb");
   if (fp1 == NULL) {
      printf("Echec ouverture (ecriture) Fichier [%s] \n",fileName.c_str());
      return (false);
   } 	
   fwrite (PixelData,lgrTotale, 1, fp1);
   fclose (fp1);
   return(true);
}

/**
 * \ingroup   gdcmFile
 * \brief Ecrit sur disque UNE image Dicom
 *        Aucun test n'est fait sur l'"Endiannerie" du processeur.
 *         Ca fonctionnera correctement (?) sur processeur Intel
 *         (Equivalent a IdDcmWrite) 
 *
 * @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @return false if write fails	
 */

bool gdcmFile::WriteDcmImplVR (std::string fileName) {
   return WriteBase(fileName, ImplicitVR);
}

/**
 * \ingroup   gdcmFile
 * \brief  
 * @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @return false if write fails	
 */
 
bool gdcmFile::WriteDcmImplVR (const char* fileName) {
   return WriteDcmImplVR (std::string (fileName));
}
	
/**
 * \ingroup   gdcmFile
 * \brief  
 * @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @return false if write fails	
 */

bool gdcmFile::WriteDcmExplVR (std::string fileName) {
   return WriteBase(fileName, ExplicitVR);
}
	
/**
 * \ingroup   gdcmFile
 * \brief  Ecrit au format ACR-NEMA sur disque l'entete et les pixels
 *        (a l'attention des logiciels cliniques 
 *        qui ne prennent en entrée QUE des images ACR ...
 * \warning if a DICOM_V3 header is supplied,
 *         groups < 0x0008 and shadow groups are ignored)
 * \warning NO TEST is performed on processor "Endiannity".
 *        Ca fonctionnera correctement (?) sur processeur Intel
 *        (Equivalent a IdDcmWrite) 
 *
 * @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @return false if write fails		
 */

bool gdcmFile::WriteAcr (std::string fileName) {
   return WriteBase(fileName, ACR);
}

//-----------------------------------------------------------------------------
// Protected
/**
 * \ingroup   gdcmFile
 *
* @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @param  type file type (ExplicitVR, ImplicitVR, ...)
 * @return false if write fails		
 */
bool gdcmFile::WriteBase (std::string fileName, FileType type) {

   FILE * fp1;
   fp1 = fopen(fileName.c_str(),"wb");
   if (fp1 == NULL) {
      printf("Echec ouverture (ecriture) Fichier [%s] \n",fileName.c_str());
      return (false);
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
   if ( Header->GetFileType() == ACR_LIBIDO){
         rows    = Header->GetEntryByNumber(0x0028, 0x0010);
         columns = Header->GetEntryByNumber(0x0028, 0x0011);
         Header->SetEntryByNumber(columns,  0x0028, 0x0010);
         Header->SetEntryByNumber(rows   ,  0x0028, 0x0011);
   }	
   // ----------------- End of Special Patch ----------------

   Header->Write(fp1, type);

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to re-write ACR-LibIDO formated images
   // 
   // ...and we restore the Header to be Dicom Compliant again 
   // just after writting

   if (Header->GetFileType() == ACR_LIBIDO){
         Header->SetEntryByNumber(rows   , 0x0028, 0x0010);
         Header->SetEntryByNumber(columns, 0x0028, 0x0011);
   }	
   // ----------------- End of Special Patch ----------------

   fwrite(PixelData, lgrTotale, 1, fp1);
   fclose (fp1);
   return(true);
}

//-----------------------------------------------------------------------------
// Private
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
         printf("SWAP value (16 bits) not allowed : %d\n", swap);
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
         printf("SWAP value (32 bits) not allowed : %d\n", swap);
   } 
return;
}

/**
 * \ingroup gdcmFile
 * \brief   Read pixel data from disk (optionaly decompressing) into the
 *          caller specified memory location.
 * @param   destination where the pixel data should be stored.
 *
 */
bool gdcmFile::ReadPixelData(void* destination) {

   FILE *fp;

   if ( !(fp=Header->OpenFile()))
      return false;
      
   if ( fseek(fp, Header->GetPixelOffset(), SEEK_SET) == -1 ) {
      Header->CloseFile();
      return false;
   }
   

   // ----------------------  Compacted File (12 Bits Per Pixel)
   /* unpack 12 Bits pixels into 16 Bits pixels */
   /* 2 pixels 12bit =     [0xABCDEF]           */
   /* 2 pixels 16bit = [0x0ABD] + [0x0FCE]      */
   if (Header->GetBitsAllocated()==12) {
      int nbPixels = Header->GetXSize() * Header->GetYSize();
      unsigned char b0, b1, b2;
      
      unsigned short int* pdestination = (unsigned short int*)destination;    
      for(int p=0;p<nbPixels;p+=2) {
         fread(&b0,1,1,fp);
         fread(&b1,1,1,fp);
         fread(&b2,1,1,fp);      
         //Two steps is necessary to please VC++
         *pdestination++ =  ((b0 >> 4) << 8) + ((b0 & 0x0f) << 4) + (b1 & 0x0f);
                             /* A */          /* B */            /* D */
         *pdestination++ =  ((b2 & 0x0f) << 8) + ((b1 >> 4) << 4) + (b2 >> 4);
                             /* F */          /* C */            /* E */
		  
	// Troubles expected on Big-Endian processors ?	      
      }

      Header->CloseFile();
      return(true);
   }        

   // ----------------------  Uncompressed File
   if ( !Header->IsDicomV3()                             ||
        Header->IsImplicitVRLittleEndianTransferSyntax() ||
        Header->IsExplicitVRLittleEndianTransferSyntax() ||
        Header->IsExplicitVRBigEndianTransferSyntax()    ||
        Header->IsDeflatedExplicitVRLittleEndianTransferSyntax() ) {

      size_t ItemRead = fread(destination, Header->GetPixelAreaLength(), 1, fp);
      
      if ( ItemRead != 1 ) {
         Header->CloseFile();
         return false;
      } else {
         Header->CloseFile();
         return true;
      }
   } 

   // ---------------------- Run Length Encoding
   if (Header->IsRLELossLessTransferSyntax()) {
      bool res = (bool)gdcm_read_RLE_file (fp,destination);
      Header->CloseFile();
      return res; 
   }  
    
   // --------------- SingleFrame/Multiframe JPEG Lossless/Lossy/2000 
   int nb;
   std::string str_nb=Header->GetEntryByNumber(0x0028,0x0100);
   if (str_nb == GDCM_UNFOUND ) {
      nb = 16;
   } else {
      nb = atoi(str_nb.c_str() );
      if (nb == 12) nb =16;  // ?? 12 should be ACR-NEMA only ?
   }

   int nBytes= nb/8;
   
   int taille = Header->GetXSize() * Header->GetYSize()  
               * Header->GetSamplesPerPixel();    
   long fragmentBegining; // for ftell, fseek

   bool jpg2000 =     Header->IsJPEG2000();
   bool jpgLossless = Header->IsJPEGLossless();
    
   bool res = true;
   guint16 ItemTagGr,ItemTagEl;
   int ln;  
   
   //  Position on begining of Jpeg Pixels
   
   fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
   fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
   if(Header->GetSwapCode()) {
      ItemTagGr=Header->SwapShort(ItemTagGr); 
      ItemTagEl=Header->SwapShort(ItemTagEl);            
   }
   fread(&ln,4,1,fp); 
   if(Header->GetSwapCode()) 
      ln=Header->SwapLong(ln);    // Basic Offset Table Item length
      
   if (ln != 0) {
      // What is it used for ?!?
      char *BasicOffsetTableItemValue = (char *)malloc(ln+1);        
      fread(BasicOffsetTableItemValue,ln,1,fp); 
   }
   
   // first Fragment initialisation
   fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
   fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
   if(Header->GetSwapCode()) {
      ItemTagGr=Header->SwapShort(ItemTagGr); 
      ItemTagEl=Header->SwapShort(ItemTagEl);            
   }
           
   // parsing fragments until Sequence Delim. Tag found
   while ( ( ItemTagGr == 0xfffe) && (ItemTagEl != 0xe0dd) ) { 
      // --- for each Fragment

      fread(&ln,4,1,fp); 
      if(Header->GetSwapCode()) 
         ln=Header->SwapLong(ln);    // Fragment Item length
   
      fragmentBegining=ftell(fp);   

      if (jpg2000) {          // JPEG 2000 :    call to ???
 
         res = (bool)gdcm_read_JPEG2000_file (fp,destination);  // Not Yet written 

      } // ------------------------------------- endif (JPEG2000)
	
      else if (jpgLossless) { // JPEG LossLess : call to xmedcom JPEG
		   
         JPEGLosslessDecodeImage (fp,  // Reading Fragment pixels
				     (unsigned short *)destination,
				     Header->GetPixelSize()*8* Header->GetSamplesPerPixel(),
                                     ln);						 	   
         res=1; // in order not to break the loop
  
      } // ------------------------------------- endif (JPEGLossless)
               
      else {                   // JPEG Lossy : call to IJG 6b

         if  (Header->GetBitsStored() == 8) {
            res = (bool)gdcm_read_JPEG_file (fp,destination);  // Reading Fragment pixels         
         } else {
            res = (bool)gdcm_read_JPEG_file12 (fp,destination);// Reading Fragment pixels  
         } 
      }  // ------------------------------------- endif (JPEGLossy)    
         
      if (!res) break;
               
      destination = (char *)destination + taille * nBytes; // location in user's memory 
                                                           // for next fragment (if any) 
      
      fseek(fp,fragmentBegining,SEEK_SET); // To be sure we start 
      fseek(fp,ln,SEEK_CUR);               // at the begining of next fragment
      
      ItemTagGr = ItemTagEl =0;
      fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
      fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
      if(Header->GetSwapCode()) {
         ItemTagGr=Header->SwapShort(ItemTagGr); 
         ItemTagEl=Header->SwapShort(ItemTagEl);            
      } 
   
   }     // endWhile parsing fragments until Sequence Delim. Tag found    
 
   Header->CloseFile();
   return res;
}
//-----------------------------------------------------------------------------
