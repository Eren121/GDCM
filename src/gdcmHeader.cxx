// gdcmHeader.cxx
//-----------------------------------------------------------------------------
#include "gdcmHeader.h"

#include <stdio.h>
#include <cerrno>
#include <cctype>    // for isalpha

#include "gdcmUtil.h"
#include "gdcmTS.h"

//-----------------------------------------------------------------------------
// Refer to gdcmHeader::CheckSwap()
//const unsigned int gdcmHeader::HEADER_LENGTH_TO_READ = 256;

// Refer to gdcmHeader::SetMaxSizeLoadEntry()
//const unsigned int gdcmHeader::MAX_SIZE_LOAD_ELEMENT_VALUE = 4096;

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmHeader
 * \brief   
 * @param   InFilename
 * @param   exception_on_error
 * @param   enable_sequences = true to allow the header 
 *          to be parsed *inside* the SeQuences, 
 *          when they have an actual length 
 * @param   ignore_shadow = true if user wants to skip shadow groups 
            during parsing, to save memory space
 *\TODO : may be we need one more bool, 
 *         to allow skipping the private elements while parsing the header
 *         in order to save space	  
 */
gdcmHeader::gdcmHeader(const char *InFilename, 
                       bool exception_on_error,
                       bool enable_sequences, 
		       bool ignore_shadow):
   gdcmParser(InFilename,exception_on_error,enable_sequences,ignore_shadow)
{
}

/**
 * \ingroup gdcmHeader
 * \brief   
 * @param   exception_on_error
 */
gdcmHeader::gdcmHeader(bool exception_on_error) :
   gdcmParser(exception_on_error)
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
   if(!gdcmParser::IsReadable())
      return(false);

   std::string res = GetEntryByNumber(0x0028, 0x0005);
   if ( res != GDCM_UNFOUND && atoi(res.c_str()) > 4 ) 
      return false; // Image Dimensions
   if ( !GetHeaderEntryByNumber(0x0028, 0x0100) )
      return false; // "Bits Allocated"
   if ( !GetHeaderEntryByNumber(0x0028, 0x0101) )
      return false; // "Bits Stored"
   if ( !GetHeaderEntryByNumber(0x0028, 0x0102) )
      return false; // "High Bit"
   if ( !GetHeaderEntryByNumber(0x0028, 0x0103) )
      return false; // "Pixel Representation"
   return true;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a JPEGBaseLineProcess1 one.
 *
 * @return  True when JPEGBaseLineProcess1found. False in all other cases.
 */
bool gdcmHeader::IsJPEGBaseLineProcess1TransferSyntax(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.4.50" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a JPEGExtendedProcess2-4 one.
 *
 * @return  True when JPEGExtendedProcess2-4 found. False in all other cases.
 */
bool gdcmHeader::IsJPEGExtendedProcess2_4TransferSyntax(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);
   return ( Element->GetValue() == "1.2.840.10008.1.2.4.51" );
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a JPEGExtendeProcess3-5 one.
 *
 * @return  True when JPEGExtendedProcess3-5 found. False in all other cases.
 */
bool gdcmHeader::IsJPEGExtendedProcess3_5TransferSyntax(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.4.52" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a JPEGSpectralSelectionProcess6-8 one.
 *
 * @return  True when JPEGSpectralSelectionProcess6-8 found. False in all
 *          other cases.
 */
bool gdcmHeader::IsJPEGSpectralSelectionProcess6_8TransferSyntax(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.4.53" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a RLE Lossless one.
 *
 * @return  True when RLE Lossless found. False in all
 *          other cases.
 */
bool gdcmHeader::IsRLELossLessTransferSyntax(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.5" ) {
      return true;
    }
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief  Determines if Transfer Syntax was already encountered
 *          and if it corresponds to a JPEG Lossless one. 
 *
 * @return  True when RLE Lossless found. False in all
 *          other cases. 
 */
bool gdcmHeader::IsJPEGLossless(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
    // faire qq chose d'intelligent a la place de ça
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   const char * Transfert = Element->GetValue().c_str();
   if ( memcmp(Transfert+strlen(Transfert)-2 ,"70",2)==0) return true;
   if ( memcmp(Transfert+strlen(Transfert)-2 ,"55",2)==0) return true;
   if (Element->GetValue() == "1.2.840.10008.1.2.4.57")   return true;

   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a JPEG200 one.0
 *
 * @return  True when JPEG2000 (Lossly or LossLess) found. False in all
 *          other cases.
 */
bool gdcmHeader::IsJPEG2000(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if (    (Transfer == "1.2.840.10008.1.2.4.90") 
        || (Transfer == "1.2.840.10008.1.2.4.91") )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Predicate for dicom version 3 file.
 * @return  True when the file is a dicom version 3.
 */
bool gdcmHeader::IsDicomV3(void) {
   // Checking if Transfert Syntax exists is enough
   return (GetHeaderEntryByNumber(0x0002, 0x0010) != NULL);
}

/**
 * \ingroup gdcmHeader
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
 *          (The file contains a Signal, not an Image).
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
 *          being the "Planes" tag content.
 * @return  The encountered size when found, 1 by default (single image).
 */
int gdcmHeader::GetZSize(void) {
   // Both  DicomV3 and ACR/Nema consider the "Number of Frames"
   // as the third dimension.
   std::string StrSize = GetEntryByNumber(0x0028,0x0008);
   if (StrSize != GDCM_UNFOUND)
      return atoi(StrSize.c_str());

   // We then consider the "Planes" entry as the third dimension [we
   // cannot retrieve by name since "Planes tag is present both in
   // IMG (0028,0012) and OLY (6000,0012) sections of the dictionary]. 
   StrSize = GetEntryByNumber(0x0028,0x0012);
   if (StrSize != GDCM_UNFOUND)
      return atoi(StrSize.c_str());
   return 1;
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of Bits Stored (actually used)
 *          (as opposite to number of Bits Allocated)
 * 
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
 * 
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
 * 
 * @return  The encountered number of Samples Per Pixel, 1 by default.
 *          (Gray level Pixels)
 */
int gdcmHeader::GetSamplesPerPixel(void) { 
   std::string StrSize = GetEntryByNumber(0x0028,0x0002);
   if (StrSize == GDCM_UNFOUND)
      return 1; // Well, it's supposed to be mandatory ...
                // but sometimes it's missing : we assume Gray pixels
   return atoi(StrSize.c_str());
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the Planar Configuration for RGB images
 *          (0 : RGB Pixels , 1 : R Plane + G Plane + B Plane)
 * 
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
 * \warning 12 bit images appear as 16 bit.
 * \        24 bit images appear as 8 bit
 * @return  0S if nothing found. NOT USABLE file. The caller has to check
 */
std::string gdcmHeader::GetPixelType(void) {
   std::string BitsAlloc = GetEntryByNumber(0x0028, 0x0100); // Bits Allocated
   if (BitsAlloc == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetPixelType: unfound Bits Allocated");
      BitsAlloc = std::string("16");
   }
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
 * \        of *image* pixels (not *icone image* pixels, if any !)
 */
size_t gdcmHeader::GetPixelOffset(void) {
   // We may encounter the 'RETired' (0x0028, 0x0200) tag
   // (Image Location") . This Element contains the number of
   // the group that contains the pixel data (hence the "Pixel Data"
   // is found by indirection through the "Image Location").
   // Inside the group pointed by "Image Location" the searched element
   // is conventionally the element 0x0010 (when the norm is respected).
   // When the "Image Location" is absent we default to group 0x7fe0.
   //
   // If the element (0x0088,0x0200) 'icone image sequence' is found
   // (grPixel,numPixel) is stored twice : the first one for the icon
   // the second one for the image ...
   // pb : sometimes , (0x0088,0x0200) exists, but doesn't contain *anything*
   // see gdcmData/MxTwinLossLess.dcm ...
   guint16 grPixel;
   guint16 numPixel;
   std::string ImageLocation = GetEntryByNumber(0x0028, 0x0200);

   if ( ImageLocation == GDCM_UNFOUND ) { // Image Location
      grPixel = 0x7fe0;                   // default value
   } else {
      grPixel = (guint16) atoi( ImageLocation.c_str() );
   }
   
   if (grPixel == 0xe07f) // sometimes Image Location value doesn't follow 
      grPixel = 0x7fe0;   // the supposed processor endianity. 
                          // see gdcmData/cr172241.dcm
      
   if (grPixel != 0x7fe0) 
      // This is a kludge for old dirty Philips imager.
      numPixel = 0x1010;
   else
      numPixel = 0x0010;
      
   IterHT it = GetHeaderEntrySameNumber(grPixel,numPixel);          
   //std::string icone = GetEntryByNumber(0x0088,0x0200); //icone image sequence
   TagKey key = gdcmDictEntry::TranslateToKey(grPixel,numPixel);
   gdcmHeaderEntry* PixelElement;
  
   if (tagHT.count(key) == 1)   
      PixelElement = (it.first)->second;
   else
      PixelElement = (++it.first)->second;
   
   if (PixelElement) {
      return PixelElement->GetOffset();
   }
   else
      return 0;
}
// TODO : unify those two (previous one and next one)
/**
 * \ingroup gdcmHeader
 * \brief   Recover the pixel area length (in Bytes)
 *  @return 0 by default. NOT USABLE file. The caller has to check.
 */
size_t gdcmHeader::GetPixelAreaLength(void) {
   // If this file complies with the norm we should encounter the
   // "Image Location" tag (0x0028,  0x0200). This tag contains the
   // the group that contains the pixel data (hence the "Pixel Data"
   // is found by indirection through the "Image Location").
   // Inside the group pointed by "Image Location" the searched element
   // is conventionally the element 0x0010 (when the norm is respected).
   // When the "Image Location" is absent we default to group 0x7fe0.
   guint16 grPixel;
   guint16 numPixel;
   std::string ImageLocation = GetEntryByNumber(0x0028, 0x0200);
   if ( ImageLocation == GDCM_UNFOUND ) { // Image Location
      grPixel = 0x7fe0;                   // default value
   } else {
      grPixel = (guint16) atoi( ImageLocation.c_str() );
   }
   if (grPixel == 0xe07f) // sometimes group doesn't follow 
      grPixel = 0x7fe0;   // the supposed processor endianity. see cr172241.dcm
      
   if (grPixel != 0x7fe0)
      // This is a kludge for old dirty Philips imager.
      numPixel = 0x1010;
   else
      numPixel = 0x0010;
              
   IterHT it = GetHeaderEntrySameNumber(grPixel,numPixel);          
   //std::string icone = GetEntryByNumber(0x0088,0x0200); //icone image sequence
   TagKey key = gdcmDictEntry::TranslateToKey(grPixel,numPixel);
   gdcmHeaderEntry* PixelElement;
  
   if (tagHT.count(key) == 1)   
      PixelElement = (it.first)->second;
   else
      PixelElement = (++it.first)->second;
   
   if (PixelElement)
      return PixelElement->GetLength();
   else {
      std::cout << "Big trouble : Pixel Element ("
                << std::hex << grPixel<<","<< numPixel<< ") NOT found" 
                << std::endl;
      return 0;
   }
}

/**
  * \ingroup gdcmHeader
  * \brief tells us if LUT are used
  * \warning Right now, 'Segmented xxx Palette Color Lookup Table Data'
  * \        are NOT considered as LUT, since nobody knows
  * \        how to deal with them
  * @return a Boolean 
  */
bool gdcmHeader::HasLUT(void) {

   // Check the presence of the LUT Descriptors 
   
   // LutDescriptorRed    
   if ( !GetHeaderEntryByNumber(0x0028,0x1101) )
      return false;
   // LutDescriptorGreen 
   if ( !GetHeaderEntryByNumber(0x0028,0x1102) )
      return false;
   // LutDescriptorBlue 
   if ( !GetHeaderEntryByNumber(0x0028,0x1103) )
      return false;
      
   //  It is not enough :
   //  we check also 
   
   // Red Palette Color Lookup Table Data
   if ( !GetHeaderEntryByNumber(0x0028,0x1201) )
      return false; 
   // Green Palette Color Lookup Table Data       
   if ( !GetHeaderEntryByNumber(0x0028,0x1202) )
      return false;
   // Blue Palette Color Lookup Table Data      
   if ( !GetHeaderEntryByNumber(0x0028,0x1203) )
      return false;   
   return true;
}

/**
  * \ingroup gdcmHeader
  * \brief gets the info from 0028,1101 : Lookup Table Desc-Red
  * \           else 0
  * @return Lookup Table number of Bits , 0 by default
  * \       when (0028,0004),Photometric Interpretation = [PALETTE COLOR ] 
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
  * \       when (0028,0004),Photometric Interpretation = [PALETTE COLOR ]
  * \        and (0028,1101),(0028,1102),(0028,1102)  
  * \          - xxx Palette Color Lookup Table Descriptor - are found
  * \        and (0028,1201),(0028,1202),(0028,1202) 
  * \          - xxx Palette Color Lookup Table Data - are found 
  * \warning does NOT deal with :
  * \ 0028 1100 Gray Lookup Table Descriptor (Retired)
  * \ 0028 1221 Segmented Red Palette Color Lookup Table Data
  * \ 0028 1222 Segmented Green Palette Color Lookup Table Data
  * \ 0028 1223 Segmented Blue Palette Color Lookup Table Data 
  * \ no known Dicom reader deals with them :-(
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
   
   unsigned char *LUTRGBA = (unsigned char *)calloc(1024,1); // 256 * 4 (R, G, B, Alpha) 
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
 * \ingroup gdcmHeader
 * \brief gets the info from 0002,0010 : Transfert Syntax
 * \      else 1.
 * @return Transfert Syntax Name (as oposite to Transfert Syntax UID)
 */
std::string gdcmHeader::GetTransfertSyntaxName(void) { 
   // use the gdcmTS (TS : Transfert Syntax)
   std::string TransfertSyntax = GetEntryByNumber(0x0002,0x0010);
   if (TransfertSyntax == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetTransfertSyntaxName: unfound Transfert Syntax (0002,0010)");
      return "Uncompressed ACR-NEMA";
   }
   // we do it only when we need it
   gdcmTS * ts = gdcmGlobal::GetTS();
   std::string tsName=ts->GetValue(TransfertSyntax);
   //delete ts; // Seg Fault when deleted ?!
   return tsName;
}

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
void gdcmHeader::SetImageDataSize(size_t ImageDataSize) {
   std::string content1;
   char car[20];	
   // Assumes HeaderEntry (0x7fe0, 0x0010) exists ...
   // TODO define private members PixelGroupNumber, PicxelElementNumber
   //      update them, use them (only necessary for ACR-NEMA, not DICOM)	
   sprintf(car,"%d",ImageDataSize);
 
   gdcmHeaderEntry *a = GetHeaderEntryByNumber(0x7fe0, 0x0010);
   a->SetLength(ImageDataSize);
 		
   ImageDataSize+=8;
   sprintf(car,"%d",ImageDataSize);
   content1=car;	
   SetEntryByNumber(content1, 0x7fe0, 0x0000);
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
