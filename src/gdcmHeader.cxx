// $Header: /cvs/public/gdcm/src/Attic/gdcmHeader.cxx,v 1.90 2003/09/24 16:18:32 jpr Exp $

//This is needed when compiling in debug mode
#ifdef _MSC_VER
//'identifier' : not all control paths return a value
//#pragma warning ( disable : 4715 )
// 'identifier' : class 'type' needs to have dll-interface to be used by
// clients of class 'type2'
#pragma warning ( disable : 4251 )
// 'identifier' : identifier was truncated to 'number' characters in the
// debug information
#pragma warning ( disable : 4786 )
#endif //_MSC_VER

#include <stdio.h>
#include <cerrno>
// For nthos:
#ifdef _MSC_VER
#include <winsock.h>
#else
#include <netinet/in.h>
#endif
#include <cctype>    // for isalpha
#include <sstream>
#include "gdcmUtil.h"
#include "gdcmHeader.h"
#include "gdcmTS.h"

// Refer to gdcmHeader::CheckSwap()
#define HEADER_LENGTH_TO_READ       256
// Refer to gdcmHeader::SetMaxSizeLoadElementValue()
//#define _MaxSizeLoadElementValue_   1024
#define _MaxSizeLoadElementValue_   4096
/**
 * \ingroup gdcmHeader
 * \brief   
 */
void gdcmHeader::Initialise(void) {
   dicom_vr = gdcmGlobal::GetVR();
   dicom_ts = gdcmGlobal::GetTS();
   Dicts =    gdcmGlobal::GetDicts();
   RefPubDict = Dicts->GetDefaultPubDict();
   RefShaDict = (gdcmDict*)0;
}

/**
 * \ingroup gdcmHeader
 * \brief   
 * @param   InFilename
 * @param   exception_on_error
 */
gdcmHeader::gdcmHeader(const char *InFilename, bool exception_on_error) {
   SetMaxSizeLoadElementValue(_MaxSizeLoadElementValue_);
   filename = InFilename;
   Initialise();
   if ( !OpenFile(exception_on_error))
      return;
printf ("avant ParseHeader\n");
   ParseHeader();
printf ("avant LoadElements\n");

   LoadElements();
   CloseFile();
}

/**
 * \ingroup gdcmHeader
 * \brief   
 * @param   exception_on_error
 */
gdcmHeader::gdcmHeader(bool exception_on_error) {
  SetMaxSizeLoadElementValue(_MaxSizeLoadElementValue_);
  Initialise();
}

/**
 * \ingroup gdcmHeader
 * \brief   
 * @param   exception_on_error
 * @return  
 */
 bool gdcmHeader::OpenFile(bool exception_on_error)
  throw(gdcmFileError) {
  fp=fopen(filename.c_str(),"rb");
  if(exception_on_error) {
    if(!fp)
      throw gdcmFileError("gdcmHeader::gdcmHeader(const char *, bool)");
  }
  if ( fp )
     return true;
  dbg.Verbose(0, "gdcmHeader::gdcmHeader cannot open file", filename.c_str());
  return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   
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
 * \brief   Canonical destructor.
 */
gdcmHeader::~gdcmHeader (void) {
  dicom_vr =   (gdcmVR*)0; 
  Dicts    =   (gdcmDictSet*)0;
  RefPubDict = (gdcmDict*)0;
  RefShaDict = (gdcmDict*)0;
  return;
}

// Fourth semantics:
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

/**
 * \ingroup gdcmHeader
 * \brief   Discover what the swap code is (among little endian, big endian,
 *          bad little endian, bad big endian).
 *
 */
void gdcmHeader::CheckSwap()
{
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
 * \ingroup   gdcmHeader
 * \brief     Find the value representation of the current tag.
 * @param ElVal
 */
void gdcmHeader::FindVR( gdcmElValue *ElVal) {
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
      if ( ElVal->IsVrUnknown() ) {
         // When not a dictionary entry, we can safely overwrite the vr.
         ElVal->SetVR(vr);
         return; 
      }
      if ( ElVal->GetVR() == vr ) {
         // The vr we just read and the dictionary agree. Nothing to do.
         return;
      }
      // The vr present in the file and the dictionary disagree. We assume
      // the file writer knew best and use the vr of the file. Since it would
      // be unwise to overwrite the vr of a dictionary (since it would
      // compromise it's next user), we need to clone the actual DictEntry
      // and change the vr for the read one.
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
   
      sprintf(msg,"Falsely explicit vr file (%04x,%04x)\n", ElVal->GetGroup(),ElVal->GetElement());
      dbg.Verbose(1, "gdcmHeader::FindVR: ",msg);
   
   fseek(fp, PositionOnEntry, SEEK_SET);
   // When this element is known in the dictionary we shall use, e.g. for
   // the semantics (see  the usage of IsAnInteger), the vr proposed by the
   // dictionary entry. Still we have to flag the element as implicit since
   // we know now our assumption on expliciteness is not furfilled.
   // avoid  .
   if ( ElVal->IsVrUnknown() )
      ElVal->SetVR("Implicit");
   ElVal->SetImplicitVr();
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a ImplicitVRLittleEndian one.
 *
 * @return  True when ImplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmHeader::IsImplicitVRLittleEndianTransferSyntax(void) {
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
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
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
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
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
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
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
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
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.4.50" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   
 *
 * @return 
 */
bool gdcmHeader::IsJPEGLossless(void) {
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
    // faire qq chose d'intelligent a la place de ça
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
   const char * Transfert = Element->GetValue().c_str();
   if ( memcmp(Transfert+strlen(Transfert)-2 ,"70",2)==0) return true;
   if ( memcmp(Transfert+strlen(Transfert)-2 ,"55",2)==0) return true;
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
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
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
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
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
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
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
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.5" )
      return true;
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
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
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
   if (   (filetype == ExplicitVR)
       || (filetype == ImplicitVR) )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   When the length of an element value is obviously wrong (because
 *          the parser went Jabberwocky) one can hope improving things by
 *          applying this heuristic.
 */
void gdcmHeader::FixFoundLength(gdcmElValue * ElVal, guint32 FoundLength) {
   if ( FoundLength == 0xffffffff)
      FoundLength = 0;
   ElVal->SetLength(FoundLength);
}

/**
 * \ingroup gdcmHeader
 * \brief   
 *
 * @return 
 */
 guint32 gdcmHeader::FindLengthOB(void) {
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
      
      long l = ftell(fp);

      if (errno == 1)
         return 0;
      TotalLength += 4;  // We even have to decount the group and element 
     
      if ( g != 0xfffe           && g!=0xb00c ) /*for bogus header */ {
         char msg[100]; // for sprintf. Sorry
         sprintf(msg,"wrong group (%04x) for an item sequence (%04x,%04x)\n",g, g,n);
         dbg.Verbose(1, "gdcmHeader::FindLengthOB: ",msg); 
         long l = ftell(fp);
         errno = 1;
         return 0;
      }
      if ( n == 0xe0dd       || ( g==0xb00c && n==0x0eb6 ) ) /* for bogus header  */ 
         FoundSequenceDelimiter = true;
      else if ( n != 0xe000 ){
         char msg[100];  // for sprintf. Sorry
         sprintf(msg,"wrong element (%04x) for an item sequence (%04x,%04x)\n",n, g,n);
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
 * \brief   
 *
 * @return 
 */
 void gdcmHeader::FindLength (gdcmElValue * ElVal) {
   guint16 element = ElVal->GetElement();
   guint16 group   = ElVal->GetGroup();
   std::string  vr      = ElVal->GetVR();
   guint16 length16;
   if( (element == 0x0010) && (group == 0x7fe0) ) {
      dbg.SetDebug(0);
      dbg.Verbose(2, "gdcmHeader::FindLength: ",
                     "on est sur 7fe0 0010");
   }   
   
   if ( (filetype == ExplicitVR) && ! ElVal->IsImplicitVr() ) {
      if ( (vr=="OB") || (vr=="OW") || (vr=="SQ") || (vr=="UN") ) {
      
         // The following reserved two bytes (see PS 3.5-2001, section
         // 7.1.2 Data element structure with explicit vr p27) must be
         // skipped before proceeding on reading the length on 4 bytes.
         fseek(fp, 2L, SEEK_CUR);

         guint32 length32 = ReadInt32();
         if ( (vr == "OB") && (length32 == 0xffffffff) ) {
            ElVal->SetLength(FindLengthOB());
            return;
         }
         FixFoundLength(ElVal, length32);        
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
      //   encouter 1024 (0x0400) chances are the encoding changed and we
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
         dbg.Verbose(0, "gdcmHeader::FindLength",
                     "Erroneous element length fixed.");
      }
      FixFoundLength(ElVal, (guint32)length16);
      return;
   }

   // Either implicit VR or a non DICOM conformal (see not below) explicit
   // VR that ommited the VR of (at least) this element. Farts happen.
   // [Note: according to the part 5, PS 3.5-2001, section 7.1 p25
   // on Data elements "Implicit and Explicit VR Data Elements shall
   // not coexist in a Data Set and Data Sets nested within it".]
   // Length is on 4 bytes.
   FixFoundLength(ElVal, ReadInt32());
}

/**
 * \ingroup gdcmHeader
 * \brief   Swaps back the bytes of 4-byte long integer accordingly to
 *          processor order.
 *
 * @return  The suggested integer.
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
 * @param ElVal 
 * @return 
 */
 void gdcmHeader::SkipElementValue(gdcmElValue * ElVal) {
   SkipBytes(ElVal->GetLength());
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
 * \ingroup       gdcmHeader
 * \brief         Loads the element content if it's length is not bigger
 *                than the value specified with
 *                gdcmHeader::SetMaxSizeLoadElementValue()
 */
void gdcmHeader::LoadElementValue(gdcmElValue * ElVal) {
   size_t item_read;
   guint16 group  = ElVal->GetGroup();
   std::string  vr     = ElVal->GetVR();
   guint32 length = ElVal->GetLength();
   bool SkipLoad  = false;

   fseek(fp, (long)ElVal->GetOffset(), SEEK_SET);
   
   // FIXME Sequences not treated yet !
   //
   // Ne faudrait-il pas au contraire trouver immediatement
   // une maniere 'propre' de traiter les sequences (vr = SQ)
   // car commencer par les ignorer risque de conduire a qq chose
   // qui pourrait ne pas etre generalisable
   // Well, I'm expecting your code !!!
    
   if( vr == "SQ" )
      SkipLoad = true;

   // Heuristic : a sequence "contains" a set of tags (called items). It looks
   // like the last tag of a sequence (the one that terminates the sequence)
   // has a group of 0xfffe (with a dummy length).
   // Well ... 
   // Actually (fffe e000) tells us an Element is beginning
   //          (fffe e00d) tells us an Element just ended
   //          (fffe e0dd) tells us the current SEQuence just ended
  
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
      ElVal->SetValue(s.str());
      return;
   }
   
   // When an integer is expected, read and convert the following two or
   // four bytes properly i.e. as an integer as opposed to a string.
	
	// pour les elements de Value Multiplicity > 1
	// on aura en fait une serie d'entiers	
	// on devrait pouvoir faire + compact (?)
		
   if ( IsAnInteger(ElVal) ) {
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
 *                to LoadElementValue that modifies it.
 * @param ElVal   Element whose value shall be loaded. 
 * @return  
 */
void gdcmHeader::LoadElementValueSafe(gdcmElValue * ElVal) {
   long PositionOnEntry = ftell(fp);
   LoadElementValue(ElVal);
   fseek(fp, PositionOnEntry, SEEK_SET);
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
 gdcmElValue* gdcmHeader::GetElValueByNumber(guint16 Group, guint16 Elem) {

   gdcmElValue* elValue = PubElValSet.GetElementByNumber(Group, Elem);	 
   if (!elValue) {
      dbg.Verbose(1, "gdcmHeader::GetElValueByNumber",
                  "failed to Locate gdcmElValue");
      return (gdcmElValue*)0;
   }
   return elValue;
}

/**
 * \ingroup gdcmHeader
 * \brief   Build a new Element Value from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Group group   of the underlying DictEntry
 * @param   Elem  element of the underlying DictEntry
 */
gdcmElValue* gdcmHeader::NewElValueByNumber(guint16 Group, guint16 Elem) {
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDictEntry * NewTag = GetDictEntryByNumber(Group, Elem);
   if (!NewTag)
      NewTag = new gdcmDictEntry(Group, Elem);

   gdcmElValue* NewElVal = new gdcmElValue(NewTag);
   if (!NewElVal) {
      dbg.Verbose(1, "gdcmHeader::NewElValueByNumber",
                  "failed to allocate gdcmElValue");
      return (gdcmElValue*)0;
   }
   return NewElVal;
}

/**
 * \ingroup gdcmHeader
 * \brief   TODO
 * @param   Value
 * @param   Group
 * @param   Elem
 * \return integer acts as a boolean
 */
int gdcmHeader::ReplaceOrCreateByNumber(std::string Value, guint16 Group, guint16 Elem ) {

	// TODO : FIXME JPRx
	// curieux, non ?
	// on (je) cree une Elvalue ne contenant pas de valeur
	// on l'ajoute au ElValSet
	// on affecte une valeur a cette ElValue a l'interieur du ElValSet
	// --> devrait pouvoir etre fait + simplement ???
	
   gdcmElValue* nvElValue=NewElValueByNumber(Group, Elem);
   PubElValSet.Add(nvElValue);	
   PubElValSet.SetElValueByNumber(Value, Group, Elem);
   return(1);
}   


/**
 * \ingroup gdcmHeader
 * \brief   Modify or (Creates if not found) an element
 * @param   Value new value
 * @param   Group
 * @param   Elem
 * \return integer acts as a boolean 
 * 
 */
int gdcmHeader::ReplaceOrCreateByNumber(char* Value, guint16 Group, guint16 Elem ) {

   gdcmElValue* nvElValue=NewElValueByNumber(Group, Elem);
   PubElValSet.Add(nvElValue);
   std::string v = Value;	
   PubElValSet.SetElValueByNumber(v, Group, Elem);
   return(1);
}  


/**
 * \ingroup gdcmHeader
 * \brief   Set a new value if the invoked element exists
 * @param   Value
 * @param   Group
 * @param   Elem
 * \return integer acts as a boolean 
 */
int gdcmHeader::ReplaceIfExistByNumber(char* Value, guint16 Group, guint16 Elem ) {

   gdcmElValue* elValue = PubElValSet.GetElementByNumber(Group, Elem);
   std::string v = Value;	
   PubElValSet.SetElValueByNumber(v, Group, Elem);
   return 1;
} 


/**
 * \ingroup gdcmHeader
 * \brief   Checks if a given ElValue (group,number) 
 * \ exists in the Public ElValSet
 * @param   Group
 * @param   Elem
 * @return  integer acts as a boolean  
 */
 
int gdcmHeader::CheckIfExistByNumber(guint16 Group, guint16 Elem ) {
   return (PubElValSet.CheckIfExistByNumber(Group, Elem));
 }
  
/**
 * \ingroup gdcmHeader
 * \brief   Build a new Element Value from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Name    Name of the underlying DictEntry
 */
gdcmElValue* gdcmHeader::NewElValueByName(std::string Name) {

   gdcmDictEntry * NewTag = GetDictEntryByName(Name);
   if (!NewTag)
      NewTag = new gdcmDictEntry(0xffff, 0xffff, "LO", "Unknown", Name);

   gdcmElValue* NewElVal = new gdcmElValue(NewTag);
   if (!NewElVal) {
      dbg.Verbose(1, "gdcmHeader::ObtainElValueByName",
                  "failed to allocate gdcmElValue");
      return (gdcmElValue*)0;
   }
   return NewElVal;
}  

/**
 * \ingroup gdcmHeader
 * \brief   Read the next tag but WITHOUT loading it's value
 * @return  On succes the newly created ElValue, NULL on failure.      
 */
gdcmElValue * gdcmHeader::ReadNextElement(void) {
  
   guint16 g,n;
   gdcmElValue * NewElVal;
   
   g = ReadInt16();
   n = ReadInt16();
   
   //if ( (g==0x7fe0) && (n==0x0010) ) 
   
   if (errno == 1)
      // We reached the EOF (or an error occured) and header parsing
      // has to be considered as finished.
      return (gdcmElValue *)0;
   
   NewElVal = NewElValueByNumber(g, n);
   FindVR(NewElVal);
   FindLength(NewElVal);
   if (errno == 1) {
      // Call it quits
      return (gdcmElValue *)0;
   }
   NewElVal->SetOffset(ftell(fp));  
   //if ( (g==0x7fe0) && (n==0x0010) ) 
   return NewElVal;
}

/**
 * \ingroup gdcmHeader
 * \brief   Apply some heuristics to predict wether the considered 
 *          element value contains/represents an integer or not.
 * @param   ElVal The element value on which to apply the predicate.
 * @return  The result of the heuristical predicate.
 */
bool gdcmHeader::IsAnInteger(gdcmElValue * ElVal) {
   guint16 group   = ElVal->GetGroup();
   guint16 element = ElVal->GetElement();
   std::string  vr      = ElVal->GetVR();
   guint32 length  = ElVal->GetLength();

   // When we have some semantics on the element we just read, and if we
   // a priori know we are dealing with an integer, then we shall be
   // able to swap it's element value properly.
   if ( element == 0 )  {  // This is the group length of the group
      if (length == 4)
         return true;
      else {
         dbg.Error("gdcmHeader::IsAnInteger",
            "Erroneous Group Length element length.");     
      }
   }
   if ( (vr == "UL") || (vr == "US") || (vr == "SL") || (vr == "SS") )
      return true;
   
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Recover the offset (from the beginning of the file) of the pixels.
 */
size_t gdcmHeader::GetPixelOffset(void) {
   // If this file complies with the norm we should encounter the
   // "Image Location" tag (0x0028,  0x0200). This tag contains the
   // the group that contains the pixel data (hence the "Pixel Data"
   // is found by indirection through the "Image Location").
   // Inside the group pointed by "Image Location" the searched element
   // is conventionally the element 0x0010 (when the norm is respected).
   // When the "Image Location" is absent we default to group 0x7fe0.
   guint16 grPixel;
   guint16 numPixel;
   std::string ImageLocation = GetPubElValByName("Image Location");
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
         
   gdcmElValue* PixelElement = PubElValSet.GetElementByNumber(grPixel,
                                                              numPixel);
   if (PixelElement)
      return PixelElement->GetOffset();
   else
      return 0;
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
 * \brief   Searches within the public dictionary for element value of
 *          a given tag.
 * @param   group Group of the researched tag.
 * @param   element Element of the researched tag.
 * @return  Corresponding element value when it exists, and the string
 *          GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetPubElValByNumber(guint16 group, guint16 element) {
   return PubElValSet.GetElValueByNumber(group, element);
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
std::string gdcmHeader::GetPubElValRepByNumber(guint16 group, guint16 element) {
   gdcmElValue* elem =  PubElValSet.GetElementByNumber(group, element);
   if ( !elem )
      return GDCM_UNFOUND;
   return elem->GetVR();
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within the public dictionary for element value of
 *          a given tag.
 * @param   TagName name of the researched element.
 * @return  Corresponding element value when it exists, and the string
 *          GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetPubElValByName(std::string TagName) {
   return PubElValSet.GetElValueByName(TagName);
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within the elements parsed with the public dictionary for
 *          the element value representation of a given tag.
 *
 *          Obtaining the VR (Value Representation) might be needed by caller
 *          to convert the string typed content to caller's native type 
 *          (think of C++ vs Python). The VR is actually of a higher level
 *          of semantics than just the native C++ type.
 * @param   TagName name of the researched element.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetPubElValRepByName(std::string TagName) {
   gdcmElValue* elem =  PubElValSet.GetElementByName(TagName);
   if ( !elem )
      return GDCM_UNFOUND;
   return elem->GetVR();
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within elements parsed with the SHADOW dictionary 
 *          for the element value of a given tag.
 * @param   group Group of the researched tag.
 * @param   element Element of the researched tag.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetShaElValByNumber(guint16 group, guint16 element) {
   return ShaElValSet.GetElValueByNumber(group, element);
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within the elements parsed with the SHADOW dictionary
 *          for the element value representation of a given tag.
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
std::string gdcmHeader::GetShaElValRepByNumber(guint16 group, guint16 element) {
   gdcmElValue* elem =  ShaElValSet.GetElementByNumber(group, element);
   if ( !elem )
      return GDCM_UNFOUND;
   return elem->GetVR();
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within the elements parsed with the shadow dictionary
 *          for an element value of given tag.
 * @param   TagName name of the researched element.
 * @return  Corresponding element value when it exists, and the string
 *          GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetShaElValByName(std::string TagName) {
   return ShaElValSet.GetElValueByName(TagName);
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within the elements parsed with the shadow dictionary for
 *          the element value representation of a given tag.
 *
 *          Obtaining the VR (Value Representation) might be needed by caller
 *          to convert the string typed content to caller's native type 
 *          (think of C++ vs Python). The VR is actually of a higher level
 *          of semantics than just the native C++ type.
 * @param   TagName name of the researched element.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetShaElValRepByName(std::string TagName) {
   gdcmElValue* elem =  ShaElValSet.GetElementByName(TagName);
   if ( !elem )
      return GDCM_UNFOUND;
   return elem->GetVR();
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within elements parsed with the public dictionary 
 *          and then within the elements parsed with the shadow dictionary
 *          for the element value of a given tag.
 * @param   group Group of the researched tag.
 * @param   element Element of the researched tag.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetElValByNumber(guint16 group, guint16 element) {
   std::string pub = GetPubElValByNumber(group, element);
   if (pub.length())
      return pub;
   return GetShaElValByNumber(group, element);
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within elements parsed with the public dictionary 
 *          and then within the elements parsed with the shadow dictionary
 *          for the element value representation of a given tag.
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
std::string gdcmHeader::GetElValRepByNumber(guint16 group, guint16 element) {
   std::string pub = GetPubElValRepByNumber(group, element);
   if (pub.length())
      return pub;
   return GetShaElValRepByNumber(group, element);
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within elements parsed with the public dictionary 
 *          and then within the elements parsed with the shadow dictionary
 *          for the element value of a given tag.
 * @param   TagName name of the researched element.
 * @return  Corresponding element value when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetElValByName(std::string TagName) {
   std::string pub = GetPubElValByName(TagName);
   if (pub.length())
      return pub;
   return GetShaElValByName(TagName);
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within elements parsed with the public dictionary 
 *          and then within the elements parsed with the shadow dictionary
 *          for the element value representation of a given tag.
 *
 *          Obtaining the VR (Value Representation) might be needed by caller
 *          to convert the string typed content to caller's native type 
 *          (think of C++ vs Python). The VR is actually of a higher level
 *          of semantics than just the native C++ type.
 * @param   TagName name of the researched element.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmHeader::GetElValRepByName(std::string TagName) {
   std::string pub = GetPubElValRepByName(TagName);
   if (pub.length())
      return pub;
   return GetShaElValRepByName(TagName);
}

/**
 * \ingroup gdcmHeader
 * \brief   Accesses an existing gdcmElValue in the PubElValSet of this instance
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * @param   content new value to substitute with
 * @param   group   group of the ElVal to modify
 * @param   element element of the ElVal to modify
 */
int gdcmHeader::SetPubElValByNumber(std::string content, guint16 group,
                                    guint16 element)
                                    
//TODO  : homogeneiser les noms : SetPubElValByNumber   qui appelle PubElValSet.SetElValueByNumber 
//        pourquoi pas            SetPubElValueByNumber ??
{

   return (  PubElValSet.SetElValueByNumber (content, group, element) );
}

/**
 * \ingroup gdcmHeader
 * \brief   Accesses an existing gdcmElValue in the PubElValSet of this instance
 *          through tag name and modifies it's content with the given value.
 * @param   content new value to substitute with
 * @param   TagName name of the tag to be modified
 */
int gdcmHeader::SetPubElValByName(std::string content, std::string TagName) {
   return (  PubElValSet.SetElValueByName (content, TagName) );
}

/**
 * \ingroup gdcmHeader
 * \brief   Accesses an existing gdcmElValue in the PubElValSet of this instance
 *          through it's (group, element) and modifies it's length with
 *          the given value.
 * \warning Use with extreme caution.
 * @param   length new length to substitute with
 * @param   group   group of the ElVal to modify
 * @param   element element of the ElVal to modify
 * @return  1 on success, 0 otherwise.
 */

int gdcmHeader::SetPubElValLengthByNumber(guint32 length, guint16 group,
                                    guint16 element) {
	return (  PubElValSet.SetElValueLengthByNumber (length, group, element) );
}

/**
 * \ingroup gdcmHeader
 * \brief   Accesses an existing gdcmElValue in the ShaElValSet of this instance
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * @param   content new value to substitute with
 * @param   group   group of the ElVal to modify
 * @param   element element of the ElVal to modify
 * @return  1 on success, 0 otherwise.
 */
int gdcmHeader::SetShaElValByNumber(std::string content,
                                    guint16 group, guint16 element) {
   return (  ShaElValSet.SetElValueByNumber (content, group, element) );
}

/**
 * \ingroup gdcmHeader
 * \brief   Accesses an existing gdcmElValue in the ShaElValSet of this instance
 *          through tag name and modifies it's content with the given value.
 * @param   content new value to substitute with
 * @param   ShadowTagName name of the tag to be modified
 */
int gdcmHeader::SetShaElValByName(std::string content, std::string ShadowTagName) {
   return (  ShaElValSet.SetElValueByName (content, ShadowTagName) );
}

/**
 * \ingroup gdcmHeader
 * \brief   Parses the header of the file but WITHOUT loading element values.
 */
void gdcmHeader::ParseHeader(bool exception_on_error) throw(gdcmFormatError) {
   gdcmElValue * newElValue = (gdcmElValue *)0;
   
   rewind(fp);
   CheckSwap();
   while ( (newElValue = ReadNextElement()) ) {
      SkipElementValue(newElValue);
      PubElValSet.Add(newElValue);
   }
}

/**
 * \ingroup gdcmHeader
 * \brief  This predicate, based on hopefully reasonnable heuristics,
 *         decides whether or not the current gdcmHeader was properly parsed
 *         and contains the mandatory information for being considered as
 *         a well formed and usable image.
 * @return true when gdcmHeader is the one of a reasonable Dicom file,
 *         false otherwise. 
 */
bool gdcmHeader::IsReadable(void) {
   if (   GetElValByName("Image Dimensions") != GDCM_UNFOUND
      && atoi(GetElValByName("Image Dimensions").c_str()) > 4 ) {
      return false;
   }
   if ( GetElValByName("Bits Allocated")       == GDCM_UNFOUND )
      return false;
   if ( GetElValByName("Bits Stored")          == GDCM_UNFOUND )
      return false;
   if ( GetElValByName("High Bit")             == GDCM_UNFOUND )
      return false;
   if ( GetElValByName("Pixel Representation") == GDCM_UNFOUND )
      return false;
   return true;
}

/**
 * \ingroup gdcmHeader
 * \brief   Small utility function that creates a new manually crafted
 *          (as opposed as read from the file) gdcmElValue with user
 *          specified name and adds it to the public tag hash table.
 * \note    A fake TagKey is generated so the PubDict can keep it's coherence.
 * @param   NewTagName The name to be given to this new tag.
 * @param   VR The Value Representation to be given to this new tag.
 * @ return The newly hand crafted Element Value.
 */
gdcmElValue* gdcmHeader::NewManualElValToPubDict(std::string NewTagName, std::string VR) {
   gdcmElValue* NewElVal = (gdcmElValue*)0;
   guint32 StuffGroup = 0xffff;   // Group to be stuffed with additional info
   guint32 FreeElem = 0;
   gdcmDictEntry* NewEntry = (gdcmDictEntry*)0;

   FreeElem = PubElValSet.GenerateFreeTagKeyInGroup(StuffGroup);
   if (FreeElem == UINT32_MAX) {
      dbg.Verbose(1, "gdcmHeader::NewManualElValToPubDict",
                     "Group 0xffff in Public Dict is full");
      return (gdcmElValue*)0;
   }
   NewEntry = new gdcmDictEntry(StuffGroup, FreeElem,
                                VR, "GDCM", NewTagName);
   NewElVal = new gdcmElValue(NewEntry);
   PubElValSet.Add(NewElVal);
   return NewElVal;
}

/**
 * \ingroup gdcmHeader
 * \brief   Loads the element values of all the elements present in the
 *          public tag based hash table.
 */
void gdcmHeader::LoadElements(void) {
   rewind(fp);   
   TagElValueHT ht = PubElValSet.GetTagHt();
   for (TagElValueHT::iterator tag = ht.begin(); tag != ht.end(); ++tag) {
      LoadElementValue(tag->second);
   }
   rewind(fp);

   // Load 'non string' values   
   std::string PhotometricInterpretation = GetPubElValByNumber(0x0028,0x0004);   
   if( PhotometricInterpretation == "PALETTE COLOR " ){ 
      LoadElementVoidArea(0x0028,0x1200);  // gray LUT   
      LoadElementVoidArea(0x0028,0x1201);  // R    LUT
      LoadElementVoidArea(0x0028,0x1202);  // G    LUT
      LoadElementVoidArea(0x0028,0x1203);  // B    LUT
      
      LoadElementVoidArea(0x0028,0x1221);  // Segmented Red   Palette Color LUT Data
      LoadElementVoidArea(0x0028,0x1222);  // Segmented Green Palette Color LUT Data
      LoadElementVoidArea(0x0028,0x1223);  // Segmented Blue  Palette Color LUT Data
   }

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to read ACR-LibIDO formated images
   //
   // if recognition code tells us we deal with a LibIDO image
   // we switch lineNumber and columnNumber
   //
   std::string RecCode;	
   RecCode = GetPubElValByNumber(0x0008, 0x0010);
   if (RecCode == "ACRNEMA_LIBIDO_1.1" ||
       RecCode == "CANRME_AILIBOD1_1." ) {
         filetype = ACR_LIBIDO; 
         std::string rows    = GetPubElValByNumber(0x0028, 0x0010);
         std::string columns = GetPubElValByNumber(0x0028, 0x0011);
         SetPubElValByNumber(columns, 0x0028, 0x0010);
         SetPubElValByNumber(rows   , 0x0028, 0x0011);
   }
   // ----------------- End of Special Patch ----------------
}

/**
  * \ingroup gdcmHeader
  * \brief
  * @return
  */ 
void gdcmHeader::PrintPubElVal(std::ostream & os) {
   PubElValSet.Print(os);
}

/**
  * \ingroup gdcmHeader
  * \brief
  * @return
  */  
void gdcmHeader::PrintPubDict(std::ostream & os) {
   RefPubDict->Print(os);
}

/**
  * \ingroup gdcmHeader
  * \brief
  * @return
  */ 
int gdcmHeader::Write(FILE * fp, FileType type) {
   return PubElValSet.Write(fp, type);
}

//
// ------------------------ 'non string' elements related functions
//

/**
 * \ingroup       gdcmHeader
 * \brief         Loads (from disk) the element content 
 *                when a string is not suitable
 */
void * gdcmHeader::LoadElementVoidArea(guint16 Group, guint16 Elem) {
   gdcmElValue * Element= PubElValSet.GetElementByNumber(Group, Elem);
   if ( !Element )
      return NULL;
   size_t o =(size_t)Element->GetOffset();
   fseek(fp, o, SEEK_SET);
   int l=Element->GetLength();
   void * a = malloc(l);
   if(!a) {
   	std::cout << "Big Broblem (LoadElementVoidArea, malloc) " 
   	     << std::hex << Group << " " << Elem << std::endl;
   	return NULL;
   }  
   int res = PubElValSet.SetVoidAreaByNumber(a, Group, Elem);
   // TODO check the result 
   size_t l2 = fread(a, 1, l ,fp);
   if(l != l2) {
   	std::cout << "Big Broblem (LoadElementVoidArea, fread) " 
   	     << std::hex << Group << " " << Elem << std::endl;
   	free(a);
   	return NULL;
   }  
}

/**
 * \ingroup gdcmHeader
 * \brief   Gets (from Header) the offset  of a 'non string' element value 
 * \        (LoadElementValue has already be executed)
 * @param   Group
 * @param   Elem
 * @return File Offset of the Element Value 
 */
 size_t gdcmHeader::GetPubElValOffsetByNumber(guint16 Group, guint16 Elem) {
   gdcmElValue* elValue = PubElValSet.GetElementByNumber(Group, Elem);	 
   if (!elValue) {
      dbg.Verbose(1, "gdcmHeader::GetElValueByNumber",
                  "failed to Locate gdcmElValue");
      return (size_t)0;
   }
   return elValue->GetOffset();
}

/**
 * \ingroup gdcmHeader
* \brief   Gets (from Header) a 'non string' element value 
 * \        (LoadElementValue has already be executed)  
 * @param   Group
 * @param   Elem
 * @return Pointer to the 'non string' area
 
 */
 void * gdcmHeader::GetPubElValVoidAreaByNumber(guint16 Group, guint16 Elem) {
   gdcmElValue* elValue = PubElValSet.GetElementByNumber(Group, Elem);	 
   if (!elValue) {
      dbg.Verbose(1, "gdcmHeader::GetElValueByNumber",
                  "failed to Locate gdcmElValue");
      return (NULL);
   }
   return elValue->GetVoidArea();
}


//
// =============================================================================
//   Heuristics based accessors
//==============================================================================
//

// TODO : move to an other file.


/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of columns of image.
 * @return  The encountered size when found, 0 by default.
 */
int gdcmHeader::GetXSize(void) {
   // We cannot check for "Columns" because the "Columns" tag is present
   // both in IMG (0028,0011) and OLY (6000,0011) sections of the dictionary.
   std::string StrSize = GetPubElValByNumber(0x0028,0x0011);
   if (StrSize == GDCM_UNFOUND)
      return 0;
   return atoi(StrSize.c_str());
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of lines of image.
 * \warning The defaulted value is 1 as opposed to gdcmHeader::GetXSize()
 * @return  The encountered size when found, 1 by default.
 */
int gdcmHeader::GetYSize(void) {
   // We cannot check for "Rows" because the "Rows" tag is present
   // both in IMG (0028,0010) and OLY (6000,0010) sections of the dictionary.
   std::string StrSize = GetPubElValByNumber(0x0028,0x0010);
   if (StrSize != GDCM_UNFOUND)
      return atoi(StrSize.c_str());
   if ( IsDicomV3() )
      return 0;
   else
      // The Rows (0028,0010) entry is optional for ACR/NEMA. It might
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
 * @return  The encountered size when found, 1 by default.
 */
int gdcmHeader::GetZSize(void) {
   // Both in DicomV3 and ACR/Nema the consider the "Number of Frames"
   // as the third dimension.
   std::string StrSize = GetPubElValByNumber(0x0028,0x0008);
   if (StrSize != GDCM_UNFOUND)
      return atoi(StrSize.c_str());

   // We then consider the "Planes" entry as the third dimension [we
   // cannot retrieve by name since "Planes tag is present both in
   // IMG (0028,0012) and OLY (6000,0012) sections of the dictionary]. 
   StrSize = GetPubElValByNumber(0x0028,0x0012);
   if (StrSize != GDCM_UNFOUND)
      return atoi(StrSize.c_str());
   return 1;
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of Bits Stored
 *          (as opposite to number of Bits Allocated)
 * 
 * @return  The encountered number of Bits Stored, 0 by default.
 */
int gdcmHeader::GetBitsStored(void) { 
   std::string StrSize = GetPubElValByNumber(0x0028,0x0101);
   if (StrSize == GDCM_UNFOUND)
      return 1;
   return atoi(StrSize.c_str());
}


/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of Samples Per Pixel
 *          (1 : gray level, 3 : RGB)
 * 
 * @return  The encountered number of Samples Per Pixel, 1 by default.
 */
int gdcmHeader::GetSamplesPerPixel(void) { 
   std::string StrSize = GetPubElValByNumber(0x0028,0x0002);
   if (StrSize == GDCM_UNFOUND)
      return 1; // Well, it's supposed to be mandatory ...
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
   std::string StrSize = GetPubElValByNumber(0x0028,0x0006);
   if (StrSize == GDCM_UNFOUND)
      return 0;
   return atoi(StrSize.c_str());
}

/**
 * \ingroup gdcmHeader
 * \brief   Return the size (in bytes) of a single pixel of data.
 * @return  The size in bytes of a single pixel of data.
 *
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
 * @return  
 */
std::string gdcmHeader::GetPixelType(void) {
   std::string BitsAlloc;
   BitsAlloc = GetElValByName("Bits Allocated");
   if (BitsAlloc == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetPixelType: unfound Bits Allocated");
      BitsAlloc = std::string("16");
   }
   if (BitsAlloc == "12")
      BitsAlloc = std::string("16");

   std::string Signed;
   Signed = GetElValByName("Pixel Representation");
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
  * \brief gets the info from 0002,0010 : Transfert Syntax
  * \           else 1.
  * @return Transfert Syntax Name (as oposite to Transfert Syntax UID)
  */
std::string gdcmHeader::GetTransferSyntaxName(void) { 
   std::string TransfertSyntax = GetPubElValByNumber(0x0002,0x0010);
   if (TransfertSyntax == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetTransferSyntaxName: unfound Transfert Syntax (0002,0010)");
      return "Uncompressed ACR-NEMA";
   }
   // we do it only when we need it
   gdcmTS * ts = gdcmGlobal::GetTS();
   std::string tsName=ts->GetValue(TransfertSyntax);
   //delete ts; // Seg Fault when deleted ?!
   return tsName;
}

// -------------------------------- Lookup Table related functions ------------

/**
  * \ingroup gdcmHeader
  * \brief gets the info from 0028,1101 : Lookup Table Desc-Red
  * \           else 0
  * @return Lookup Table Length 
  * \       when (0028,0004),Photometric Interpretation = [PALETTE COLOR ] 
  */
  
int gdcmHeader::GetLUTLength(void) {
   std::vector<std::string> tokens;
   int LutLength;
   //int LutDepth;
   //int LutNbits;
   // Just hope Lookup Table Desc-Red = Lookup Table Desc-Red = Lookup Table Desc-Blue
   std::string LutDescriptionR = GetPubElValByNumber(0x0028,0x1101);
   if (LutDescriptionR == GDCM_UNFOUND)
      return 0;
   std::string LutDescriptionG = GetPubElValByNumber(0x0028,0x1102);
   if (LutDescriptionG == GDCM_UNFOUND)
      return 0;
   std::string LutDescriptionB = GetPubElValByNumber(0x0028,0x1103);
   if (LutDescriptionB == GDCM_UNFOUND)
      return 0;
   if( (LutDescriptionR != LutDescriptionG) || (LutDescriptionR != LutDescriptionB) ) {
      dbg.Verbose(0, "gdcmHeader::GetLUTLength: The CLUT R,G,B are not equal");
      return 0;   
   } 
   std::cout << "Lut Description " << LutDescriptionR <<std::endl;
   tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
   Tokenize (LutDescriptionR, tokens, "\\");
   LutLength=atoi(tokens[0].c_str());
   //LutDepth=atoi(tokens[1].c_str());
   //LutNbits=atoi(tokens[2].c_str());
   tokens.clear();
   return LutLength;
}

/**
  * \ingroup gdcmHeader
  * \brief gets the info from 0028,1101 : Lookup Table Desc-Red
  * \           else 0
  * @return Lookup Table nBit 
  * \       when (0028,0004),Photometric Interpretation = [PALETTE COLOR ] 
  */
  
int gdcmHeader::GetLUTNbits(void) {
   std::vector<std::string> tokens;
   //int LutLength;
   //int LutDepth;
   int LutNbits;
   // Just hope Lookup Table Desc-Red = Lookup Table Desc-Red = Lookup Table Desc-Blue
   // Consistency already checked in GetLUTLength
   std::string LutDescription = GetPubElValByNumber(0x0028,0x1101);
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
  * \brief gets the info from 0028,1201 : Lookup Table Red
  * \           else 0
  * @return Lookup Table Red 
  * \       when (0028,0004),Photometric Interpretation = [PALETTE COLOR ] 
  */ 
void * gdcmHeader::GetLUTRed(void) {
   return GetPubElValVoidAreaByNumber(0x0028,0x1201);  
}

/**
  * \ingroup gdcmHeader
  * \brief gets the info from 0028,1202 : Lookup Table Green
  * \           else 0
  * @return Lookup Table Red 
  * \       when (0028,0004),Photometric Interpretation = [PALETTE COLOR ] 
  */ 
  void * gdcmHeader::GetLUTGreen(void) {
   return GetPubElValVoidAreaByNumber(0x0028,0x1202);
}

/**
  * \ingroup gdcmHeader
  * \brief gets the info from 0028,1202 : Lookup Table Blue
  * \           else 0
  * @return Lookup Table Blue 
  * \       when (0028,0004),Photometric Interpretation = [PALETTE COLOR ] 
  */ 
void * gdcmHeader::GetLUTBlue(void) {
   return GetPubElValVoidAreaByNumber(0x0028,0x1203);
}

/**
  * \ingroup gdcmHeader
  * \brief 
  * @return Lookup Table RGB
  * \       when (0028,0004),Photometric Interpretation = [PALETTE COLOR ]
  * \        and (0028,1201),(0028,1202),(0028,1202) are found
  * \warning : hazardous ! Use better GetPubElValVoidAreaByNumber
  */ 
void * gdcmHeader::GetLUTRGB(void) {
// Not so easy : see 
// http://www.barre.nom.fr/medical/dicom2/limitations.html#Color%20Lookup%20Tables
// and  OT-PAL-8-face.dcm

   if (GetPubElValByNumber(0x0028,0x0004) == GDCM_UNFOUND) {
   dbg.Verbose(0, "gdcmHeader::GetLUTRGB: unfound Photometric Interpretation");
   	return NULL;
   }  
   void * LutR,*LutG,*LutB;
   int l;
     
  // Maybe, some day we get an image 
  // that respects the definition ...
  // Let's consider no ones does.
  
   l= GetLUTLength();  
   if(l==0) 
     return (NULL);     
   int nBits=GetLUTNbits();
  // a virer quand on aura trouve UNE image 
  // qui correspond VRAIMENT à la definition !
    std::cout << "l " << l << " nBits " << nBits;
   
   l= l/(nBits/8);
    
   LutR =GetPubElValVoidAreaByNumber(0x0028,0x1201);
   LutG =GetPubElValVoidAreaByNumber(0x0028,0x1202);
   LutB =GetPubElValVoidAreaByNumber(0x0028,0x1203);
   
   // Warning : Any value for nBits  as to be considered as 8
   //           Any value for Length as to be considered as 256
   // That's DICOM ...
   
   // Just wait before removing the following code
   /*
   if (nBits == 16) {
      guint16 * LUTRGB, *rgb;
      LUTRGB = rgb = (guint16 *) malloc(3*l*sizeof( guint16));
      guint16 * r = (guint16 *)LutR;
      guint16 * g = (guint16 *)LutG;
      guint16 * b = (guint16 *)LutB;
      for(int i=0;i<l;i++) {
         *rgb++ = *r++;
         *rgb++ = *g++;
         *rgb++ = *b++;
      }
      return(LUTRGB); 
   } else
   
   */ {      // we assume it's always 8 Bits
      l=256; // we assume ...
      unsigned char * LUTRGB, *rgb;
      LUTRGB = rgb = (unsigned char *) malloc(3*l*sizeof( char));
      unsigned char * r = (unsigned char *)LutR;
      unsigned char * g = (unsigned char *)LutG;
      unsigned char * b = (unsigned char *)LutB;
      for(int i=0;i<l;i++) {
      //std::cout << "lut16 " << i << " : " << *r << " " << *g << " " << *b << std::endl;
      printf("lut 8 %d : %d %d %d \n",i,*r,*g,*b);
         *rgb++ = *r++;
         *rgb++ = *g++;
         *rgb++ = *b++;
      } 
      free(LutR); free(LutB); free(LutG);
      return(LUTRGB);   
   } 
}
 
