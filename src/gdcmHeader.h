// gdcmHeader.h

#ifndef GDCMHEADER_H
#define GDCMHEADER_H

#include <map>
#include "gdcmCommon.h"
#include "gdcmException.h"
#include "gdcmDictSet.h"
#include "gdcmElValue.h"
#include "gdcmElValSet.h"

typedef string VRKey;
typedef string VRAtr;
typedef map<VRKey, VRAtr> VRHT;    // Value Representation Hash Table

/// The purpose of an instance of gdcmHeader is to act as a container of
/// all the DICOM elements and their corresponding values (and
/// additionaly the corresponding DICOM dictionary entry) of the header
/// of a DICOM file.
///
/// The typical usage of instances of class gdcmHeader is to classify a set of
/// dicom files according to header information e.g. to create a file hierarchy
/// reflecting the Patient/Study/Serie informations, or extracting a given
/// SerieId. Accesing the content (image[s] or volume[s]) is beyond the
/// functionality of this class and belongs to gdmcFile.
/// \note  The various entries of the explicit value representation (VR) shall
///        be managed within a dictionary which is shared by all gdcmHeader
///        instances.
/// \note  The gdcmHeader::Set*Tag* family members cannot be defined as
///        protected due to Swig limitations for as Has_a dependency between
///        gdcmFile and gdcmHeader.

class GDCM_EXPORT gdcmHeader {
   void SkipBytes(guint32);
private:
   /// Pointer to the Value Representation Hash Table which contains all
   /// the VR of the DICOM version3 public dictionary. 
   static VRHT *dicom_vr;
 
   /// Global dictionary container
   gdcmDictSet* Dicts;
   /// Public dictionary used to parse this header
   gdcmDict* RefPubDict;
   /// Optional "shadow dictionary" (private elements) used to parse this
   /// header
   gdcmDict* RefShaDict;

   /// ELement VALueS parsed with the PUBlic dictionary.
   gdcmElValSet PubElValSet;
   /// ELement VALueS parsed with the SHAdow dictionary.
   gdcmElValSet ShaElValSet;
   /// Refering underlying filename.
   string filename; 
   FILE * fp;
   
   // FIXME sw should be an enum e.g.
   //enum EndianType {
      //LittleEndian, 
      //BadLittleEndian,
      //BigEndian, 
      //BadBigEndian};
   /// Swap code e.g. little, big, bad-big, bad-little endian). Warning:
   /// this code is not fixed during header parsing.
   int sw;

   /// Size treshold above which an element value will NOT be loaded in 
   /// memory (to avoid loading the image/volume itself). By default,
   /// this upper bound is fixed to 1024 bytes (which might look reasonable
   /// when one considers the definition of the various VR contents).
   guint32 MaxSizeLoadElementValue;

   guint16 ReadInt16(void);
   guint32 ReadInt32(void);
   guint16 SwapShort(guint16);
   guint32 SwapLong(guint32);
   guint32 FindLengthOB(void);
   void Initialise(void);
   void CheckSwap(void);
   void InitVRDict(void);
   void SwitchSwapToBigEndian(void);
   // CLEAN ME: NewManualElValToPubDict is NOT called any more.
   gdcmElValue*  NewManualElValToPubDict(string NewTagName, string VR);
   void SetMaxSizeLoadElementValue(long);

   gdcmDictEntry * GetDictEntryByKey(guint16, guint16);
   gdcmDictEntry * GetDictEntryByName(string name);

   // ElValue related utilities
   gdcmElValue * ReadNextElement(void);
   gdcmElValue * NewElValueByKey(guint16 group, guint16 element);
   gdcmElValue * NewElValueByName(string name);
   void FindLength(gdcmElValue *);
   void FindVR(gdcmElValue *);
   void LoadElementValue(gdcmElValue *);
   void LoadElementValueSafe(gdcmElValue *);
   void SkipElementValue(gdcmElValue *);
   void FixFoundLength(gdcmElValue*, guint32);
   bool IsAnInteger(gdcmElValue *);
   void LoadElements(void);
   
   bool IsImplicitVRLittleEndianTransferSyntax(void);
   bool IsExplicitVRLittleEndianTransferSyntax(void);
   bool IsDeflatedExplicitVRLittleEndianTransferSyntax(void);
   bool IsExplicitVRBigEndianTransferSyntax(void);
   bool IsJPEGBaseLineProcess1TransferSyntax(void);
   bool IsJPEGExtendedProcess2_4TransferSyntax(void); 
   bool IsJPEGExtendedProcess3_5TransferSyntax(void);
   bool IsJPEGSpectralSelectionProcess6_8TransferSyntax(void); 
   
   bool IsJPEGLossless(void); 
   bool IsDicomV3(void); 
      
protected:
   FileType filetype;
   int write(ostream&);   
   int anonymize(ostream&);  // FIXME : anonymize should be a friend ?
public:
   bool IsReadable(void);
   virtual void ParseHeader(bool exception_on_error = false)
     throw(gdcmFormatError);
   gdcmHeader(const char *filename, bool exception_on_error = false)
     throw(gdcmFileError);
   virtual ~gdcmHeader();
   
   size_t GetPixelOffset(void);
   void   GetPixels(size_t, void *);
   int    GetSwapCode(void) { return sw; }

   // TODO Swig int SetPubDict(string filename);
   // When some proprietary shadow groups are disclosed, we can set up
   // an additional specific dictionary to access extra information.
   // TODO Swig int SetShaDict(string filename);

   string GetPubElValByName(string TagName);
   string GetPubElValByNumber(guint16 group, guint16 element);
   string GetPubElValRepByName(string TagName);
   string GetPubElValRepByNumber(guint16 group, guint16 element);

   TagElValueHT & GetPubElVal(void) { return PubElValSet.GetTagHt(); };
   void   PrintPubElVal(ostream & os = cout);
   void   PrintPubDict (ostream & os = cout);
     
   // TODO Swig string* GetShaTagNames(); 
   string GetShaElValByName(string TagName);
   string GetShaElValByNumber(guint16 group, guint16 element);
   string GetShaElValRepByName(string TagName);
   string GetShaElValRepByNumber(guint16 group, guint16 element);

   string GetElValByName(string TagName);
   string GetElValByNumber(guint16 group, guint16 element);
   string GetElValRepByName(string TagName);
   string GetElValRepByNumber(guint16 group, guint16 element);

   int SetPubElValByName(string content, string TagName);
   int SetPubElValByNumber(string content, guint16 group, guint16 element);
   int SetShaElValByName(string content, string ShadowTagName);
   int SetShaElValByNumber(string content, guint16 group, guint16 element);
   
   int SetPubElValLengthByNumber(guint32 lgr, guint16 group, guint16 element);                                   
   int ReplaceOrCreateByNumber(guint16 Group, guint16 Elem, string Value);                                
   int GetXSize(void);  
   int GetYSize(void);
   int GetZSize(void);       
   string GetPixelType(void);  
   int Write(FILE *, FileType);
   
};

#endif
