// gdcmHeader.h
//-----------------------------------------------------------------------------
#ifndef GDCMHEADER_H
#define GDCMHEADER_H

#include "gdcmCommon.h"
#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmException.h"
#include "gdcmDictSet.h"
#include "gdcmHeaderEntry.h"

#include <stdio.h>
#include <map>
#include <list>       // for linking together *all* the Dicom Elements

//-----------------------------------------------------------------------------
typedef std::string VRKey;
typedef std::string VRAtr;
typedef std::map<VRKey, VRAtr> VRHT;    // Value Representation Hash Table

typedef std::multimap<TagKey, gdcmHeaderEntry*> TagHeaderEntryHT;
typedef std::pair<TagKey, gdcmHeaderEntry*> PairHT;
typedef std::pair<TagHeaderEntryHT::iterator,TagHeaderEntryHT::iterator> IterHT; 

typedef std::list<gdcmHeaderEntry*> ListTag; // for linking together the Elements

// TODO : to be removed after re-writting   gdcmHeaderEntrySet::UpdateGroupLength
//          using the chained list instead of the H table
typedef std::string GroupKey;
typedef std::map<GroupKey, int> GroupHT;

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmHeader
 * \brief
 * The purpose of an instance of gdcmHeader is to act as a container of
 * all the DICOM elements and their corresponding values (and
 * additionaly the corresponding DICOM dictionary entry) of the header
 * of a DICOM file.
 *
 * The typical usage of instances of class gdcmHeader is to classify a set of
 * dicom files according to header information e.g. to create a file hierarchy
 * reflecting the Patient/Study/Serie informations, or extracting a given
 * SerieId. Accessing the content (image[s] or volume[s]) is beyond the
 * functionality of this class and belongs to gdmcFile.
 * \note  The various entries of the explicit value representation (VR) shall
 *        be managed within a dictionary which is shared by all gdcmHeader
 *        instances.
 * \note  The gdcmHeader::Set*Tag* family members cannot be defined as
 *        protected due to Swig limitations for as Has_a dependency between
 *        gdcmFile and gdcmHeader.
 */
class GDCM_EXPORT gdcmHeader {
public:
   gdcmHeader(bool exception_on_error = false);
   gdcmHeader(const char *filename, 
              bool  exception_on_error = false, 
              bool  enable_sequences   = false);
	      
   virtual ~gdcmHeader();

// Standard values and informations contained in the header
   inline std::string GetFileName(void) {return filename;}

   bool IsReadable(void);
   bool IsImplicitVRLittleEndianTransferSyntax(void);
   bool IsExplicitVRLittleEndianTransferSyntax(void);
   bool IsDeflatedExplicitVRLittleEndianTransferSyntax(void);
   bool IsExplicitVRBigEndianTransferSyntax(void);
   bool IsJPEGBaseLineProcess1TransferSyntax(void);
   bool IsJPEGExtendedProcess2_4TransferSyntax(void); 
   bool IsJPEGExtendedProcess3_5TransferSyntax(void);
   bool IsJPEGSpectralSelectionProcess6_8TransferSyntax(void); 
   bool IsRLELossLessTransferSyntax(void); 
   bool IsJPEGLossless(void); 
   bool IsJPEG2000(void); 
   bool IsDicomV3(void); 
   FileType GetFileType(void);

   // Some heuristic based accessors, end user intended 
   // (to be move to gdcmHeaderHelper?) 
   int GetXSize(void);  
   int GetYSize(void);
   int GetZSize(void);
   int GetBitsStored(void);
   int GetBitsAllocated(void);
   int GetSamplesPerPixel(void);   
   int GetPlanarConfiguration(void);

   int GetPixelSize(void);   
   std::string GetPixelType(void);  
   size_t GetPixelOffset(void);
   size_t GetPixelAreaLength(void);

   bool   HasLUT(void);
   int    GetLUTNbits(void);
   unsigned char * GetLUTRGBA(void);

   std::string GetTransfertSyntaxName(void);

   // When some proprietary shadow groups are disclosed, we can set up
   // an additional specific dictionary to access extra information.
   
   // OK : we still have *ONE* HeaderEntrySet, 
   // with both Public and Shadow Elements
   // parsed against THE Public Dictionary and A (single) Shadow Dictionary
   
   // TODO Swig int SetShaDict(std::string filename);
   // TODO Swig int SetPubDict(std::string filename);

// Public element value
   std::string GetPubEntryByName    (std::string tagName);
   std::string GetPubEntryVRByName  (std::string tagName);
   std::string GetPubEntryByNumber  (guint16 group, guint16 element);
   std::string GetPubEntryVRByNumber(guint16 group, guint16 element);
   
   bool SetPubEntryByName  (std::string content, std::string tagName); 
   bool SetPubEntryByNumber(std::string content, guint16 group, guint16 element);  
   bool SetPubEntryLengthByNumber(guint32 lgr, guint16 group, guint16 element);   
   
   /**
    * \ingroup gdcmHeader
    * \brief   returns a ref to the Dicom Header H table (multimap)
    * return the Dicom Header H table
    */
   inline TagHeaderEntryHT & GetPubEntry(void) { return tagHT; };
   
   /**
    * \ingroup gdcmHeader
    * \brief   returns a ref to the Dicom Header chained list
    * return the Dicom Header chained list
    */
   inline ListTag      & GetPubListEntry(void) { return listEntries; };
   
   /**
    * \ingroup gdcmHeader
    * \brief   Sets the print level for the Dicom Header 
    * \note 0 for Light Print; 1 for 'heavy' Print
    */
   void  SetPrintLevel(int level) { printLevel = level; };
   
   void PrintPubEntry(std::ostream & os = std::cout);
   void PrintPubDict (std::ostream & os = std::cout);

// Element value
   std::string GetEntryByName    (std::string tagName);
   std::string GetEntryVRByName  (std::string tagName);
   std::string GetEntryByNumber  (guint16 group, guint16 element);
   std::string GetEntryVRByNumber(guint16 group, guint16 element);

   bool SetEntryByName(std::string content,std::string tagName); 
//   bool SetEntryByNumber(std::string content,guint16 group, guint16 element);


// Read (used in gdcmFile)
   FILE *OpenFile(bool exception_on_error = false) throw(gdcmFileError);
   bool CloseFile(void);
   virtual void ParseHeader(bool exception_on_error = false) throw(gdcmFormatError);

// Write (used in gdcmFile)
   bool Write(FILE *, FileType);
   void SetImageDataSize(size_t ExpectedSize);

   bool ReplaceOrCreateByNumber(std::string Value, guint16 Group, guint16 Elem); 
   bool ReplaceOrCreateByNumber(     char * Value, guint16 Group, guint16 Elem);                                
   bool ReplaceIfExistByNumber (     char * Value, guint16 Group, guint16 Elem);

// System access
   inline int GetSwapCode(void) { return sw; }
   guint16 SwapShort(guint16); // needed by gdcmFile
   guint32 SwapLong(guint32);  // for JPEG Files
 
 
 
// ================= Was in EntrySet =================
   void Print(std::ostream &);
   void Add(gdcmHeaderEntry*);				
   bool SetEntryByNumber(std::string content, guint16 group, guint16 element);
   bool SetEntryLengthByNumber(guint32 l, guint16 group, guint16 element);
   bool SetVoidAreaByNumber(void *a, guint16 Group, guint16 Elem );
   guint32 GenerateFreeTagKeyInGroup(guint16 group);  
// ===================================================

   
protected:
   int CheckIfExistByNumber(guint16 Group, guint16 Elem ); // int !

   gdcmHeaderEntry *GetHeaderEntryByName  (std::string Name);
   gdcmHeaderEntry *GetHeaderEntryByNumber(guint16 group, guint16 element); 

   int write(std::ostream&);   
   int anonymize(std::ostream&);  // FIXME : anonymize should be a friend ?

   size_t GetPubEntryOffsetByNumber  (guint16 Group, guint16 Elem);
   void * GetPubEntryVoidAreaByNumber(guint16 Group, guint16 Elem);   
   void * LoadEntryVoidArea          (guint16 Group, guint16 Element);


// ================= Was in EntrySet =================
   void UpdateGroupLength(bool SkipSequence = false, FileType type = ImplicitVR);
   void WriteEntries(FileType type, FILE *);
// ===================================================

// Variables
   FILE * fp;
   FileType filetype; // ACR, ACR_LIBIDO, ExplicitVR, ImplicitVR, Unknown
   
private:
   // Read
   void LoadHeaderEntries    (void);
   void LoadHeaderEntry      (gdcmHeaderEntry *);
   void LoadHeaderEntrySafe  (gdcmHeaderEntry *);
   void FindHeaderEntryLength(gdcmHeaderEntry *);
   void FindHeaderEntryVR    (gdcmHeaderEntry *);

   void SkipHeaderEntry          (gdcmHeaderEntry *);
   void FixHeaderEntryFoundLength(gdcmHeaderEntry *, guint32);
   bool IsHeaderEntryAnInteger   (gdcmHeaderEntry *);

   guint32 FindHeaderEntryLengthOB(void);

   guint16 ReadInt16(void);
   guint32 ReadInt32(void);
   void    SkipBytes(guint32);

   void Initialise(void);
   void CheckSwap(void);
   void SwitchSwapToBigEndian(void);
   void SetMaxSizeLoadElementValue(long);

   // Dict
   gdcmDictEntry *GetDictEntryByName  (std::string Name);
   gdcmDictEntry *GetDictEntryByNumber(guint16, guint16);

   // HeaderEntry related utilities
   gdcmHeaderEntry *ReadNextHeaderEntry   (void);
   gdcmHeaderEntry *NewHeaderEntryByNumber(guint16 group, guint16 element);
   gdcmHeaderEntry *NewHeaderEntryByName  (std::string Name);

   // Deprecated (Not used)
   gdcmHeaderEntry *NewManualHeaderEntryToPubDict(std::string NewTagName,
                                                  std::string VR);

// Variables
   // Pointer to the Value Representation Hash Table which contains all
   // the VR of the DICOM version3 public dictionary. 
   gdcmVR *dicom_vr;     // Not a class member for thread-safety reasons
   
   // Pointer to the Transfert Syntax Hash Table which contains all
   // the TS of the DICOM version3 public dictionary. 
   gdcmTS *dicom_ts;     // Not a class member for thread-safety reasons 
     
   // Pointer to global dictionary container
   gdcmDictSet *Dicts;   // Not a class member for thread-safety reasons
   
   // Public dictionary used to parse this header
   gdcmDict *RefPubDict;
   
   // Optional "shadow dictionary" (private elements) used to parse this
   // header
   gdcmDict *RefShaDict;

   
   // ================ Was in gdcmHeaderEntrySet =========   
   TagHeaderEntryHT tagHT; // H Table (multimap), to provide fast access
   ListTag listEntries;    // chained list, to keep the 'spacial' ordering 
   int wasUpdated;	   // true if a gdcmHeaderEntry was added post parsing 
   int printLevel;         // for PrintHeader     
   // ====================================================
       
   // Refering underlying filename.
   std::string filename; 
  
   int enableSequences;

   // FIXME sw should be an enum e.g.
   //enum EndianType {
      //LittleEndian, 
      //BadLittleEndian,
      //BigEndian, 
      //BadBigEndian};
   // Swap code e.g. little, big, bad-big, bad-little endian). Warning:
   // this code is not fixed during header parsing.
   int sw;

   // Size treshold above which an element value will NOT be loaded in 
   // memory (to avoid loading the image/volume itself). By default,
   // this upper bound is fixed to 1024 bytes (which might look reasonable
   // when one considers the definition of the various VR contents).
   guint32 MaxSizeLoadElementValue;

   static const unsigned int HEADER_LENGTH_TO_READ; 
   static const unsigned int MAX_SIZE_LOAD_ELEMENT_VALUE;
};

//-----------------------------------------------------------------------------
#endif
