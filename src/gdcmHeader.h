// gdcmHeader.h

#ifndef GDCMHEADER_H
#define GDCMHEADER_H

#include <map>
#include "gdcmCommon.h"
#include "gdcmException.h"
#include "gdcmDictSet.h"
#include "gdcmElValue.h"
#include "gdcmElValSet.h"

////////////////////////////////////////////////////////////////////////////
// The purpous of an instance of gdcmHeader is to act as a container of
// all the elements and their corresponding values (and additionaly the
// corresponding DICOM dictionary entry) of the header of a DICOM file.
//
// The typical usage of instances of class gdcmHeader is to classify a set of
// dicom files according to header information e.g. to create a file hierarchy
// reflecting the Patient/Study/Serie informations, or extracting a given
// SerieId. Accesing the content (image[s] or volume[s]) is beyond the
// functionality of this class and belong to gdmcFile (see below).
// Notes:
// * the various entries of the explicit value representation (VR) shall
//   be managed within a dictionary which is shared by all gdcmHeader instances
// * the gdcmHeader::Set*Tag* family members cannot be defined as protected
//   (Swig limitations for as Has_a dependency between gdcmFile and gdcmHeader)

typedef string VRKey;
typedef string VRAtr;
typedef map<VRKey, VRAtr> VRHT;    // Value Representation Hash Table

class GDCM_EXPORT gdcmHeader {
	void SkipBytes(guint32);
private:
	static VRHT *dicom_vr;
	// Dictionaries of data elements:

	gdcmDictSet* Dicts;         // global dictionary container
	gdcmDict* RefPubDict;       // public dictionary
	gdcmDict* RefShaDict;       // shadow dictionary (optional)
	// Parsed element values:
	ElValSet PubElVals;         // parsed with Public Dictionary
	ElValSet ShaElVals;         // parsed with Shadow Dictionary
	string filename;            // refering underlying file
	FILE * fp;
	guint16 grPixel;
	guint16 numPixel;
	// Ne faudrait-il pas une indication sur la presence ou non
	// du 'groupe des pixels' dans l'entete?
	// (voir pb du DICOMDIR)
	
	// Swap code (little, big, bad-big, bad-little endian): this code is not fixed
	// during parsing.FIXME sw should be an enum e.g.
	//enum EndianType {
		//LittleEndian, 
		//BadLittleEndian,
		//BigEndian, 
		//BadBigEndian};
	int sw;

	// Only the elements whose size is below this bound will be loaded.
	// By default, this upper bound is limited to 1024 (which looks reasonable
	// when one considers the definition of the various VR contents).
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
	void AddAndDefaultElements(void);
	void SetMaxSizeLoadElementValue(long);

	gdcmDictEntry * GetDictEntryByKey(guint16, guint16);
	gdcmDictEntry * GetDictEntryByName(string name);

	// ElValue related utilities
	ElValue * ReadNextElement(void);
	ElValue * NewElValueByKey(guint16 group, guint16 element);
	ElValue * NewElValueByName(string name);
	void FindLength(ElValue *);
	void FindVR(ElValue *);
	void LoadElementValue(ElValue *);
	void LoadElementValueSafe(ElValue *);
	void SkipElementValue(ElValue *);
	void FixFoundLength(ElValue*, guint32);
	bool IsAnInteger(ElValue *);
	
	bool IsImplicitVRLittleEndianTransferSyntax(void);
	bool IsExplicitVRLittleEndianTransferSyntax(void);
	bool IsDeflatedExplicitVRLittleEndianTransferSyntax(void);
	bool IsExplicitVRBigEndianTransferSyntax(void);
	bool IsJPEGBaseLineProcess1TransferSyntax(void);
	bool IsJPEGExtendedProcess2_4TransferSyntax(void);	
	bool IsJPEGExtendedProcess3_5TransferSyntax(void);
	bool IsJPEGSpectralSelectionProcess6_8TransferSyntax(void);	
		
protected:
	enum FileType {
		Unknown = 0,
		TrueDicom,
		ExplicitVR,
		ImplicitVR,
		ACR,
		ACR_LIBIDO};  // CLEANME
	FileType filetype;
	int write(ostream&);   
	int anonymize(ostream&);  // FIXME : anonymize should be a friend ?
public:
	void LoadElements(void);
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

	// Get the element values themselves:
	string GetPubElValByName(string TagName);
	string GetPubElValByNumber(guint16 group, guint16 element);

	// Getting the element value representation (VR) might be needed by caller
	// to convert the string typed content to caller's native type 
	// (think of C/C++ vs Python).

	string GetPubElValRepByName(string TagName);
	string GetPubElValRepByNumber(guint16 group, guint16 element);

	TagElValueHT & GetPubElVal(void) { return PubElVals.GetTagHt(); };
	void   PrintPubElVal(ostream & os = cout);
	void   PrintPubDict(ostream & os = cout);
	  
	// Same thing with the shadow :
	// TODO Swig string* GetShaTagNames(); 
	string GetShaElValByName(string TagName);
	string GetShaElValByNumber(guint16 group, guint16 element);
	string GetShaElValRepByName(string TagName);
	string GetShaElValRepByNumber(guint16 group, guint16 element);

	// Wrappers of the above (public is privileged over shadow) to avoid
	// bugging the caller with knowing if ElVal is from the public or shadow
	// dictionary.
	string GetElValByName(string TagName);
	string GetElValByNumber(guint16 group, guint16 element);
	string GetElValRepByName(string TagName);
	string GetElValRepByNumber(guint16 group, guint16 element);

	int SetPubElValByName(string content, string TagName);
	int SetPubElValByNumber(string content, guint16 group, guint16 element);
	int SetShaElValByName(string content, string ShadowTagName);
	int SetShaElValByNumber(string content, guint16 group, guint16 element);

	ElValSet GetPubElVals() { return(PubElVals); }
};

#endif
