// gdcmHeader.cxx

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

// Refer to gdcmHeader::CheckSwap()
#define HEADER_LENGTH_TO_READ       256
// Refer to gdcmHeader::SetMaxSizeLoadElementValue()
#define _MaxSizeLoadElementValue_   1024

void gdcmHeader::Initialise(void) {
   dicom_vr = gdcmGlobal::GetVR();
   Dicts = gdcmGlobal::GetDicts();
   RefPubDict = Dicts->GetDefaultPubDict();
   RefShaDict = (gdcmDict*)0;
}

gdcmHeader::gdcmHeader(const char *InFilename, bool exception_on_error) {
  SetMaxSizeLoadElementValue(_MaxSizeLoadElementValue_);
  filename = InFilename;
  Initialise();
  OpenFile(exception_on_error);
  ParseHeader();
  LoadElements();
  CloseFile();
}

bool gdcmHeader::OpenFile(bool exception_on_error)
  throw(gdcmFileError) {
  fp=fopen(filename.c_str(),"rb");
  if(exception_on_error) {
    if(!fp)
      throw gdcmFileError("gdcmHeader::gdcmHeader(const char *, bool)");
  }
  else
    dbg.Error(!fp, "gdcmHeader::gdcmHeader cannot open file", filename.c_str());
  if ( fp )
     return true;
  return false;
}

bool gdcmHeader::CloseFile(void) {
  int closed = fclose(fp);
  fp = (FILE *)0;
  if (! closed)
     return false;
  return true;
}

gdcmHeader::~gdcmHeader (void) {
   dicom_vr = (gdcmVR*)0;
   Dicts    = (gdcmDictSet*)0;
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
   guint32  x=4;  // x : pour ntohs
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
 */
void gdcmHeader::FindVR( gdcmElValue *ElVal) {
   if (filetype != ExplicitVR)
      return;

   char VR[3];
   string vr;
   int lgrLue;
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
   vr = string(VR);
      
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
   dbg.Verbose(1, "gdcmHeader::FindVR:", "Falsely explicit vr file");
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
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a ImplicitVRLittleEndian one.
 *
 * @return  True when ImplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmHeader::IsImplicitVRLittleEndianTransferSyntax(void) {
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
   string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a ExplicitVRLittleEndian one.
 *
 * @return  True when ExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmHeader::IsExplicitVRLittleEndianTransferSyntax(void) {
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
   string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.1" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a DeflatedExplicitVRLittleEndian one.
 *
 * @return  True when DeflatedExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmHeader::IsDeflatedExplicitVRLittleEndianTransferSyntax(void) {
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
   string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.1.99" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a Explicit VR Big Endian one.
 *
 * @return  True when big endian found. False in all other cases.
 */
bool gdcmHeader::IsExplicitVRBigEndianTransferSyntax(void) {
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
   string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.2" )  //1.2.2 ??? A verifier !
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a JPEGBaseLineProcess1 one.
 *
 * @return  True when JPEGBaseLineProcess1found. False in all other cases.
 */
bool gdcmHeader::IsJPEGBaseLineProcess1TransferSyntax(void) {
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
   string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.4.50" )
      return true;
   return false;
}

// faire qq chose d'intelligent a la place de ça

bool gdcmHeader::IsJPEGLossless(void) {
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
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
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a JPEGExtendedProcess2-4 one.
 *
 * @return  True when JPEGExtendedProcess2-4 found. False in all other cases.
 */
bool gdcmHeader::IsJPEGExtendedProcess2_4TransferSyntax(void) {
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
   string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.4.51" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
 *          and if it corresponds to a JPEGExtendeProcess3-5 one.
 *
 * @return  True when JPEGExtendedProcess3-5 found. False in all other cases.
 */
bool gdcmHeader::IsJPEGExtendedProcess3_5TransferSyntax(void) {
   gdcmElValue* Element = PubElValSet.GetElementByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadElementValueSafe(Element);
   string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.4.52" )
      return true;
   return false;
}

/**
 * \ingroup gdcmHeader
 * \brief   Determines if the Transfer Syntax was allready encountered
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
   string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.4.53" )
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
      if (errno == 1)
         return 0;
      TotalLength += 4;  // We even have to decount the group and element 
      if ( g != 0xfffe ) {
         dbg.Verbose(1, "gdcmHeader::FindLengthOB: ",
                     "wrong group for an item sequence.");
         errno = 1;
         return 0;
      }
      if ( n == 0xe0dd )
         FoundSequenceDelimiter = true;
      else if ( n != 0xe000) {
         dbg.Verbose(1, "gdcmHeader::FindLengthOB: ",
                     "wrong element for an item sequence.");
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

void gdcmHeader::FindLength(gdcmElValue * ElVal) {
   guint16 element = ElVal->GetElement();
   string  vr      = ElVal->GetVR();
   guint16 length16;
   
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
      // add an additional check: we look in the allready parsed elements
      // for the presence of a "Transfer Syntax" whose value has to be "big
      // endian encoding". When this is the case, chances are we have got our
      // hands on a big endian encoded file: we switch the swap code to
      // big endian and proceed...
      if ( (element  == 0x000) && (length16 == 0x0400) ) {
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

void gdcmHeader::SkipBytes(guint32 NBytes) {
   //FIXME don't dump the returned value
   (void)fseek(fp, (long)NBytes, SEEK_CUR);
}

void gdcmHeader::SkipElementValue(gdcmElValue * ElVal) {
   SkipBytes(ElVal->GetLength());
}

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
   guint16 elem   = ElVal->GetElement();
   string  vr     = ElVal->GetVR();
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
      ostringstream s;
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
		ostringstream s;
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


guint16 gdcmHeader::ReadInt16(void) {
   guint16 g;
   size_t item_read;
   item_read = fread (&g, (size_t)2,(size_t)1, fp);
   errno = 0;
   if ( item_read != 1 ) {
      dbg.Verbose(1, "gdcmHeader::ReadInt16", " File read error");
      errno = 1;
      return 0;
   }
   g = SwapShort(g);
   return g;
}

guint32 gdcmHeader::ReadInt32(void) {
   guint32 g;
   size_t item_read;
   item_read = fread (&g, (size_t)4,(size_t)1, fp);
   errno = 0;
   if ( item_read != 1 ) {
      dbg.Verbose(1, "gdcmHeader::ReadInt32", " File read error");
      errno = 1;
      return 0;
   }
   g = SwapLong(g);
   return g;
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
 * @param   
 */
int gdcmHeader::ReplaceOrCreateByNumber(string Value, guint16 Group, guint16 Elem ) {

	gdcmElValue* nvElValue=NewElValueByNumber(Group, Elem);
	PubElValSet.Add(nvElValue);	
	PubElValSet.SetElValueByNumber(Value, Group, Elem);
	return(1);
}   


/**
 * \ingroup gdcmHeader
 * \brief   Build a new Element Value from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Name    Name of the underlying DictEntry
 */
gdcmElValue* gdcmHeader::NewElValueByName(string Name) {

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
   if (errno == 1)
      // We reached the EOF (or an error occured) and header parsing
      // has to be considered as finished.
      return (gdcmElValue *)0;
   
   NewElVal = NewElValueByNumber(g, n);
   FindVR(NewElVal);
   FindLength(NewElVal);
   if (errno == 1)
      // Call it quits
      return (gdcmElValue *)0;
   NewElVal->SetOffset(ftell(fp));
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
   string  vr      = ElVal->GetVR();
   guint32 length  = ElVal->GetLength();

   // When we have some semantics on the element we just read, and if we
   // a priori know we are dealing with an integer, then we shall be
   // able to swap it's element value properly.
   if ( element == 0 )  {  // This is the group length of the group
      if (length == 4)
         return true;
      else {
         printf("Erroneous Group Length element length %d\n",length);
                    
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
   string ImageLocation = GetPubElValByName("Image Location");
   if ( ImageLocation == "gdcm::Unfound" ) {
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
gdcmDictEntry * gdcmHeader::GetDictEntryByName(string Name) {
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
 *          "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetPubElValByNumber(guint16 group, guint16 element) {
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
 *          and the string "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetPubElValRepByNumber(guint16 group, guint16 element) {
   gdcmElValue* elem =  PubElValSet.GetElementByNumber(group, element);
   if ( !elem )
      return "gdcm::Unfound";
   return elem->GetVR();
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within the public dictionary for element value of
 *          a given tag.
 * @param   TagName name of the researched element.
 * @return  Corresponding element value when it exists, and the string
 *          "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetPubElValByName(string TagName) {
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
 *          and the string "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetPubElValRepByName(string TagName) {
   gdcmElValue* elem =  PubElValSet.GetElementByName(TagName);
   if ( !elem )
      return "gdcm::Unfound";
   return elem->GetVR();
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within elements parsed with the SHADOW dictionary 
 *          for the element value of a given tag.
 * @param   group Group of the researched tag.
 * @param   element Element of the researched tag.
 * @return  Corresponding element value representation when it exists,
 *          and the string "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetShaElValByNumber(guint16 group, guint16 element) {
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
 *          and the string "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetShaElValRepByNumber(guint16 group, guint16 element) {
   gdcmElValue* elem =  ShaElValSet.GetElementByNumber(group, element);
   if ( !elem )
      return "gdcm::Unfound";
   return elem->GetVR();
}

/**
 * \ingroup gdcmHeader
 * \brief   Searches within the elements parsed with the shadow dictionary
 *          for an element value of given tag.
 * @param   TagName name of the researched element.
 * @return  Corresponding element value when it exists, and the string
 *          "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetShaElValByName(string TagName) {
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
 *          and the string "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetShaElValRepByName(string TagName) {
   gdcmElValue* elem =  ShaElValSet.GetElementByName(TagName);
   if ( !elem )
      return "gdcm::Unfound";
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
 *          and the string "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetElValByNumber(guint16 group, guint16 element) {
   string pub = GetPubElValByNumber(group, element);
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
 *          and the string "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetElValRepByNumber(guint16 group, guint16 element) {
   string pub = GetPubElValRepByNumber(group, element);
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
 *          and the string "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetElValByName(string TagName) {
   string pub = GetPubElValByName(TagName);
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
 *          and the string "gdcm::Unfound" otherwise.
 */
string gdcmHeader::GetElValRepByName(string TagName) {
   string pub = GetPubElValRepByName(TagName);
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
int gdcmHeader::SetPubElValByNumber(string content, guint16 group,
                                    guint16 element)
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
int gdcmHeader::SetPubElValByName(string content, string TagName) {
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
int gdcmHeader::SetShaElValByNumber(string content,
                                    guint16 group, guint16 element) {
   return (  ShaElValSet.SetElValueByNumber (content, group, element) );
}

/**
 * \ingroup gdcmHeader
 * \brief   Accesses an existing gdcmElValue in the ShaElValSet of this instance
 *          through tag name and modifies it's content with the given value.
 * @param   content new value to substitute with
 * @param   TagName name of the tag to be modified
 */
int gdcmHeader::SetShaElValByName(string content, string TagName) {
   return (  ShaElValSet.SetElValueByName (content, TagName) );
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
 * \brief   Retrieve the number of columns of image.
 * @return  The encountered size when found, 0 by default.
 */
int gdcmHeader::GetXSize(void) {
   // We cannot check for "Columns" because the "Columns" tag is present
   // both in IMG (0028,0011) and OLY (6000,0011) sections of the dictionary.
   string StrSize = GetPubElValByNumber(0x0028,0x0011);
   if (StrSize == "gdcm::Unfound")
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
   string StrSize = GetPubElValByNumber(0x0028,0x0010);
   if (StrSize != "gdcm::Unfound")
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
   string StrSize = GetPubElValByNumber(0x0028,0x0008);
   if (StrSize != "gdcm::Unfound")
      return atoi(StrSize.c_str());

   // We then consider the "Planes" entry as the third dimension [we
   // cannot retrieve by name since "Planes tag is present both in
   // IMG (0028,0012) and OLY (6000,0012) sections of the dictionary]. 
   StrSize = GetPubElValByNumber(0x0028,0x0012);
   if (StrSize != "gdcm::Unfound")
      return atoi(StrSize.c_str());
   return 1;
}

/**
 * \ingroup gdcmHeader
 * \brief   Build the Pixel Type of the image.
 *          Possible values are:
 *          - U8  unsigned  8 bit,
 *          - S8    signed  8 bit,
 *          - U16 unsigned 16 bit,
 *          - S16   signed 16 bit,
 *          - U32 unsigned 32 bit,
 *          - S32   signed 32 bit,
 * \warning 12 bit images appear as 16 bit.
 * @return 
 */
string gdcmHeader::GetPixelType(void) {
   string BitsAlloc;
   BitsAlloc = GetElValByName("Bits Allocated");
   if (BitsAlloc == "gdcm::Unfound") {
      dbg.Verbose(0, "gdcmHeader::GetPixelType: unfound Bits Allocated");
      BitsAlloc = string("16");
   }
   if (BitsAlloc == "12")
      BitsAlloc = string("16");

   string Signed;
   Signed = GetElValByName("Pixel Representation");
   if (Signed == "gdcm::Unfound") {
      dbg.Verbose(0, "gdcmHeader::GetPixelType: unfound Pixel Representation");
      BitsAlloc = string("0");
   }
   if (Signed == "0")
      Signed = string("U");
   else
      Signed = string("S");

   return( BitsAlloc + Signed);
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
   if (   GetElValByName("Image Dimensions") != "gdcm::Unfound"
      && atoi(GetElValByName("Image Dimensions").c_str()) > 4 ) {
      return false;
   }
   if (  GetElValByName("Bits Allocated") == "gdcm::Unfound" )
      return false;
   if (  GetElValByName("Bits Stored") == "gdcm::Unfound" )
      return false;
   if (  GetElValByName("High Bit") == "gdcm::Unfound" )
      return false;
   if (  GetElValByName("Pixel Representation") == "gdcm::Unfound" )
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
gdcmElValue* gdcmHeader::NewManualElValToPubDict(string NewTagName, string VR) {
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
}

void gdcmHeader::PrintPubElVal(ostream & os) {
   PubElValSet.Print(os);
}

void gdcmHeader::PrintPubDict(ostream & os) {
   RefPubDict->Print(os);
}

int gdcmHeader::Write(FILE * fp, FileType type) {
   return PubElValSet.Write(fp, type);
}
