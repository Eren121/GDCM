// gdcmHeader.h
//-----------------------------------------------------------------------------
#ifndef GDCMHEADER_H
#define GDCMHEADER_H

#include "gdcmCommon.h"
#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmException.h"
#include "gdcmDictSet.h"
#include "gdcmElValue.h"
#include "gdcmElValSet.h"
#include <map>

//-----------------------------------------------------------------------------
typedef std::string VRKey;
typedef std::string VRAtr;
typedef std::map<VRKey, VRAtr> VRHT;    // Value Representation Hash Table

//-----------------------------------------------------------------------------
/*
 * The purpose of an instance of gdcmHeader is to act as a container of
 * all the DICOM elements and their corresponding values (and
 * additionaly the corresponding DICOM dictionary entry) of the header
 * of a DICOM file.
 *
 * The typical usage of instances of class gdcmHeader is to classify a set of
 * dicom files according to header information e.g. to create a file hierarchy
 * reflecting the Patient/Study/Serie informations, or extracting a given
 * SerieId. Accesing the content (image[s] or volume[s]) is beyond the
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
   
   // OK : we still have *ONE* ElValSet, 
   // with both Public and Shadow Elements
   // parsed against THE Public Dictionary and A (single) Shadow Dictionary
   
   // TODO Swig int SetShaDict(std::string filename);
   // TODO Swig int SetPubDict(std::string filename);

// Public element value
   std::string GetPubElValByName     (std::string tagName);
   std::string GetPubElValRepByName  (std::string tagName);
   std::string GetPubElValByNumber   (guint16 group, guint16 element);
   std::string GetPubElValRepByNumber(guint16 group, guint16 element);
   
   bool SetPubElValByName  (std::string content, std::string tagName); 
   bool SetPubElValByNumber(std::string content, guint16 group, guint16 element);  
   bool SetPubElValLengthByNumber(guint32 lgr, guint16 group, guint16 element); 

   inline ListTag      & GetPubListElem(void) { return PubElValSet.GetListElem();};
   inline TagElValueHT & GetPubElVal(void) { return PubElValSet.GetTagHt();   };

   void PrintPubElVal(std::ostream & os = std::cout);
   void PrintPubDict (std::ostream & os = std::cout);

// Element value
   std::string GetElValByName     (std::string tagName);
   std::string GetElValRepByName  (std::string tagName);
   std::string GetElValByNumber   (guint16 group, guint16 element);
   std::string GetElValRepByNumber(guint16 group, guint16 element);

   bool SetElValueByName(std::string content,std::string tagName); 
//   bool SetElValueByNumber(std::string content,guint16 group, guint16 element);

//   inline ListTag      & GetListElem(void) { return PubElValSet.GetListElem();};
//   inline TagElValueHT & GetElVal(void) { return PubElValSet.GetTagHt();   };

// Read (used in gdcmFile)
   FILE *OpenFile(bool exception_on_error = false) throw(gdcmFileError);
   bool CloseFile(void);
   virtual void ParseHeader(bool exception_on_error = false) throw(gdcmFormatError);

// Write (used in gdcmFile)
   bool Write(FILE *, FileType);
   void SetImageDataSize(size_t ExpectedSize);

// System access
   inline int GetSwapCode(void) { return sw; }
   guint16 SwapShort(guint16); // needed by gdcmFile
   guint32 SwapLong(guint32);  // for JPEG Files
   
   // was protected
   bool ReplaceOrCreateByNumber(std::string Value, guint16 Group, guint16 Elem); 
   bool ReplaceOrCreateByNumber(     char * Value, guint16 Group, guint16 Elem);                                
   bool ReplaceIfExistByNumber (     char * Value, guint16 Group, guint16 Elem);

protected:
   bool CheckIfExistByNumber(guint16 Group, guint16 Elem );
   gdcmElValue * GetElValueByNumber(guint16 group, guint16 element); 

   int write(std::ostream&);   
   int anonymize(std::ostream&);  // FIXME : anonymize should be a friend ?

   size_t GetPubElValOffsetByNumber  (guint16 Group, guint16 Elem);
   void * GetPubElValVoidAreaByNumber(guint16 Group, guint16 Elem);   
   void * LoadElementVoidArea        (guint16 Group, guint16 Element);


// Variables
   FILE * fp;
   FileType filetype; // ACR, ACR_LIBIDO, ExplicitVR, ImplicitVR, Unknown
   
private:
   // Read
   void LoadElements        (void);
   void LoadElementValue    (gdcmElValue *);
   void LoadElementValueSafe(gdcmElValue *);
   void FindLength          (gdcmElValue *);
   void FindVR              (gdcmElValue *);

   guint32 FindLengthOB(void);

   void SkipElementValue    (gdcmElValue *);
   void FixFoundLength      (gdcmElValue *, guint32);
   bool IsAnInteger         (gdcmElValue *);

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

   // ElValue related utilities
   gdcmElValue *ReadNextElement   (void);
   gdcmElValue *NewElValueByNumber(guint16 group, guint16 element);
   gdcmElValue *NewElValueByName  (std::string Name);
   gdcmElValue* GetElementByName  (std::string Name);

   // Deprecated
   gdcmElValue *NewManualElValToPubDict(std::string NewTagName,
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

   /// ELement VALueS parsed with the PUBlic dictionary.
   gdcmElValSet PubElValSet;
   
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
