// gdcmHeader.cxx
//-----------------------------------------------------------------------------
#include "gdcmHeader.h"

#include <stdio.h>
#include <cerrno>
// For nthos:
#ifdef _MSC_VER
#include <winsock.h>
#else
#include <netinet/in.h>
#endif
#include <cctype>    // for isalpha

#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif

#include "gdcmUtil.h"
#include "gdcmTS.h"

//-----------------------------------------------------------------------------
// Refer to gdcmHeader::CheckSwap()
const unsigned int gdcmHeader::HEADER_LENGTH_TO_READ = 256;

// Refer to gdcmHeader::SetMaxSizeLoadElementValue()
const unsigned int gdcmHeader::MAX_SIZE_LOAD_ELEMENT_VALUE = 4096;

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
 *\TODO : may be we need one more bool, 
 *         to allow skipping the private elements while parsing the header
 *         in order to save space	  
 */
gdcmHeader::gdcmHeader(const char *InFilename, 
                       bool exception_on_error,
                       bool enable_sequences ) {
   if (enable_sequences)
      enableSequences = 1;
   else
      enableSequences = 0;
   
   SetMaxSizeLoadElementValue(MAX_SIZE_LOAD_ELEMENT_VALUE);
   filename = InFilename;
   Initialise();
   if ( !OpenFile(exception_on_error))
      return;
   ParseHeader();
   wasUpdated = 0;  // will be set to 1 if user adds an entry
   LoadHeaderEntries();
   CloseFile();
}

/**
 * \ingroup gdcmHeader
 * \brief   
 * @param   exception_on_error
 */
gdcmHeader::gdcmHeader(bool exception_on_error) {
  SetMaxSizeLoadElementValue(MAX_SIZE_LOAD_ELEMENT_VALUE);
  Initialise();
}

/**
 * \ingroup gdcmHeader
 * \brief   Canonical destructor.
 */
gdcmHeader::~gdcmHeader (void) {
  dicom_vr =   (gdcmVR*)0; 
  Dicts    =   (gdcmDictSet*)0;
  RefPubDict = (gdcmDict*)0;
  RefShaDict = (gdcmDict*)0;
  return;
}

//-----------------------------------------------------------------------------
// Print

/**
  * \ingroup gdcmHeader
  * \brief Prints the Header Entries (Dicom Elements)
  *        both from the H Table and the chained list
  * @return
  */ 
void gdcmHeader::PrintPubEntry(std::ostream & os) {
   PubEntrySet.Print(os);
}

/**
  * \ingroup gdcmHeader
  * \brief Prints The Dict Entries of THE public Dicom Dictionnry
  * @return
  */  
void gdcmHeader::PrintPubDict(std::ostream & os) {
   RefPubDict->Print(os);
}

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup gdcmHeader
 * \brief  This predicate, based on hopefully reasonable heuristics,
 *         decides whether or not the current gdcmHeader was properly parsed
 *         and contains the mandatory information for being considered as
 *         a well formed and usable Dicom/Acr File.
 * @return true when gdcmHeader is the one of a reasonable Dicom/Acr file,
 *         false otherwise. 
 */
bool gdcmHeader::IsReadable(void) {
   std::string res = GetEntryByNumber(0x0028, 0x0005);
   if ( res != GDCM_UNFOUND && atoi(res.c_str()) > 4 ) {
      return false; // Image Dimensions
   }

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
 *          and if it corresponds to a ImplicitVRLittleEndian one.
 *
 * @return  True when ImplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmHeader::IsImplicitVRLittleEndianTransferSyntax(void) {
   gdcmHeaderEntry *Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a ExplicitVRLittleEndian one.
 *
 * @return  True when ExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmHeader::IsExplicitVRLittleEndianTransferSyntax(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.1" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a DeflatedExplicitVRLittleEndian one.
 *
 * @return  True when DeflatedExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmHeader::IsDeflatedExplicitVRLittleEndianTransferSyntax(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.1.99" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a Explicit VR Big Endian one.
 *
 * @return  True when big endian found. False in all other cases.
 */
bool gdcmHeader::IsExplicitVRBigEndianTransferSyntax(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.2" )  //1.2.2 ??? A verifier !
      return true;
   return false;
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

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.4.51" )
      return true;
   return false;
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
   if ( Transfer == "1.2.840.10008.1.2.5" )
      return true;
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
 * \brief  returns the File Type 
 *         (ACR, ACR_LIBIDO, ExplicitVR, ImplicitVR, Unknown)
 * @return 
 */
FileType gdcmHeader::GetFileType(void) {
   return(filetype);
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of columns of image.
 * @return  The encountered size when found, 0 by default.
 *          0 means the file is NOT USABLE. The caller will have to check
 */
int gdcmHeader::GetXSize(void) {
   // We cannot check for "Columns" because the "Columns" tag is present
   // both in IMG (0028,0011) and OLY (6000,0011) sections of the dictionary.
   std::string StrSize = GetEntryByNumber(0x0028,0x0011);
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
   // We cannot check for "Rows" because the "Rows" tag is present
   // both in IMG (0028,0010) and OLY (6000,0010) sections of the dictionary.
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
 * \brief   Recover the offset (from the beginning of the file) of the pixels.
 */
size_t gdcmHeader::GetPixelOffset(void) {
   // If this file complies with the norm we should encounter the
   // "Image Location" tag (0x0028, 0x0200). This tag contains the
   // the group that contains the pixel data (hence the "Pixel Data"
   // is found by indirection through the "Image Location").
   // Inside the group pointed by "Image Location" the searched element
   // is conventionally the element 0x0010 (when the norm is respected).
   // When the "Image Location" is absent we default to group 0x7fe0.
   guint16 grPixel;
   guint16 numPixel;
   std::string ImageLocation = GetEntryByNumber(0x0028, 0x0200);

   if ( ImageLocation == GDCM_UNFOUND ) { // Image Location
      grPixel = 0x7fe0;
   } else {
      grPixel = (guint16) atoi( ImageLocation.c_str() );
   }
   if (grPixel != 0x7fe0)
      // This is a kludge for old dirty Philips imager.
      numPixel = 0x1010;
   else
      numPixel = 0x0010;
         
   gdcmHeaderEntry* PixelElement = GetHeaderEntryByNumber(grPixel,numPixel);
   if (PixelElement)
      return PixelElement->GetOffset();
   else
      return 0;
}

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
   if ( ImageLocation == GDCM_UNFOUND ) {
      grPixel = 0x7fe0;
   } else {
      grPixel = (guint16) atoi( ImageLocation.c_str() );
   }
   if (grPixel != 0x7fe0)
      // This is a kludge for old dirty Philips imager.
      numPixel = 0x1010;
   else
      numPixel = 0x0010;
         
   gdcmHeaderEntry* PixelElement = GetHeaderEntryByNumber(grPixel,numPixel);
   if (PixelElement)
      return PixelElement->GetLength();
   else
      return 0;
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
   if ( !GetHeaderEntryByNumber(0x0028,0x1101) )
      return false;
   // LutDescriptorGreen 
   if ( !GetHeaderEntryByNumber(0x0028,0x1102) )
      return false;
   // LutDescriptorBlue 
   if ( !GetHeaderEntryByNumber(0x0028,0x1103) )
      return false;
   //  It is not enough
   // we check also 
   if ( !GetHeaderEntryByNumber(0x0028,0x1201) )
      return false;  
   if ( !GetHeaderEntryByNumber(0x0028,0x1202) )
      return false;
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
   if (gdcmHeader::GetEntryByNumber(0x0028,0x0004) != "PALETTE COLOR ") {
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
   debG   =atoi(tokens[1].c_str());
   nbitsG =atoi(tokens[2].c_str());
   tokens.clear();  
   
   tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
   Tokenize (LutDescriptionB, tokens, "\\");
   lengthB=atoi(tokens[0].c_str()); // Blue LUT length in Bytes
   debB   =atoi(tokens[1].c_str());
   nbitsB =atoi(tokens[2].c_str());
   tokens.clear();
 
// Load LUTs into memory, (as they were stored on disk)
   unsigned char *lutR = (unsigned char *)
                         GetPubEntryVoidAreaByNumber(0x0028,0x1201);
   unsigned char *lutG = (unsigned char *)
                         GetPubEntryVoidAreaByNumber(0x0028,0x1202);
   unsigned char *lutB = (unsigned char *)
                         GetPubEntryVoidAreaByNumber(0x0028,0x1203); 
   
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
   // We give up the checking to avoid some overhead 
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
 * \ingroup gdcmHeader
 * \brief   Searches within the file Header for element value of
 *          a given tag.
 * @param   tagName name of the searched element.
 * @return  Corresponding element value when it exists, and the string
 *          GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetPubEntryByName(std::string tagName) {
   gdcmDictEntry *dictEntry = RefPubDict->GetTagByName(tagName); 
   if( dictEntry == NULL)
      return GDCM_UNFOUND;

   return(GetEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()));  
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within the elements parsed with the file Header for
 *          the element value representation of a given tag.
 *
 *          Obtaining the VR (Value Representation) might be needed by caller
 *          to convert the string typed content to caller's native type 
 *          (think of C++ vs Python). The VR is actually of a higher level
 *          of semantics than just the native C++ type.
 * @param   tagName name of the searched element.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetPubEntryVRByName(std::string tagName) {
   gdcmDictEntry *dictEntry = RefPubDict->GetTagByName(tagName); 
   if( dictEntry == NULL)
      return GDCM_UNFOUND;

   gdcmHeaderEntry* elem =  GetHeaderEntryByNumber(dictEntry->GetGroup(),
                                                   dictEntry->GetElement());					
   return elem->GetVR();
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within the public dictionary for element value of
 *          a given tag.
 * @param   group Group of the researched tag.
 * @param   element Element of the researched tag.
 * @return  Corresponding element value when it exists, and the string
 *          GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetPubEntryByNumber(guint16 group, guint16 element) {
   return PubEntrySet.GetEntryByNumber(group, element);
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within the public dictionary for element value
 *          representation of a given tag.
 *
 *          Obtaining the VR (Value Representation) might be needed by caller
 *          to convert the string typed content to caller's native type 
 *          (think of C++ vs Python). The VR is actually of a higher level
 *          of semantics than just the native C++ type.
 * @param   group Group of the researched tag.
 * @param   element Element of the researched tag.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetPubEntryVRByNumber(guint16 group, guint16 element) {
   gdcmHeaderEntry* elem =  GetHeaderEntryByNumber(group, element);
   if ( !elem )
      return GDCM_UNFOUND;
   return elem->GetVR();
}

/**
 * \ingroup gdcmHeader
 * \brief   Accesses an existing gdcmHeaderEntry (i.e. a Dicom Element)
 *          in the PubHeaderEntrySet of this instance
 *          through tag name and modifies it's content with the given value.
 * @param   content new value to substitute with
 * @param   tagName name of the Header Entry (Dicom Element) to be modified
 */
bool gdcmHeader::SetPubEntryByName(std::string content, std::string tagName) {
   //return (  PubHeaderEntrySet.SetHeaderEntryByName (content, tagName) );
   gdcmDictEntry *dictEntry = RefPubDict->GetTagByName(tagName); 
   if( dictEntry == NULL)
      return false;
   return(PubEntrySet.SetEntryByNumber(content,
                                       dictEntry->GetGroup(),
                                       dictEntry->GetElement()));   
}

/**
 * \ingroup gdcmHeader
 * \brief   Accesses an existing gdcmHeaderEntry (i.e. a Dicom Element)
 *          in the PubHeaderEntrySet of this instance
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * @param   content new value to substitute with
 * @param   group   group of the Dicom Element to modify
 * @param   element element of the Dicom Element to modify
 */
bool gdcmHeader::SetPubEntryByNumber(std::string content, guint16 group,
                                    guint16 element)
                                    
//TODO  : homogeneiser les noms : SetPubElValByNumber   
//                    qui appelle PubHeaderEntrySet.SetHeaderEntryByNumber 
//        pourquoi pas            SetPubHeaderEntryByNumber ??
{
   return ( PubEntrySet.SetEntryByNumber (content, group, element) );
}

/**
 * \ingroup gdcmHeader
 * \brief   Accesses an existing gdcmHeaderEntry (i.e. a Dicom Element)
 *          in the PubHeaderEntrySet of this instance
 *          through it's (group, element) and modifies it's length with
 *          the given value.
 * \warning Use with extreme caution.
 * @param   length new length to substitute with
 * @param   group   group of the ElVal to modify
 * @param   element element of the ElVal to modify
 * @return  1 on success, 0 otherwise.
 */

bool gdcmHeader::SetPubEntryLengthByNumber(guint32 length, guint16 group,
                                    guint16 element) {
	return (  PubEntrySet.SetEntryLengthByNumber (length, group, element) );
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value of a given tag.
 * @param   tagName name of the searched element.
 * @return  Corresponding element value when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetEntryByName(std::string tagName) {
   return GetPubEntryByName(tagName);
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value representation of a given tag.
 *
 *          Obtaining the VR (Value Representation) might be needed by caller
 *          to convert the string typed content to caller's native type 
 *          (think of C++ vs Python). The VR is actually of a higher level
 *          of semantics than just the native C++ type.
 * @param   tagName name of the searched element.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetEntryVRByName(std::string tagName) {
   return GetPubEntryVRByName(tagName);
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value representation of a given tag.
 * @param   group Group of the searched tag.
 * @param   element Element of the searched tag.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetEntryByNumber(guint16 group, guint16 element) {
   return GetPubEntryByNumber(group, element);
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value representation of a given tag..
 *
 *          Obtaining the VR (Value Representation) might be needed by caller
 *          to convert the string typed content to caller's native type 
 *          (think of C++ vs Python). The VR is actually of a higher level
 *          of semantics than just the native C++ type.
 * @param   group Group of the searched tag.
 * @param   element Element of the searched tag.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetEntryVRByNumber(guint16 group, guint16 element) {
   return GetPubEntryVRByNumber(group, element);
}

/**
 * \ingroup gdcmHeader
 * \brief  Sets the value (string) of the Header Entry (Dicom Element)
 * @param   content string value of the Dicom Element
 * @param   tagName name of the searched Dicom Element.
 * @return  true when found
 */
bool gdcmHeader::SetEntryByName(std::string content,std::string tagName) {
   gdcmDictEntry *dictEntry = RefPubDict->GetTagByName(tagName); 
   if( dictEntry == NULL)
      return false;				    
				   				   				    
				    
   TagKey key = gdcmDictEntry::TranslateToKey(dictEntry->GetGroup(), 
                                              dictEntry->GetElement());
   if ( PubEntrySet.GetTagHT().count(key) == 0 )
      return false;
   int l = content.length();
   if(l%2) {  // Odd length are padded with a space (020H).
      l++;
      content = content + '\0';
   }
      
   //tagHt[key]->SetValue(content);   
   gdcmHeaderEntry * a;
   IterHT p;
   TagHeaderEntryHT::iterator p2;
   // DO NOT remove the following lines : they explain how the stuff works 
   //p= tagHt.equal_range(key); // get a pair of iterators first-last synonym
   //p2=p.first;                // iterator on the first synonym 
   //a=p2->second;              // H Table target column (2-nd col)    
   // or, easier :
   a = ((PubEntrySet.GetTagHT().equal_range(key)).first)->second;       
   a-> SetValue(content);   
   std::string vr = a->GetVR();
   
   guint32 lgr;
   if( (vr == "US") || (vr == "SS") ) 
      lgr = 2;
   else if( (vr == "UL") || (vr == "SL") )
      lgr = 4;
   else
      lgr = l;	   
   a->SetLength(lgr);   
   return true;
}

/**
 * \ingroup gdcmHeader
 * \brief   opens the file
 * @param   exception_on_error
 * @return  
 */
FILE *gdcmHeader::OpenFile(bool exception_on_error)
  throw(gdcmFileError) {
  fp=fopen(filename.c_str(),"rb");
  if(exception_on_error) {
    if(!fp)
      throw gdcmFileError("gdcmHeader::gdcmHeader(const char *, bool)");
  }

  if ( fp ) {
     guint16 zero;
     fread(&zero,  (size_t)2, (size_t)1, fp);

    //ACR -- or DICOM with no Preamble --
    if( zero == 0x0008 || zero == 0x0800 || zero == 0x0002 || zero == 0x0200)
       return(fp);

    //DICOM
    fseek(fp, 126L, SEEK_CUR);
    char dicm[4];
    fread(dicm,  (size_t)4, (size_t)1, fp);
    if( memcmp(dicm, "DICM", 4) == 0 )
       return(fp);

    fclose(fp);
    dbg.Verbose(0, "gdcmHeader::gdcmHeader not DICOM/ACR", filename.c_str());
  }
  else {
    dbg.Verbose(0, "gdcmHeader::gdcmHeader cannot open file", filename.c_str());
  }
  return(NULL);
}

/**
 * \ingroup gdcmHeader
 * \brief closes the file  
 * @return  TRUE if the close was successfull 
 */
bool gdcmHeader::CloseFile(void) {
  int closed = fclose(fp);
  fp = (FILE *)0;
  if (! closed)
     return false;
  return true;
}

/**
 * \ingroup gdcmHeader
 * \brief   Parses the header of the file but WITHOUT loading element values.
 */
void gdcmHeader::ParseHeader(bool exception_on_error) throw(gdcmFormatError) {
   gdcmHeaderEntry * newHeaderEntry = (gdcmHeaderEntry *)0;
   
   rewind(fp);
   CheckSwap();
   while ( (newHeaderEntry = ReadNextHeaderEntry()) ) { 
      SkipHeaderEntry(newHeaderEntry);
      PubEntrySet.Add(newHeaderEntry);
   }
}

/**
 * \ingroup gdcmHeader
 * \brief 
 * @param fp file pointer on an already open file
 * @param type file type ( ImplicitVR, ExplicitVR, ...)
 * @return  Boolean
 */ 
bool gdcmHeader::Write(FILE * fp, FileType type) {

   // TODO : move the following lines (and a lot of others, to be written)
   // to a future function CheckAndCorrectHeader

   if (type == ImplicitVR) {
      std::string implicitVRTransfertSyntax = "1.2.840.10008.1.2";
      ReplaceOrCreateByNumber(implicitVRTransfertSyntax,0x0002, 0x0010);
      
      //FIXME Refer to standards on page 21, chapter 6.2 "Value representation":
      //      values with a VR of UI shall be padded with a single trailing null
      //      Dans le cas suivant on doit pader manuellement avec un 0
      
      PubEntrySet.SetEntryLengthByNumber(18, 0x0002, 0x0010);
   } 

   if (type == ExplicitVR) {
      std::string explicitVRTransfertSyntax = "1.2.840.10008.1.2.1";
      ReplaceOrCreateByNumber(explicitVRTransfertSyntax,0x0002, 0x0010);
      
      //FIXME Refer to standards on page 21, chapter 6.2 "Value representation":
      //      values with a VR of UI shall be padded with a single trailing null
      //      Dans le cas suivant on doit pader manuellement avec un 0
      
      PubEntrySet.SetEntryLengthByNumber(20, 0x0002, 0x0010);
   }

   return PubEntrySet.Write(fp, type);
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
   sprintf(car,"%d",ImageDataSize);
 
   gdcmHeaderEntry *a = GetHeaderEntryByNumber(0x7fe0, 0x0010);
   a->SetLength(ImageDataSize);
 		
   ImageDataSize+=8;
   sprintf(car,"%d",ImageDataSize);
   content1=car;	
   SetPubEntryByNumber(content1, 0x7fe0, 0x0000);
}

/**
 * \ingroup gdcmHeader
 * \brief   Modifies the value of a given Header Entry (Dicom Element)
 *          if it exists; Creates it with the given value if it doesn't
 * @param   Value passed as a std::string
 * @param   Group
 * @param   Elem
 * \return  boolean
 */
bool gdcmHeader::ReplaceOrCreateByNumber(std::string Value, 
                                        guint16 Group, guint16 Elem ) {

   if (CheckIfExistByNumber(Group, Elem) == 0) {
      gdcmHeaderEntry* a =NewHeaderEntryByNumber(Group, Elem);
      if (a == NULL) 
         return false;
      PubEntrySet.Add(a);
   }   
   PubEntrySet.SetEntryByNumber(Value, Group, Elem);
   return(true);
}   

/**
 * \ingroup gdcmHeader
 * \brief   Modifies the value of a given Header Entry (Dicom Element)
 *          if it exists; Creates it with the given value if it doesn't
 * @param   Value passed as a char*
 * @param   Group
 * @param   Elem
 * \return  boolean 
 * 
 */
bool gdcmHeader::ReplaceOrCreateByNumber(char* Value, guint16 Group, guint16 Elem ) {

   gdcmHeaderEntry* nvHeaderEntry=NewHeaderEntryByNumber(Group, Elem);
   // TODO : check if fails
   PubEntrySet.Add(nvHeaderEntry);
   std::string v = Value;	
   PubEntrySet.SetEntryByNumber(v, Group, Elem);
   return(true);
}  

/**
 * \ingroup gdcmHeader
 * \brief   Set a new value if the invoked element exists
 *          Seems to be useless !!!
 * @param   Value
 * @param   Group
 * @param   Elem
 * \return integer acts as a boolean 
 */
bool gdcmHeader::ReplaceIfExistByNumber(char* Value, guint16 Group, guint16 Elem ) {

   std::string v = Value;	
   PubEntrySet.SetEntryByNumber(v, Group, Elem);
   return true;
} 

/**
 * \ingroup gdcmHeader
 * \brief   Swaps back the bytes of 4-byte long integer accordingly to
 *          processor order.
 * @return  The properly swaped 32 bits integer.
 */
guint32 gdcmHeader::SwapLong(guint32 a) {
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
 * \brief   Swaps the bytes so they agree with the processor order
 * @return  The properly swaped 16 bits integer.
 */
guint16 gdcmHeader::SwapShort(guint16 a) {
   if ( (sw==4321)  || (sw==2143) )
      a =(((a<<8) & 0x0ff00) | ((a>>8)&0x00ff));
   return (a);
}

//-----------------------------------------------------------------------------
// Protected
/**
 * \ingroup gdcmHeader
 * \brief   
 *
 * @return 
 */
gdcmHeaderEntry *gdcmHeader::GetHeaderEntryByNumber(guint16 Group, guint16 Elem) {
   gdcmHeaderEntry *HeaderEntry = PubEntrySet.GetHeaderEntryByNumber(Group, Elem);	 
   if (!HeaderEntry) {
      dbg.Verbose(1, "gdcmHeader::GetHeaderEntryByNumber",
                  "failed to Locate gdcmHeaderEntry");
      return NULL;
   }
   return HeaderEntry;
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within the Header Entries for a Dicom Element of
 *          a given tag.
 * @param   tagName name of the searched Dicom Element.
 * @return  Corresponding Dicom Element when it exists, and NULL
 *          otherwise.
 */
 gdcmHeaderEntry *gdcmHeader::GetHeaderEntryByName(std::string tagName) {
   gdcmDictEntry *dictEntry = RefPubDict->GetTagByName(tagName); 
   if( dictEntry == NULL)
      return NULL;

  return(GetHeaderEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()));
}

/**
 * \ingroup gdcmHeader
 * \brief   Checks if a given HeaderEntry (group,number) 
 * \ exists in the Public HeaderEntrySet
 * @param   Group
 * @param   Elem
 * @return  boolean  
 */
bool gdcmHeader::CheckIfExistByNumber(guint16 Group, guint16 Elem ) {
   return (PubEntrySet.CheckIfExistByNumber(Group, Elem)>0);
}

/**
 * \ingroup gdcmHeader
 * \brief   Gets (from Header) the offset  of a 'non string' element value 
 * \        (LoadElementValues has already be executed)
 * @param   Group
 * @param   Elem
 * @return File Offset of the Element Value 
 */
size_t gdcmHeader::GetPubEntryOffsetByNumber(guint16 Group, guint16 Elem) {
   gdcmHeaderEntry* Entry = GetHeaderEntryByNumber(Group, Elem);	 
   if (!Entry) {
      dbg.Verbose(1, "gdcmHeader::GetHeaderEntryByNumber",
                      "failed to Locate gdcmHeaderEntry");
      return (size_t)0;
   }
   return Entry->GetOffset();
}

/**
 * \ingroup gdcmHeader
 * \brief   Gets (from Header) a 'non string' element value 
 * \        (LoadElementValues has already be executed)  
 * @param   Group
 * @param   Elem
 * @return Pointer to the 'non string' area
 */
void * gdcmHeader::GetPubEntryVoidAreaByNumber(guint16 Group, guint16 Elem) {
   gdcmHeaderEntry* Entry = GetHeaderEntryByNumber(Group, Elem);	 
   if (!Entry) {
      dbg.Verbose(1, "gdcmHeader::GetHeaderEntryByNumber",
                  "failed to Locate gdcmHeaderEntry");
      return (NULL);
   }
   return Entry->GetVoidArea();
}

/**
 * \ingroup       gdcmHeader
 * \brief         Loads (from disk) the element content 
 *                when a string is not suitable
 */
void * gdcmHeader::LoadEntryVoidArea(guint16 Group, guint16 Elem) {
   gdcmHeaderEntry * Element= GetHeaderEntryByNumber(Group, Elem);
   if ( !Element )
      return NULL;
   size_t o =(size_t)Element->GetOffset();
   fseek(fp, o, SEEK_SET);
   int l=Element->GetLength();
   void * a = malloc(l);
   if(!a) {
   	return NULL;
   }  
   
   PubEntrySet.SetVoidAreaByNumber(a, Group, Elem);
   // TODO check the result 
   size_t l2 = fread(a, 1, l ,fp);
   if(l != l2) {
   	free(a);
   	return NULL;
   }
   return a;  
}

//-----------------------------------------------------------------------------
// Private
/**
 * \ingroup gdcmHeader
 * \brief   Loads the element values of all the Header Entries pointed in the
 *          public Chained List.
 */
void gdcmHeader::LoadHeaderEntries(void) {
   rewind(fp);
   for (ListTag::iterator i = GetPubListEntry().begin();  
      i != GetPubListEntry().end();
      ++i){
         LoadHeaderEntry(*i);
   }   
            
   rewind(fp);

   // Load 'non string' values   
   std::string PhotometricInterpretation = GetPubEntryByNumber(0x0028,0x0004);   
   if( PhotometricInterpretation == "PALETTE COLOR " ){ 
      LoadEntryVoidArea(0x0028,0x1200);  // gray LUT   
      LoadEntryVoidArea(0x0028,0x1201);  // R    LUT
      LoadEntryVoidArea(0x0028,0x1202);  // G    LUT
      LoadEntryVoidArea(0x0028,0x1203);  // B    LUT
      
      LoadEntryVoidArea(0x0028,0x1221);  // Segmented Red   Palette Color LUT Data
      LoadEntryVoidArea(0x0028,0x1222);  // Segmented Green Palette Color LUT Data
      LoadEntryVoidArea(0x0028,0x1223);  // Segmented Blue  Palette Color LUT Data
   }

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to read ACR-LibIDO formated images
   //
   // if recognition code tells us we deal with a LibIDO image
   // we switch lineNumber and columnNumber
   //
   std::string RecCode;	
   RecCode = GetPubEntryByNumber(0x0008, 0x0010); // recognition code
   if (RecCode == "ACRNEMA_LIBIDO_1.1" ||
       RecCode == "CANRME_AILIBOD1_1." ) {
         filetype = ACR_LIBIDO; 
         std::string rows    = GetPubEntryByNumber(0x0028, 0x0010);
         std::string columns = GetPubEntryByNumber(0x0028, 0x0011);
         SetPubEntryByNumber(columns, 0x0028, 0x0010);
         SetPubEntryByNumber(rows   , 0x0028, 0x0011);
   }
   // ----------------- End of Special Patch ----------------
}

/**
 * \ingroup       gdcmHeader
 * \brief         Loads the element content if it's length is not bigger
 *                than the value specified with
 *                gdcmHeader::SetMaxSizeLoadElementValue()
 * @param        ElVal Header Entry (Dicom Element) to be dealt with
 */
void gdcmHeader::LoadHeaderEntry(gdcmHeaderEntry * ElVal) {
   size_t item_read;
   guint16 group  = ElVal->GetGroup();
   std::string  vr= ElVal->GetVR();
   guint32 length = ElVal->GetLength();
   bool SkipLoad  = false;

   fseek(fp, (long)ElVal->GetOffset(), SEEK_SET);
   
   // the test was commented out to 'go inside' the SeQuences
   // we don't any longer skip them !
    
   // if( vr == "SQ" )  //  (DO NOT remove this comment)
   //    SkipLoad = true;

   // A SeQuence "contains" a set of Elements.  
   //          (fffe e000) tells us an Element is beginning
   //          (fffe e00d) tells us an Element just ended
   //          (fffe e0dd) tells us the current SeQuence just ended
   if( group == 0xfffe )
      SkipLoad = true;

   if ( SkipLoad ) {
      ElVal->SetLength(0);
      ElVal->SetValue("gdcm::Skipped");
      return;
   }

   // When the length is zero things are easy:
   if ( length == 0 ) {
      ElVal->SetValue("");
      return;
   }

   // The elements whose length is bigger than the specified upper bound
   // are not loaded. Instead we leave a short notice of the offset of
   // the element content and it's length.
   if (length > MaxSizeLoadElementValue) {
      std::ostringstream s;
      s << "gdcm::NotLoaded.";
      s << " Address:" << (long)ElVal->GetOffset();
      s << " Length:"  << ElVal->GetLength();
      s << " x(" << std::hex << ElVal->GetLength() << ")";
      ElVal->SetValue(s.str());
      return;
   }
   
   // When integer(s) are expected, read and convert the following 
   // n *(two or four bytes)
   // properly i.e. as integers as opposed to a strings.	
   // Elements with Value Multiplicity > 1
   // contain a set of integers (not a single one) 
    	
   // Any compacter code suggested (?)
   if ( IsHeaderEntryAnInteger(ElVal) ) {
      guint32 NewInt;
      std::ostringstream s;
      int nbInt;
      if (vr == "US" || vr == "SS") {
         nbInt = length / 2;
         NewInt = ReadInt16();
         s << NewInt;
         if (nbInt > 1) {
            for (int i=1; i < nbInt; i++) {
               s << '\\';
               NewInt = ReadInt16();
               s << NewInt;
            }
         }
			
      } else if (vr == "UL" || vr == "SL") {
         nbInt = length / 4;
         NewInt = ReadInt32();
         s << NewInt;
         if (nbInt > 1) {
            for (int i=1; i < nbInt; i++) {
               s << '\\';
               NewInt = ReadInt32();
               s << NewInt;
            }
         }
      }					
#ifdef GDCM_NO_ANSI_STRING_STREAM
      s << std::ends; // to avoid oddities on Solaris
#endif //GDCM_NO_ANSI_STRING_STREAM
      ElVal->SetValue(s.str());
      return;	
   }
   
   // We need an additional byte for storing \0 that is not on disk
   char* NewValue = (char*)malloc(length+1);
   if( !NewValue) {
      dbg.Verbose(1, "LoadElementValue: Failed to allocate NewValue");
      return;
   }
   NewValue[length]= 0;
   
   item_read = fread(NewValue, (size_t)length, (size_t)1, fp);
   if ( item_read != 1 ) {
      free(NewValue);
      dbg.Verbose(1, "gdcmHeader::LoadElementValue","unread element value");
      ElVal->SetValue("gdcm::UnRead");
      return;
   }
   ElVal->SetValue(NewValue);
   free(NewValue);
}

/**
 * \ingroup       gdcmHeader
 * \brief         Loads the element while preserving the current
 *                underlying file position indicator as opposed to
 *                to LoadHeaderEntry that modifies it.
 * @param entry   Header Entry whose value shall be loaded. 
 * @return  
 */
void gdcmHeader::LoadHeaderEntrySafe(gdcmHeaderEntry * entry) {
   long PositionOnEntry = ftell(fp);
   LoadHeaderEntry(entry);
   fseek(fp, PositionOnEntry, SEEK_SET);
}

/**
 * \ingroup gdcmHeader
 * \brief   
 * @param entry   Header Entry whose value shall be loaded. 

 * @return 
 */
 void gdcmHeader::FindHeaderEntryLength (gdcmHeaderEntry * ElVal) {
   guint16 element = ElVal->GetElement();
   guint16 group   = ElVal->GetGroup();
   std::string  vr = ElVal->GetVR();
   guint16 length16;
   if( (element == 0x0010) && (group == 0x7fe0) ) {
      dbg.SetDebug(-1);
      dbg.Verbose(2, "gdcmHeader::FindLength: ",
                     "we reached 7fe0 0010");
   }   
   
   if ( (filetype == ExplicitVR) && ! ElVal->IsImplicitVr() ) {
      if ( (vr=="OB") || (vr=="OW") || (vr=="SQ") || (vr=="UN") ) {
      
         // The following reserved two bytes (see PS 3.5-2001, section
         // 7.1.2 Data element structure with explicit vr p27) must be
         // skipped before proceeding on reading the length on 4 bytes.
         fseek(fp, 2L, SEEK_CUR);

         guint32 length32 = ReadInt32();

         if ( (vr == "OB") && (length32 == 0xffffffff) ) {
            ElVal->SetLength(FindHeaderEntryLengthOB());
            return;
         }
         FixHeaderEntryFoundLength(ElVal, length32); 
         return;
      }

      // Length is encoded on 2 bytes.
      length16 = ReadInt16();
      
      // We can tell the current file is encoded in big endian (like
      // Data/US-RGB-8-epicard) when we find the "Transfer Syntax" tag
      // and it's value is the one of the encoding of a big endian file.
      // In order to deal with such big endian encoded files, we have
      // (at least) two strategies:
      // * when we load the "Transfer Syntax" tag with value of big endian
      //   encoding, we raise the proper flags. Then we wait for the end
      //   of the META group (0x0002) among which is "Transfer Syntax",
      //   before switching the swap code to big endian. We have to postpone
      //   the switching of the swap code since the META group is fully encoded
      //   in little endian, and big endian coding only starts at the next
      //   group. The corresponding code can be hard to analyse and adds
      //   many additional unnecessary tests for regular tags.
      // * the second strategy consists in waiting for trouble, that shall
      //   appear when we find the first group with big endian encoding. This
      //   is easy to detect since the length of a "Group Length" tag (the
      //   ones with zero as element number) has to be of 4 (0x0004). When we
      //   encounter 1024 (0x0400) chances are the encoding changed and we
      //   found a group with big endian encoding.
      // We shall use this second strategy. In order to make sure that we
      // can interpret the presence of an apparently big endian encoded
      // length of a "Group Length" without committing a big mistake, we
      // add an additional check: we look in the already parsed elements
      // for the presence of a "Transfer Syntax" whose value has to be "big
      // endian encoding". When this is the case, chances are we have got our
      // hands on a big endian encoded file: we switch the swap code to
      // big endian and proceed...
      if ( (element  == 0x0000) && (length16 == 0x0400) ) {
         if ( ! IsExplicitVRBigEndianTransferSyntax() ) {
            dbg.Verbose(0, "gdcmHeader::FindLength", "not explicit VR");
            errno = 1;
            return;
         }
         length16 = 4;
         SwitchSwapToBigEndian();
         // Restore the unproperly loaded values i.e. the group, the element
         // and the dictionary entry depending on them.
         guint16 CorrectGroup   = SwapShort(ElVal->GetGroup());
         guint16 CorrectElem    = SwapShort(ElVal->GetElement());
         gdcmDictEntry * NewTag = GetDictEntryByNumber(CorrectGroup,
                                                       CorrectElem);
         if (!NewTag) {
            // This correct tag is not in the dictionary. Create a new one.
            NewTag = new gdcmDictEntry(CorrectGroup, CorrectElem);
         }
         // FIXME this can create a memory leaks on the old entry that be
         // left unreferenced.
         ElVal->SetDictEntry(NewTag);
      }
       
      // Heuristic: well some files are really ill-formed.
      if ( length16 == 0xffff) {
         length16 = 0;
         //dbg.Verbose(0, "gdcmHeader::FindLength",
         //            "Erroneous element length fixed.");
         // Actually, length= 0xffff means that we deal with
         // Unknown Sequence Length 
      }

      FixHeaderEntryFoundLength(ElVal, (guint32)length16);
      return;
   }

   // Either implicit VR or a non DICOM conformal (see not below) explicit
   // VR that ommited the VR of (at least) this element. Farts happen.
   // [Note: according to the part 5, PS 3.5-2001, section 7.1 p25
   // on Data elements "Implicit and Explicit VR Data Elements shall
   // not coexist in a Data Set and Data Sets nested within it".]
   // Length is on 4 bytes.
   FixHeaderEntryFoundLength(ElVal, ReadInt32());
   return;
}

/**
 * \ingroup   gdcmHeader
 * \brief     Find the Value Representation of the current Dicom Element.
 * @param ElVal
 */
void gdcmHeader::FindHeaderEntryVR( gdcmHeaderEntry *ElVal) {
   if (filetype != ExplicitVR)
      return;

   char VR[3];
   std::string vr;
   int lgrLue;
   char msg[100]; // for sprintf. Sorry

   long PositionOnEntry = ftell(fp);
   // Warning: we believe this is explicit VR (Value Representation) because
   // we used a heuristic that found "UL" in the first tag. Alas this
   // doesn't guarantee that all the tags will be in explicit VR. In some
   // cases (see e-film filtered files) one finds implicit VR tags mixed
   // within an explicit VR file. Hence we make sure the present tag
   // is in explicit VR and try to fix things if it happens not to be
   // the case.
   bool RealExplicit = true;
   
   lgrLue=fread (&VR, (size_t)2,(size_t)1, fp);
   VR[2]=0;
   vr = std::string(VR);
      
   // Assume we are reading a falsely explicit VR file i.e. we reached
   // a tag where we expect reading a VR but are in fact we read the
   // first to bytes of the length. Then we will interogate (through find)
   // the dicom_vr dictionary with oddities like "\004\0" which crashes
   // both GCC and VC++ implementations of the STL map. Hence when the
   // expected VR read happens to be non-ascii characters we consider
   // we hit falsely explicit VR tag.

   if ( (!isalpha(VR[0])) && (!isalpha(VR[1])) )
      RealExplicit = false;

   // CLEANME searching the dicom_vr at each occurence is expensive.
   // PostPone this test in an optional integrity check at the end
   // of parsing or only in debug mode.
   if ( RealExplicit && !dicom_vr->Count(vr) )
      RealExplicit= false;

   if ( RealExplicit ) {
      if ( ElVal->IsVRUnknown() ) {
         // When not a dictionary entry, we can safely overwrite the VR.
         ElVal->SetVR(vr);
         return; 
      }
      if ( ElVal->GetVR() == vr ) {
         // The VR we just read and the dictionary agree. Nothing to do.
         return;
      }
      // The VR present in the file and the dictionary disagree. We assume
      // the file writer knew best and use the VR of the file. Since it would
      // be unwise to overwrite the VR of a dictionary (since it would
      // compromise it's next user), we need to clone the actual DictEntry
      // and change the VR for the read one.
      gdcmDictEntry* NewTag = new gdcmDictEntry(ElVal->GetGroup(),
                                 ElVal->GetElement(),
                                 vr,
                                 "FIXME",
                                 ElVal->GetName());
      ElVal->SetDictEntry(NewTag);
      return; 
   }
   
   // We thought this was explicit VR, but we end up with an
   // implicit VR tag. Let's backtrack.   
   
      sprintf(msg,"Falsely explicit vr file (%04x,%04x)\n", 
                   ElVal->GetGroup(),ElVal->GetElement());
      dbg.Verbose(1, "gdcmHeader::FindVR: ",msg);
   
   fseek(fp, PositionOnEntry, SEEK_SET);
   // When this element is known in the dictionary we shall use, e.g. for
   // the semantics (see the usage of IsAnInteger), the VR proposed by the
   // dictionary entry. Still we have to flag the element as implicit since
   // we know now our assumption on expliciteness is not furfilled.
   // avoid  .
   if ( ElVal->IsVRUnknown() )
      ElVal->SetVR("Implicit");
   ElVal->SetImplicitVr();
}

/**
 * \ingroup gdcmHeader
 * \brief   
 * @param ElVal 
 * @return 
 */
void gdcmHeader::SkipHeaderEntry(gdcmHeaderEntry * entry) {
    SkipBytes(entry->GetLength());
}

/**
 * \ingroup gdcmHeader
 * \brief   When the length of an element value is obviously wrong (because
 *          the parser went Jabberwocky) one can hope improving things by
 *          applying this heuristic.
 */
void gdcmHeader::FixHeaderEntryFoundLength(gdcmHeaderEntry * ElVal, guint32 FoundLength) {

   ElVal->SetReadLength(FoundLength); // will be updated only if a bug is found
		     
   if ( FoundLength == 0xffffffff) {  
      FoundLength = 0;
   }
      
      // Sorry for the patch!  
      // XMedCom did the trick to read some nasty GE images ...
   else if (FoundLength == 13) {
      // The following 'if' will be removed when there is no more
      // images on Creatis HDs with a 13 length for Manufacturer...
      if ( (ElVal->GetGroup() != 0x0008) ||  
           ( (ElVal->GetElement() != 0x0070) && (ElVal->GetElement() != 0x0080) ) ) {
      // end of remove area
         FoundLength =10;
         ElVal->SetReadLength(10); // a bug is to be fixed
      }
   } 
     // to fix some garbage 'Leonardo' Siemens images
     // May be commented out to avoid overhead
   else if ( (ElVal->GetGroup() == 0x0009) &&
       ( (ElVal->GetElement() == 0x1113) || (ElVal->GetElement() == 0x1114) ) ){
      FoundLength =4;
      ElVal->SetReadLength(4); // a bug is to be fixed 
   } 
     // end of fix
	 
   // to try to 'go inside' SeQuences (with length), and not to skip them        
   else if ( ElVal->GetVR() == "SQ") { 
      if (enableSequences)    // only if the user does want to !
         FoundLength =0; 	 
   } 
    
    // a SeQuence Element is beginning                                          
    // Let's forget it's length                                                 
    // (we want to 'go inside')  
    
    // Pb : *normaly*  fffe|e000 is just a marker, its length *should be* zero
    // in gdcm-MR-PHILIPS-16-Multi-Seq.dcm we find lengthes as big as 28800
    // if we set the length to zero IsHeaderEntryAnInteger() breaks...
    // if we don't, we lost 28800 characters from the Header :-(
                                                 
   else if(ElVal->GetGroup() == 0xfffe){ 
                       // sometimes, length seems to be wrong                                      
      FoundLength =0;  // some more clever checking to be done !
                       // I give up!
		       // only  gdcm-MR-PHILIPS-16-Multi-Seq.dcm
		       // causes troubles :-(                                                     
   }     
    
   ElVal->SetUsableLength(FoundLength);
}

/**
 * \ingroup gdcmHeader
 * \brief   Apply some heuristics to predict wether the considered 
 *          element value contains/represents an integer or not.
 * @param   ElVal The element value on which to apply the predicate.
 * @return  The result of the heuristical predicate.
 */
bool gdcmHeader::IsHeaderEntryAnInteger(gdcmHeaderEntry * ElVal) {
   guint16 element = ElVal->GetElement();
   guint16 group   = ElVal->GetGroup();
   std::string  vr = ElVal->GetVR();
   guint32 length  = ElVal->GetLength();

   // When we have some semantics on the element we just read, and if we
   // a priori know we are dealing with an integer, then we shall be
   // able to swap it's element value properly.
   if ( element == 0 )  {  // This is the group length of the group
      if (length == 4)
         return true;
      else {
         std::ostringstream s;
         s << "Erroneous Group Length element length  on :" \
           << std::hex << group << " , " << element;
         dbg.Error("gdcmHeader::IsAnInteger",
            s.str().c_str());     
      }
   }
   if ( (vr == "UL") || (vr == "US") || (vr == "SL") || (vr == "SS") )
      return true;
   
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   
 *
 * @return 
 */
 guint32 gdcmHeader::FindHeaderEntryLengthOB(void) {
   // See PS 3.5-2001, section A.4 p. 49 on encapsulation of encoded pixel data.
   guint16 g;
   guint16 n; 
   long PositionOnEntry = ftell(fp);
   bool FoundSequenceDelimiter = false;
   guint32 TotalLength = 0;
   guint32 ItemLength;

   while ( ! FoundSequenceDelimiter) {
      g = ReadInt16();
      n = ReadInt16();   
      if (errno == 1)
         return 0;
      TotalLength += 4;  // We even have to decount the group and element 
     
      if ( g != 0xfffe && g!=0xb00c ) /*for bogus header */ {
         char msg[100]; // for sprintf. Sorry
         sprintf(msg,"wrong group (%04x) for an item sequence (%04x,%04x)\n",g, g,n);
         dbg.Verbose(1, "gdcmHeader::FindLengthOB: ",msg); 
         errno = 1;
         return 0;
      }
      if ( n == 0xe0dd || ( g==0xb00c && n==0x0eb6 ) ) /* for bogus header  */ 
         FoundSequenceDelimiter = true;
      else if ( n != 0xe000 ){
         char msg[100];  // for sprintf. Sorry
         sprintf(msg,"wrong element (%04x) for an item sequence (%04x,%04x)\n",
                      n, g,n);
	 dbg.Verbose(1, "gdcmHeader::FindLengthOB: ",msg);
         errno = 1;
         return 0;
      }
      ItemLength = ReadInt32();
      TotalLength += ItemLength + 4;  // We add 4 bytes since we just read
                                      // the ItemLength with ReadInt32                                     
      SkipBytes(ItemLength);
   }
   fseek(fp, PositionOnEntry, SEEK_SET);
   return TotalLength;
}

/**
 * \ingroup gdcmHeader
 * \brief Reads a supposed to be 16 Bits integer
 * \     (swaps it depending on processor endianity) 
 *
 * @return integer acts as a boolean
 */
guint16 gdcmHeader::ReadInt16(void) {
   guint16 g;
   size_t item_read;
   item_read = fread (&g, (size_t)2,(size_t)1, fp);
   if ( item_read != 1 ) {
      // dbg.Verbose(0, "gdcmHeader::ReadInt16", " Failed to read :");
      // if(feof(fp)) 
      //    dbg.Verbose(0, "gdcmHeader::ReadInt16", " End of File encountered");
      if(ferror(fp)) 
         dbg.Verbose(0, "gdcmHeader::ReadInt16", " File Error");
      errno = 1;
      return 0;
   }
   errno = 0;
   g = SwapShort(g);
   return g;
}

/**
 * \ingroup gdcmHeader
 * \brief  Reads a supposed to be 32 Bits integer
 * \       (swaps it depending on processor endianity)  
 *
 * @return 
 */
guint32 gdcmHeader::ReadInt32(void) {
   guint32 g;
   size_t item_read;
   item_read = fread (&g, (size_t)4,(size_t)1, fp);
   if ( item_read != 1 ) { 
      //dbg.Verbose(0, "gdcmHeader::ReadInt32", " Failed to read :");
      //if(feof(fp)) 
      //   dbg.Verbose(0, "gdcmHeader::ReadInt32", " End of File encountered");
     if(ferror(fp)) 
         dbg.Verbose(0, "gdcmHeader::ReadInt32", " File Error");   
      errno = 1;
      return 0;
   }
   errno = 0;   
   g = SwapLong(g);
   return g;
}

/**
 * \ingroup gdcmHeader
 * \brief   
 *
 * @return 
 */
void gdcmHeader::SkipBytes(guint32 NBytes) {
   //FIXME don't dump the returned value
   (void)fseek(fp, (long)NBytes, SEEK_CUR);
}

/**
 * \ingroup gdcmHeader
 * \brief   
 */
void gdcmHeader::Initialise(void) {
   dicom_vr = gdcmGlobal::GetVR();
   dicom_ts = gdcmGlobal::GetTS();
   Dicts    = gdcmGlobal::GetDicts();
   RefPubDict = Dicts->GetDefaultPubDict();
   RefShaDict = (gdcmDict*)0;
}

/**
 * \ingroup gdcmHeader
 * \brief   Discover what the swap code is (among little endian, big endian,
 *          bad little endian, bad big endian).
 *
 */
void gdcmHeader::CheckSwap()
{
   // Fourth semantics:
   //
   // ---> Warning : This fourth field is NOT part 
   //                of the 'official' Dicom Dictionnary
   //                and should NOT be used.
   //                (Not defined for all the groups
   //                 may be removed in a future release)
   //
   // CMD      Command        
   // META     Meta Information 
   // DIR      Directory
   // ID
   // PAT      Patient
   // ACQ      Acquisition
   // REL      Related
   // IMG      Image
   // SDY      Study
   // VIS      Visit 
   // WAV      Waveform
   // PRC
   // DEV      Device
   // NMI      Nuclear Medicine
   // MED
   // BFS      Basic Film Session
   // BFB      Basic Film Box
   // BIB      Basic Image Box
   // BAB
   // IOB
   // PJ
   // PRINTER
   // RT       Radio Therapy
   // DVH   
   // SSET
   // RES      Results
   // CRV      Curve
   // OLY      Overlays
   // PXL      Pixels
   // DL       Delimiters
   //

   // The only guaranted way of finding the swap code is to find a
   // group tag since we know it's length has to be of four bytes i.e.
   // 0x00000004. Finding the swap code in then straigthforward. Trouble
   // occurs when we can't find such group...
   guint32  s;
   guint32  x=4;  // x : for ntohs
   bool net2host; // true when HostByteOrder is the same as NetworkByteOrder
    
   int lgrLue;
   char * entCur;
   char deb[HEADER_LENGTH_TO_READ];
    
   // First, compare HostByteOrder and NetworkByteOrder in order to
   // determine if we shall need to swap bytes (i.e. the Endian type).
   if (x==ntohs(x))
      net2host = true;
   else
      net2host = false; 
    //cout << net2host << endl;
         
   // The easiest case is the one of a DICOM header, since it possesses a
   // file preamble where it suffice to look for the string "DICM".
   lgrLue = fread(deb, 1, HEADER_LENGTH_TO_READ, fp);
   
   entCur = deb + 128;
   if(memcmp(entCur, "DICM", (size_t)4) == 0) {
      dbg.Verbose(1, "gdcmHeader::CheckSwap:", "looks like DICOM Version3");
      // Next, determine the value representation (VR). Let's skip to the
      // first element (0002, 0000) and check there if we find "UL" 
      // - or "OB" if the 1st one is (0002,0001) -,
      // in which case we (almost) know it is explicit VR.
      // WARNING: if it happens to be implicit VR then what we will read
      // is the length of the group. If this ascii representation of this
      // length happens to be "UL" then we shall believe it is explicit VR.
      // FIXME: in order to fix the above warning, we could read the next
      // element value (or a couple of elements values) in order to make
      // sure we are not commiting a big mistake.
      // We need to skip :
      // * the 128 bytes of File Preamble (often padded with zeroes),
      // * the 4 bytes of "DICM" string,
      // * the 4 bytes of the first tag (0002, 0000),or (0002, 0001)
      // i.e. a total of  136 bytes.
      entCur = deb + 136;
      // FIXME
      // Use gdcmHeader::dicom_vr to test all the possibilities
      // instead of just checking for UL, OB and UI !?
      if(  (memcmp(entCur, "UL", (size_t)2) == 0) ||
      	   (memcmp(entCur, "OB", (size_t)2) == 0) ||
      	   (memcmp(entCur, "UI", (size_t)2) == 0) )   
      {
         filetype = ExplicitVR;
         dbg.Verbose(1, "gdcmHeader::CheckSwap:",
                     "explicit Value Representation");
      } else {
         filetype = ImplicitVR;
         dbg.Verbose(1, "gdcmHeader::CheckSwap:",
                     "not an explicit Value Representation");
      }
      if (net2host) {
         sw = 4321;
         dbg.Verbose(1, "gdcmHeader::CheckSwap:",
                        "HostByteOrder != NetworkByteOrder");
      } else {
         sw = 0;
         dbg.Verbose(1, "gdcmHeader::CheckSwap:",
                        "HostByteOrder = NetworkByteOrder");
      }
      
      // Position the file position indicator at first tag (i.e.
      // after the file preamble and the "DICM" string).
      rewind(fp);
      fseek (fp, 132L, SEEK_SET);
      return;
   } // End of DicomV3

   // Alas, this is not a DicomV3 file and whatever happens there is no file
   // preamble. We can reset the file position indicator to where the data
   // is (i.e. the beginning of the file).
    dbg.Verbose(1, "gdcmHeader::CheckSwap:", "not a DICOM Version3 file");
   rewind(fp);

   // Our next best chance would be to be considering a 'clean' ACR/NEMA file.
   // By clean we mean that the length of the first tag is written down.
   // If this is the case and since the length of the first group HAS to be
   // four (bytes), then determining the proper swap code is straightforward.

   entCur = deb + 4;
   // We assume the array of char we are considering contains the binary
   // representation of a 32 bits integer. Hence the following dirty
   // trick :
   s = *((guint32 *)(entCur));
   
   switch (s) {
   case 0x00040000 :
      sw = 3412;
      filetype = ACR;
      return;
   case 0x04000000 :
      sw = 4321;
      filetype = ACR;
      return;
   case 0x00000400 :
      sw = 2143;
      filetype = ACR;
      return;
   case 0x00000004 :
      sw = 0;
      filetype = ACR;
      return;
   default :
      dbg.Verbose(0, "gdcmHeader::CheckSwap:",
                     "ACR/NEMA unfound swap info (time to raise bets)");
   }

   // We are out of luck. It is not a DicomV3 nor a 'clean' ACR/NEMA file.
   // It is time for despaired wild guesses. So, let's assume this file
   // happens to be 'dirty' ACR/NEMA, i.e. the length of the group is
   // not present. Then the only info we have is the net2host one.
   filetype = Unknown;
   if (! net2host )
      sw = 0;
   else
      sw = 4321;
   return;
}

/**
 * \ingroup gdcmHeader
 * \brief   
 */
void gdcmHeader::SwitchSwapToBigEndian(void) {
   dbg.Verbose(1, "gdcmHeader::SwitchSwapToBigEndian",
                  "Switching to BigEndian mode.");
   if ( sw == 0    ) {
      sw = 4321;
      return;
   }
   if ( sw == 4321 ) {
      sw = 0;
      return;
   }
   if ( sw == 3412 ) {
      sw = 2143;
      return;
   }
   if ( sw == 2143 )
      sw = 3412;
}

/**
 * \ingroup gdcmHeader
 * \brief   
 * @param NewSize
 * @return 
 */
void gdcmHeader::SetMaxSizeLoadElementValue(long NewSize) {
   if (NewSize < 0)
      return;
   if ((guint32)NewSize >= (guint32)0xffffffff) {
      MaxSizeLoadElementValue = 0xffffffff;
      return;
   }
   MaxSizeLoadElementValue = NewSize;
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches both the public and the shadow dictionary (when they
 *          exist) for the presence of the DictEntry with given
 *          group and element. The public dictionary has precedence on the
 *          shadow one.
 * @param   group   group of the searched DictEntry
 * @param   element element of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 */
gdcmDictEntry * gdcmHeader::GetDictEntryByNumber(guint16 group,
                                                 guint16 element) {
   gdcmDictEntry * found = (gdcmDictEntry*)0;
   if (!RefPubDict && !RefShaDict) {
      dbg.Verbose(0, "gdcmHeader::GetDictEntry",
                     "we SHOULD have a default dictionary");
   }
   if (RefPubDict) {
      found = RefPubDict->GetTagByNumber(group, element);
      if (found)
         return found;
   }
   if (RefShaDict) {
      found = RefShaDict->GetTagByNumber(group, element);
      if (found)
         return found;
   }
   return found;
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches both the public and the shadow dictionary (when they
 *          exist) for the presence of the DictEntry with given name.
 *          The public dictionary has precedence on the shadow one.
 * @param   Name name of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 */
gdcmDictEntry * gdcmHeader::GetDictEntryByName(std::string Name) {
   gdcmDictEntry * found = (gdcmDictEntry*)0;
   if (!RefPubDict && !RefShaDict) {
      dbg.Verbose(0, "gdcmHeader::GetDictEntry",
                     "we SHOULD have a default dictionary");
   }
   if (RefPubDict) {
      found = RefPubDict->GetTagByName(Name);
      if (found)
         return found;
   }
   if (RefShaDict) {
      found = RefShaDict->GetTagByName(Name);
      if (found)
         return found;
   }
   return found;
}

/**
 * \ingroup gdcmHeader
 * \brief   Read the next tag but WITHOUT loading it's value
 * @return  On succes the newly created HeaderEntry, NULL on failure.      
 */
gdcmHeaderEntry * gdcmHeader::ReadNextHeaderEntry(void) {
  
   guint16 g,n;
   gdcmHeaderEntry * NewElVal;
   
   g = ReadInt16();
   n = ReadInt16();
      
   if (errno == 1)
      // We reached the EOF (or an error occured) and header parsing
      // has to be considered as finished.
      return (gdcmHeaderEntry *)0;
   
   NewElVal = NewHeaderEntryByNumber(g, n);
   FindHeaderEntryVR(NewElVal);
   FindHeaderEntryLength(NewElVal);
	
   if (errno == 1) {
      // Call it quits
      return (gdcmHeaderEntry *)0;
   }
   NewElVal->SetOffset(ftell(fp));  
   //if ( (g==0x7fe0) && (n==0x0010) ) 
   return NewElVal;
}

/**
 * \ingroup gdcmHeader
 * \brief   Build a new Element Value from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Name    Name of the underlying DictEntry
 */
gdcmHeaderEntry* gdcmHeader::NewHeaderEntryByName(std::string Name) {

   gdcmDictEntry * NewTag = GetDictEntryByName(Name);
   if (!NewTag)
      NewTag = new gdcmDictEntry(0xffff, 0xffff, "LO", "Unknown", Name);

   gdcmHeaderEntry* NewElVal = new gdcmHeaderEntry(NewTag);
   if (!NewElVal) {
      dbg.Verbose(1, "gdcmHeader::ObtainHeaderEntryByName",
                  "failed to allocate gdcmHeaderEntry");
      return (gdcmHeaderEntry*)0;
   }
   return NewElVal;
}  

/**
 * \ingroup gdcmHeader
 * \brief   Build a new Element Value from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Group group   of the underlying DictEntry
 * @param   Elem  element of the underlying DictEntry
 */
gdcmHeaderEntry* gdcmHeader::NewHeaderEntryByNumber(guint16 Group, guint16 Elem) {
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDictEntry * NewTag = GetDictEntryByNumber(Group, Elem);
   if (!NewTag)
      NewTag = new gdcmDictEntry(Group, Elem);

   gdcmHeaderEntry* NewElVal = new gdcmHeaderEntry(NewTag);
   if (!NewElVal) {
      dbg.Verbose(1, "gdcmHeader::NewHeaderEntryByNumber",
                  "failed to allocate gdcmHeaderEntry");
      return (gdcmHeaderEntry*)0;
   }
   return NewElVal;
}

/**
 * \ingroup gdcmHeader
 * \brief   Small utility function that creates a new manually crafted
 *          (as opposed as read from the file) gdcmHeaderEntry with user
 *          specified name and adds it to the public tag hash table.
 * \note    A fake TagKey is generated so the PubDict can keep it's coherence.
 * @param   NewTagName The name to be given to this new tag.
 * @param   VR The Value Representation to be given to this new tag.
 * @return  The newly hand crafted Element Value.
 */
gdcmHeaderEntry* gdcmHeader::NewManualHeaderEntryToPubDict(std::string NewTagName, 
                                                           std::string VR) {
   gdcmHeaderEntry* NewElVal = (gdcmHeaderEntry*)0;
   guint32 StuffGroup = 0xffff;   // Group to be stuffed with additional info
   guint32 FreeElem = 0;
   gdcmDictEntry* NewEntry = (gdcmDictEntry*)0;

   FreeElem = PubEntrySet.GenerateFreeTagKeyInGroup(StuffGroup);
   if (FreeElem == UINT32_MAX) {
      dbg.Verbose(1, "gdcmHeader::NewManualElValToPubDict",
                     "Group 0xffff in Public Dict is full");
      return (gdcmHeaderEntry*)0;
   }
   NewEntry = new gdcmDictEntry(StuffGroup, FreeElem,
                                VR, "GDCM", NewTagName);
   NewElVal = new gdcmHeaderEntry(NewEntry);
   PubEntrySet.Add(NewElVal);
   return NewElVal;
}

//-----------------------------------------------------------------------------
