// gdcmHeader.cxx
//-----------------------------------------------------------------------------
#include "gdcmHeader.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"
#include "gdcmTS.h"
#include "gdcmValEntry.h"

#include <vector>

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief  Constructor 
 * @param  InFilename name of the file whose header we want to analyze
 * @param  exception_on_error whether we want to throw an exception or not
 * @param  enable_sequences = true to allow the header 
 *         to be parsed *inside* the SeQuences, when they have an actual length 
 * @param  ignore_shadow = true if user wants to skip shadow groups 
 *         during parsing, to save memory space
 */
gdcmHeader::gdcmHeader(const char *InFilename, 
                       bool exception_on_error,
                       bool enable_sequences, 
                       bool ignore_shadow):
   gdcmDocument(InFilename,exception_on_error,enable_sequences,ignore_shadow)
{ 
/*
   typedef struct {
      guint32 totalSQlength;
      guint32 alreadyParsedlength;
   } pileElem;
*/
   
   // for some ACR-NEMA images GrPixel, NumPixel is *not* 7fe0,0010
   // We may encounter the 'RETired' (0x0028, 0x0200) tag
   // (Image Location") . This Element contains the number of
   // the group that contains the pixel data (hence the "Pixel Data"
   // is found by indirection through the "Image Location").
   // Inside the group pointed by "Image Location" the searched element
   // is conventionally the element 0x0010 (when the norm is respected).
   // When the "Image Location" is absent we default to group 0x7fe0.
   
   // This IS the right place for the code
 
   std::string ImageLocation = GetEntryByNumber(0x0028, 0x0200);
   if ( ImageLocation == GDCM_UNFOUND ) { // Image Location
      GrPixel = 0x7fe0;                   // default value
   } else {
      GrPixel = (guint16) atoi( ImageLocation.c_str() );
   }   
   if (GrPixel == 0xe07f) // sometimes Image Location value doesn't follow 
      GrPixel = 0x7fe0;   // the supposed processor endianity. 
                          // see gdcmData/cr172241.dcm      
   if (GrPixel != 0x7fe0) 
      // This is a kludge for old dirty Philips imager.
      NumPixel = 0x1010;
   else
      NumPixel = 0x0010;
   
}

/**
 * \brief Constructor  
 * @param exception_on_error whether we want to throw an exception or not
 */
gdcmHeader::gdcmHeader(bool exception_on_error) :
   gdcmDocument(exception_on_error)
{
}

/**
 * \ingroup gdcmHeader
 * \brief   Canonical destructor.
 */
gdcmHeader::~gdcmHeader (void) {
}

//-----------------------------------------------------------------------------
// Print


//-----------------------------------------------------------------------------
// Public

/**
 * \ingroup gdcmHeader
 * \brief  This predicate, based on hopefully reasonable heuristics,
 *         decides whether or not the current gdcmParser was properly parsed
 *         and contains the mandatory information for being considered as
 *         a well formed and usable Dicom/Acr File.
 * @return true when gdcmParser is the one of a reasonable Dicom/Acr file,
 *         false otherwise. 
 */
bool gdcmHeader::IsReadable(void) {
   if(!gdcmDocument::IsReadable()) {
      return(false);
   }
   std::string res = GetEntryByNumber(0x0028, 0x0005);
   if ( res != GDCM_UNFOUND && atoi(res.c_str()) > 4 ) 
      return false; // Image Dimensions
   if ( !GetDocEntryByNumber(0x0028, 0x0100) )
      return false; // "Bits Allocated"
   if ( !GetDocEntryByNumber(0x0028, 0x0101) )
      return false; // "Bits Stored"
   if ( !GetDocEntryByNumber(0x0028, 0x0102) )
      return false; // "High Bit"
   if ( !GetDocEntryByNumber(0x0028, 0x0103) )
      return false; // "Pixel Representation" i.e. 'Sign'
   return true;
}

/**
 * \brief   Retrieve the number of columns of image.
 * @return  The encountered size when found, 0 by default.
 *          0 means the file is NOT USABLE. The caller will have to check
 */
int gdcmHeader::GetXSize(void) {
   std::string StrSize;
   StrSize = GetEntryByNumber(0x0028,0x0011);
   if (StrSize == GDCM_UNFOUND)
      return 0;
   return atoi(StrSize.c_str());
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of lines of image.
 * \warning The defaulted value is 1 as opposed to gdcmHeader::GetXSize()
 * @return  The encountered size when found, 1 by default 
 *          (The ACR-MEMA file contains a Signal, not an Image).
 */
int gdcmHeader::GetYSize(void) {
   std::string StrSize = GetEntryByNumber(0x0028,0x0010);
   if (StrSize != GDCM_UNFOUND)
      return atoi(StrSize.c_str());
   if ( IsDicomV3() )
      return 0;
   else
      // The Rows (0028,0010) entry was optional for ACR/NEMA. It might
      // hence be a signal (1d image). So we default to 1:
      return 1;
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of planes of volume or the number
 *          of frames of a multiframe.
 * \warning When present we consider the "Number of Frames" as the third
 *          dimension. When absent we consider the third dimension as
 *          being the ACR-NEMA "Planes" tag content.
 * @return  The encountered size when found, 1 by default (single image).
 */
int gdcmHeader::GetZSize(void) {
   // Both  DicomV3 and ACR/Nema consider the "Number of Frames"
   // as the third dimension.
   std::string StrSize = GetEntryByNumber(0x0028,0x0008);
   if (StrSize != GDCM_UNFOUND)
      return atoi(StrSize.c_str());

   // We then consider the "Planes" entry as the third dimension 
   StrSize = GetEntryByNumber(0x0028,0x0012);
   if (StrSize != GDCM_UNFOUND)
      return atoi(StrSize.c_str());
   return 1;
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of Bits Stored (actually used)
 *          (as opposite to number of Bits Allocated)
 * @return  The encountered number of Bits Stored, 0 by default.
 *          0 means the file is NOT USABLE. The caller has to check it !
 */
int gdcmHeader::GetBitsStored(void) {  
   std::string StrSize = GetEntryByNumber(0x0028,0x0101);
   if (StrSize == GDCM_UNFOUND)
      return 0;  // It's supposed to be mandatory
                 // the caller will have to check
   return atoi(StrSize.c_str());
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of Bits Allocated
 *          (8, 12 -compacted ACR-NEMA files, 16, ...)
 * @return  The encountered number of Bits Allocated, 0 by default.
 *          0 means the file is NOT USABLE. The caller has to check it !
 */
int gdcmHeader::GetBitsAllocated(void) {
   std::string StrSize = GetEntryByNumber(0x0028,0x0100);
   if (StrSize == GDCM_UNFOUND)
      return 0; // It's supposed to be mandatory
                // the caller will have to check
   return atoi(StrSize.c_str());
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of Samples Per Pixel
 *          (1 : gray level, 3 : RGB -1 or 3 Planes-)
 * @return  The encountered number of Samples Per Pixel, 1 by default.
 *          (Gray level Pixels)
 */
int gdcmHeader::GetSamplesPerPixel(void) {
   std::string StrSize = GetEntryByNumber(0x0028,0x0002);
   if (StrSize == GDCM_UNFOUND)
      return 1; // Well, it's supposed to be mandatory ...
                // but sometimes it's missing : *we* assume Gray pixels
   return atoi(StrSize.c_str());
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the Planar Configuration for RGB images
 *          (0 : RGB Pixels , 1 : R Plane + G Plane + B Plane)
 * @return  The encountered Planar Configuration, 0 by default.
 */
int gdcmHeader::GetPlanarConfiguration(void) {
   std::string StrSize = GetEntryByNumber(0x0028,0x0006);
   if (StrSize == GDCM_UNFOUND)
      return 0;
   return atoi(StrSize.c_str());
}

/**
 * \ingroup gdcmHeader
 * \brief   Return the size (in bytes) of a single pixel of data.
 * @return  The size in bytes of a single pixel of data; 0 by default
 *          0 means the file is NOT USABLE; the caller will have to check        
 */
int gdcmHeader::GetPixelSize(void) {
   std::string PixelType = GetPixelType();
   if (PixelType == "8U"  || PixelType == "8S")
      return 1;
   if (PixelType == "16U" || PixelType == "16S")
      return 2;
   if (PixelType == "32U" || PixelType == "32S")
      return 4;
   if (PixelType == "FD")
      return 8;         
   dbg.Verbose(0, "gdcmHeader::GetPixelSize: Unknown pixel type");
   return 0;
}

/**
 * \ingroup gdcmHeader
 * \brief   Build the Pixel Type of the image.
 *          Possible values are:
 *          - 8U  unsigned  8 bit,
 *          - 8S    signed  8 bit,
 *          - 16U unsigned 16 bit,
 *          - 16S   signed 16 bit,
 *          - 32U unsigned 32 bit,
 *          - 32S   signed 32 bit,
 *          - FD floating double 64 bits (Not kosher DICOM, but so usefull!)
 * \warning 12 bit images appear as 16 bit.
 *          24 bit images appear as 8 bit
 * @return  0S if nothing found. NOT USABLE file. The caller has to check
 */
std::string gdcmHeader::GetPixelType(void) { 
   std::string BitsAlloc = GetEntryByNumber(0x0028, 0x0100); // Bits Allocated
   if (BitsAlloc == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetPixelType: unfound Bits Allocated");
      BitsAlloc = std::string("16");
   }
   if (BitsAlloc == "64")            // )
      return ("FD");
   if (BitsAlloc == "12")            // It will be unpacked
      BitsAlloc = std::string("16");
   else if (BitsAlloc == "24")       // (in order no to be messed up
      BitsAlloc = std::string("8");  // by old RGB images)
     
   std::string Signed = GetEntryByNumber(0x0028, 0x0103); // "Pixel Representation"
   if (Signed == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetPixelType: unfound Pixel Representation");
      BitsAlloc = std::string("0");
   }
   if (Signed == "0")
      Signed = std::string("U");
   else
      Signed = std::string("S");

   return( BitsAlloc + Signed);
}


/**
 * \ingroup gdcmHeader
 * \brief   Recover the offset (from the beginning of the file) 
 *          of *image* pixels (not *icone image* pixels, if any !)
 * @return Pixel Offset
 */
size_t gdcmHeader::GetPixelOffset(void) { 
      
   gdcmDocEntry* PixelElement = GetDocEntryByNumber(GrPixel,NumPixel);
 
   if (PixelElement) {
      return PixelElement->GetOffset();
   } else {
#ifdef GDCM_DEBUG
      std::cout << "Big trouble : Pixel Element ("
                << std::hex << GrPixel<<","<< NumPixel<< ") NOT found"
                << std::endl;  
#endif //GDCM_DEBUG
      return 0;
   }     
}
// TODO : unify those two (previous one and next one)
/**
 * \ingroup gdcmHeader
 * \brief   Recover the pixel area length (in Bytes)
 * @return Pixel Element Length, as stored in the header
 *         (NOT the memory space necessary to hold the Pixels 
 *          -in case of embeded compressed image-)
 *         0 : NOT USABLE file. The caller has to check.
 */
size_t gdcmHeader::GetPixelAreaLength(void) { 
          
   gdcmDocEntry* PixelElement = GetDocEntryByNumber(GrPixel,NumPixel);

   if (PixelElement) {
      return PixelElement->GetLength();
   } else {
#ifdef GDCM_DEBUG
      std::cout << "Big trouble : Pixel Element ("
                << std::hex << GrPixel<<","<< NumPixel<< ") NOT found"
                << std::endl;
#endif //GDCM_DEBUG
      return 0;
   }
}

/**
  * \ingroup gdcmHeader
  * \brief tells us if LUT are used
  * \warning Right now, 'Segmented xxx Palette Color Lookup Table Data'
  *          are NOT considered as LUT, since nobody knows
  *          how to deal with them
  *          Please warn me if you know sbdy that *does* know ... jprx
  * @return true if LUT Descriptors and LUT Tables were found 
  */
bool gdcmHeader::HasLUT(void) {

   // Check the presence of the LUT Descriptors, and LUT Tables    
   // LutDescriptorRed    
   if ( !GetDocEntryByNumber(0x0028,0x1101) )
      return false;
   // LutDescriptorGreen 
   if ( !GetDocEntryByNumber(0x0028,0x1102) )
      return false;
   // LutDescriptorBlue 
   if ( !GetDocEntryByNumber(0x0028,0x1103) )
      return false;   
   // Red Palette Color Lookup Table Data
   if ( !GetDocEntryByNumber(0x0028,0x1201) )
      return false; 
   // Green Palette Color Lookup Table Data       
   if ( !GetDocEntryByNumber(0x0028,0x1202) )
      return false;
   // Blue Palette Color Lookup Table Data      
   if ( !GetDocEntryByNumber(0x0028,0x1203) )
      return false;
   // FIXME : (0x0028,0x3006) : LUT Data (CTX dependent)
   //         NOT taken into account, but we don't know how to use it ...   
   return true;
}

/**
  * \ingroup gdcmHeader
  * \brief gets the info from 0028,1101 : Lookup Table Desc-Red
  *             else 0
  * @return Lookup Table number of Bits , 0 by default
  *          when (0028,0004),Photometric Interpretation = [PALETTE COLOR ]
  * @ return bit number of each LUT item 
  */
int gdcmHeader::GetLUTNbits(void) {
   std::vector<std::string> tokens;
   //int LutLength;
   //int LutDepth;
   int LutNbits;
   //Just hope Lookup Table Desc-Red = Lookup Table Desc-Red = Lookup Table Desc-Blue
   // Consistency already checked in GetLUTLength
   std::string LutDescription = GetEntryByNumber(0x0028,0x1101);
   if (LutDescription == GDCM_UNFOUND)
      return 0;
   tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
   Tokenize (LutDescription, tokens, "\\");
   //LutLength=atoi(tokens[0].c_str());
   //LutDepth=atoi(tokens[1].c_str());
   LutNbits=atoi(tokens[2].c_str());
   tokens.clear();
   return LutNbits;
}

/**
  * \ingroup gdcmHeader
  * \brief builts Red/Green/Blue/Alpha LUT from Header
  *         when (0028,0004),Photometric Interpretation = [PALETTE COLOR ]
  *          and (0028,1101),(0028,1102),(0028,1102)  
  *            - xxx Palette Color Lookup Table Descriptor - are found
  *          and (0028,1201),(0028,1202),(0028,1202) 
  *            - xxx Palette Color Lookup Table Data - are found 
  * \warning does NOT deal with :
  *   0028 1100 Gray Lookup Table Descriptor (Retired)
  *   0028 1221 Segmented Red Palette Color Lookup Table Data
  *   0028 1222 Segmented Green Palette Color Lookup Table Data
  *   0028 1223 Segmented Blue Palette Color Lookup Table Data 
  *   no known Dicom reader deals with them :-(
  * @return a RGBA Lookup Table 
  */ 
unsigned char * gdcmHeader::GetLUTRGBA(void) {
// Not so easy : see 
// http://www.barre.nom.fr/medical/dicom2/limitations.html#Color%20Lookup%20Tables

//  if Photometric Interpretation # PALETTE COLOR, no LUT to be done
   if (GetEntryByNumber(0x0028,0x0004) != "PALETTE COLOR ") {
      return NULL;
   }  
   int lengthR, debR, nbitsR;
   int lengthG, debG, nbitsG;
   int lengthB, debB, nbitsB;
   
// Get info from Lut Descriptors
// (the 3 LUT descriptors may be different)    
   std::string LutDescriptionR = GetEntryByNumber(0x0028,0x1101);
   if (LutDescriptionR == GDCM_UNFOUND)
      return NULL;
   std::string LutDescriptionG = GetEntryByNumber(0x0028,0x1102);
   if (LutDescriptionG == GDCM_UNFOUND)
      return NULL;   
   std::string LutDescriptionB = GetEntryByNumber(0x0028,0x1103);
   if (LutDescriptionB == GDCM_UNFOUND)
      return NULL;
      
   std::vector<std::string> tokens;
      
   tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
   Tokenize (LutDescriptionR, tokens, "\\");
   lengthR=atoi(tokens[0].c_str()); // Red LUT length in Bytes
   debR   =atoi(tokens[1].c_str()); // subscript of the first Lut Value
   nbitsR =atoi(tokens[2].c_str()); // Lut item size (in Bits)
   tokens.clear();
   
   tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
   Tokenize (LutDescriptionG, tokens, "\\");
   lengthG=atoi(tokens[0].c_str()); // Green LUT length in Bytes
   debG   =atoi(tokens[1].c_str()); // subscript of the first Lut Value
   nbitsG =atoi(tokens[2].c_str()); // Lut item size (in Bits)
   tokens.clear();  
   
   tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
   Tokenize (LutDescriptionB, tokens, "\\");
   lengthB=atoi(tokens[0].c_str()); // Blue LUT length in Bytes
   debB   =atoi(tokens[1].c_str()); // subscript of the first Lut Value
   nbitsB =atoi(tokens[2].c_str()); // Lut item size (in Bits)
   tokens.clear();
 
   // Load LUTs into memory, (as they were stored on disk)
   unsigned char *lutR = (unsigned char *)
                         GetEntryVoidAreaByNumber(0x0028,0x1201);
   unsigned char *lutG = (unsigned char *)
                         GetEntryVoidAreaByNumber(0x0028,0x1202);
   unsigned char *lutB = (unsigned char *)
                         GetEntryVoidAreaByNumber(0x0028,0x1203); 
   
   if (!lutR || !lutG || !lutB ) {
      return NULL;
   } 
   // forge the 4 * 8 Bits Red/Green/Blue/Alpha LUT 
   
   unsigned char *LUTRGBA = new unsigned char[1024]; // 256 * 4 (R, G, B, Alpha) 
   if (!LUTRGBA) {
      return NULL;
   }
   memset(LUTRGBA, 0, 1024);
   // Bits Allocated
   int nb;
   std::string str_nb = GetEntryByNumber(0x0028,0x0100);
   if (str_nb == GDCM_UNFOUND ) {
      nb = 16;
   } else {
      nb = atoi(str_nb.c_str() );
   }  
   int mult;

   if (nbitsR==16 && nb==8) // when LUT item size is different than pixel size
      mult=2;               // high byte must be = low byte 
   else                     // See PS 3.3-2003 C.11.1.1.2 p 619
      mult=1; 
 
   // if we get a black image, let's just remove the '+1'
   // from 'i*mult+1' and check again 
   // if it works, we shall have to check the 3 Palettes
   // to see which byte is ==0 (first one, or second one)
   // and fix the code
   // We give up the checking to avoid some (useless ?)overhead 
   // (optimistic asumption)
   unsigned char *a;      
   int i;

   a = LUTRGBA+0;
   for(i=0;i<lengthR;i++) {
      *a = lutR[i*mult+1]; 
      a+=4;       
   }        
   a = LUTRGBA+1;
   for(i=0;i<lengthG;i++) {
      *a = lutG[i*mult+1]; 
      a+=4;       
   }  
   a = LUTRGBA+2;
   for(i=0;i<lengthB;i++) {
      *a = lutB[i*mult+1]; 
      a+=4;       
   }  
   a = LUTRGBA+3;
   for(i=0;i<256;i++) {
      *a = 1; // Alpha component
      a+=4; 
   } 
   
   //How to free the now useless LUTs?
   //free(LutR); free(LutB); free(LutG); // Seg Fault when used
   return(LUTRGBA);   
} 

/**
 * \brief Accesses the info from 0002,0010 : Transfert Syntax and gdcmTS
 *        else 1.
 * @return The full Transfert Syntax Name (as opposed to Transfert Syntax UID)
 */
std::string gdcmHeader::GetTransfertSyntaxName(void) { 
   // use the gdcmTS (TS : Transfert Syntax)
   std::string TransfertSyntax = GetEntryByNumber(0x0002,0x0010);
   if (TransfertSyntax == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetTransfertSyntaxName:"
                     " unfound Transfert Syntax (0002,0010)");
      return "Uncompressed ACR-NEMA";
   }
   // we do it only when we need it
   gdcmTS * ts = gdcmGlobal::GetTS();
   std::string tsName=ts->GetValue(TransfertSyntax);
   //delete ts; /// \todo Seg Fault when deleted ?!
   return tsName;
}

/**
 * \brief Sets the Pixel Area size in the Header
 *        --> not-for-rats function
 * @param ImageDataSize new Pixel Area Size
 *        warning : nothing else is checked
 */
void gdcmHeader::SetImageDataSize(size_t ImageDataSize) {
   std::string content1;
   char car[20];

   sprintf(car,"%d",ImageDataSize);
 
   gdcmDocEntry *a = GetDocEntryByNumber(GrPixel, NumPixel);
   a->SetLength(ImageDataSize);

   ImageDataSize+=8;
   sprintf(car,"%d",ImageDataSize);
   content1=car;
   SetEntryByNumber(content1, GrPixel, NumPixel);
}

//-----------------------------------------------------------------------------
// Protected

/**
 * \brief anonymize a Header (removes Patient's personal info)
 *        (read the code to see which ones ...)
 */
bool gdcmHeader::anonymizeHeader() {

  gdcmDocEntry *patientNameHE = GetDocEntryByNumber (0x0010, 0x0010);
    
  ReplaceIfExistByNumber ("  ",0x0010, 0x2154); // Telephone   
  ReplaceIfExistByNumber ("  ",0x0010, 0x1040); // Adress
  ReplaceIfExistByNumber ("  ",0x0010, 0x0020); // Patient ID
  
  if (patientNameHE) {
     std::string StudyInstanceUID =  GetEntryByNumber (0x0020, 0x000d);
     if (StudyInstanceUID !=GDCM_UNFOUND)
        ReplaceOrCreateByNumber(StudyInstanceUID, 0x0010, 0x0010);
     else
        ReplaceOrCreateByNumber(std::string("anonymised"), 0x0010, 0x0010);
  }
  
  // Just for fun :-(
  // (if any) remove or replace all the stuff that contains a Date
  
//0008 0012 DA ID Instance Creation Date
//0008 0020 DA ID Study Date
//0008 0021 DA ID Series Date
//0008 0022 DA ID Acquisition Date
//0008 0023 DA ID Content Date
//0008 0024 DA ID Overlay Date
//0008 0025 DA ID Curve Date
//0008 002a DT ID Acquisition Datetime
//0018 9074 DT ACQ Frame Acquisition Datetime
//0018 9151 DT ACQ Frame Reference Datetime
//0018 a002 DT ACQ Contribution Date Time
//0020 3403 SH REL Modified Image Date (RET)
//0032 0032 DA SDY Study Verified Date
//0032 0034 DA SDY Study Read Date
//0032 1000 DA SDY Scheduled Study Start Date
//0032 1010 DA SDY Scheduled Study Stop Date
//0032 1040 DA SDY Study Arrival Date
//0032 1050 DA SDY Study Completion Date
//0038 001a DA VIS Scheduled Admission Date
//0038 001c DA VIS Scheduled Discharge Date
//0038 0020 DA VIS Admitting Date
//0038 0030 DA VIS Discharge Date
//0040 0002 DA PRC Scheduled Procedure Step Start Date
//0040 0004 DA PRC Scheduled Procedure Step End Date
//0040 0244 DA PRC Performed Procedure Step Start Date
//0040 0250 DA PRC Performed Procedure Step End Date
//0040 2004 DA PRC Issue Date of Imaging Service Request
//0040 4005 DT PRC Scheduled Procedure Step Start Date and Time
//0040 4011 DT PRC Expected Completion Date and Time
//0040 a030 DT PRC Verification Date Time
//0040 a032 DT PRC Observation Date Time
//0040 a120 DT PRC DateTime
//0040 a121 DA PRC Date
//0040 a13a DT PRC Referenced Datetime
//0070 0082 DA ??? Presentation Creation Date
//0100 0420 DT ??? SOP Autorization Date and Time
//0400 0105 DT ??? Digital Signature DateTime
//2100 0040 DA PJ Creation Date
//3006 0008 DA SSET Structure Set Date
//3008 0024 DA ??? Treatment Control Point Date
//3008 0054 DA ??? First Treatment Date
//3008 0056 DA ??? Most Recent Treatment Date
//3008 0162 DA ??? Safe Position Exit Date
//3008 0166 DA ??? Safe Position Return Date
//3008 0250 DA ??? Treatment Date
//300a 0006 DA RT RT Plan Date
//300a 022c DA RT Air Kerma Rate Reference Date
//300e 0004 DA RT Review Date
 return true;  
}
//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
