// gdcmlib Intro:  
// * gdcmlib is a library dedicated to reading and writing dicom files.
// * LGPL for the license
// * lightweigth as opposed to CTN or DCMTK wich come bundled which try
//   to implement the full DICOM standard (networking...). gdcmlib concentrates
//   on reading and 
// * Formats: this lib should be able to read ACR-NEMA v1 and v2, Dicom v3 (as
//   stated in part10). [cf dcmtk/dcmdata/docs/datadict.txt]
// * Targeted plateforms: Un*xes and Win32/VC++6.0
//
//
// TODO
// The declarations commented out and starting with "TODO Swig" needed
// to be temporarily removed for swig to proceed correctly (in fact
// problems appears at loading of _gdcm.[so/dll]). So, simply uncomment
// the declaration once you provided the definition of the method...

#include <string>
#include <iostream>
#include <stddef.h>   // For size_t
#include <stdio.h>    // FIXME For FILE on GCC only
#include <map>        // The requirement for the hash table (or map) that
                      // we shall use:
                      // 1/ First, next, last (iterators)
                      // 2/ should be sortable (i.e. sorted by TagKey). This
                      //    condition shall be droped since the Win32/VC++
                      //    implementation doesn't look a sorted one. Pffff....
                      // 3/ Make sure we can setup some default size value,
                      //    which should be around 4500 entries which is the
                      //    average dictionary size (said JPR)
#ifdef __GNUC__
#include <stdint.h>
#define guint16 uint16_t
#define guint32 uint32_t
#define g_malloc malloc
#define g_free   free
#endif
#ifdef _MSC_VER
#include <glib.h>
#endif

#ifdef _MSC_VER
	using namespace std;  // string type lives in the std namespace on VC++
#endif
#ifdef _MSC_VER
#define GDCM_EXPORT __declspec( dllexport )
#else
#define GDCM_EXPORT
#endif

// Tag based hash tables.
// We shall use as keys the strings (as the C++ type) obtained by
// concatenating the group value and the element value (both of type
// unsigned 16 bit integers in Dicom) expressed in hexadecimal.
// Example: consider the tag given as (group, element) = (0x0010, 0x0010).
// Then the corresponding TagKey shall be the string 0010|0010 (where
// the | (pipe symbol) acts as a separator). Refer to 
// gdcmDictEntry::TranslateToKey for this conversion function.
typedef string TagKey;

class GDCM_EXPORT gdcmDictEntry {
private:
	guint16 group;    // e.g. 0x0010
	guint16 element;  // e.g. 0x0010
	string  vr;       // Value Representation i.e. some clue about the nature
	                  // of the data represented e.g. "FD" short for
	                  // "Floating Point Double"
	// CLEAN ME: find the official dicom name for this field !
	string  fourth;   // Fourth field containing some semantics.
	string  name;     // e.g. "Patient_Name"
	TagKey  key;      // This is redundant zith (group, element) but we add
	                  // on efficiency purposes.
	// DCMTK has many fields for handling a DictEntry (see below). What are the
	// relevant ones for gdcmlib ?
	//      struct DBI_SimpleEntry {
	//         Uint16 group;
	//         Uint16 element;
	//         Uint16 upperGroup;
	//         Uint16 upperElement;
	//         DcmEVR evr;
	//         const char* tagName;
	//         int vmMin;
	//         int vmMax;
	//         const char* standardVersion;
	//         DcmDictRangeRestriction groupRestriction;
	//         DcmDictRangeRestriction elementRestriction;
	//       };
public:
	//CLEANME gdcmDictEntry();
	gdcmDictEntry(guint16 group, guint16 element,
	              string vr     = "Unknown",
					  string fourth = "Unknown",
					  string name   = "Unknown");
	static TagKey TranslateToKey(guint16 group, guint16 element);
	guint16 GetGroup(void)  { return group;};
	guint16 GetElement(void){return element;};
	string  GetVR(void)     {return vr; };
	void    SetVR(string);
	bool    IsVrUnknown(void);
	string  GetFourth(void) {return fourth;};
	string  GetName(void)   {return name;};
	string  GetKey(void)    {return key;};
};
  
typedef map<TagKey, gdcmDictEntry*> TagHT;

// A single DICOM dictionary i.e. a container for a collection of dictionary
// entries. There should be a single public dictionary (THE dictionary of
// the actual DICOM v3) but as many shadow dictionaries as imagers 
// combined with all software versions...
class GDCM_EXPORT gdcmDict {
	string name;
	string filename;
	TagHT entries;
public:
	gdcmDict(const char* FileName);   // Read Dict from disk
	// TODO Swig int AppendEntry(gdcmDictEntry* NewEntry);
	gdcmDictEntry * GetTag(guint32 group, guint32 element);
	void Print(ostream&);
};

// Container for managing a set of loaded dictionaries. Sharing dictionaries
// should avoid :
// * reloading an allready loaded dictionary.
// * having many in memory representations of the same dictionary.
typedef string DictKey;
typedef map<DictKey, gdcmDict*> DictSetHT;
class GDCM_EXPORT gdcmDictSet {
private:
	string DictPath;      // Directory path to dictionaries
	DictSetHT dicts;
	int AppendDict(gdcmDict* NewDict);
	int LoadDictFromFile(string filename, DictKey);
	void SetDictPath(void);
public:
	gdcmDictSet(void);    // loads THE DICOM v3 dictionary
	// TODO Swig int LoadDictFromFile(string filename);
///// QUESTION: the following function might not be thread safe !? Maybe
/////           we need some mutex here, to avoid concurent creation of
/////           the same dictionary !?!?!
	// TODO Swig int LoadDictFromName(string filename);
	// TODO Swig int LoadAllDictFromDirectory(string DirectoryName);
	// TODO Swig string* GetAllDictNames();
	int LoadDicomV3Dict(void);
	void Print(ostream&);
	gdcmDict* GetDict(DictKey DictName);
	gdcmDict* GetDefaultPublicDict(void);
};

// The dicom header of a Dicom file contains a set of such ELement VALUES
// (when successfuly parsed against a given Dicom dictionary)
class GDCM_EXPORT ElValue {
private:
	gdcmDictEntry *entry;
	guint32 LgrElem;
	bool ImplicitVr;       // Even when reading explicit vr files, some
	                       // elements happen to be implicit. Flag them here
	                       // since we can't use the entry->vr without breaking
	                       // the underlying dictionary.
	// Might prove of some interest (see _ID_DCM_ELEM)
	// int Swap;
public:
	string  value;     // used to be char * valeurElem
	size_t Offset;     // Offset from the begining of file for direct user access
	ElValue(gdcmDictEntry*);
	void   SetVR(string);
	string GetVR(void);
	bool   IsVrUnknown(void) { return entry->IsVrUnknown(); };
	void SetLength(guint32 l){LgrElem = l; };
	void SetValue(string val){ value = val; };
	void SetOffset(size_t of){ Offset = of; };
	void SetImplicitVr(void) { ImplicitVr = true; };
	bool  IsImplicitVr(void) { return ImplicitVr; };
	string  GetValue(void)   { return value; };
	guint32 GetLength(void)  { return LgrElem; };
	size_t  GetOffset(void)  { return Offset; };
	guint16 GetGroup(void)   { return entry->GetGroup(); };
	guint16 GetElement(void) { return entry->GetElement(); };
	string  GetKey(void)     { return entry->GetKey(); };
	string  GetName(void)    { return entry->GetName();};
};

typedef map<TagKey, ElValue*> TagElValueHT;
typedef map<string, ElValue*> TagElValueNameHT;
// Container for a set of succefully parsed ElValues.
class GDCM_EXPORT ElValSet {
	// We need both accesses with a TagKey and the Dicentry.Name
	TagElValueHT tagHt;
	TagElValueNameHT NameHt;
public:
	void Add(ElValue*);
	void Print(ostream &);
	void PrintByName(ostream &);
	ElValue* GetElement(guint32 group, guint32 element);
	string GetElValue(guint32 group, guint32 element);
	string GetElValue(string);
	TagElValueHT & GetTagHt(void);
};

// The various entries of the explicit value representation (VR) shall
// be managed within a dictionary. 
typedef string VRKey;
typedef string VRAtr;
typedef map<TagKey, VRAtr> VRHT;    // Value Representation Hash Table

// The typical usage of objects of this class is to classify a set of
// dicom files according to header information e.g. to create a file hierachy
// reflecting the Patient/Study/Serie informations, or extracting a given
// SerieId. Accesing the content (image[s] or volume[s]) is beyond the
// functionality of this class (see dmcFile below).
// Notes:
// * the gdcmHeader::Set*Tag* family members cannot be defined as protected
//   (Swig limitations for as Has_a dependency between gdcmFile and gdcmHeader)
class GDCM_EXPORT gdcmHeader {	
//FIXME sw should be qn EndianType !!!
	//enum EndianType {
		//LittleEndian, 
		//BadLittleEndian,
		//BigEndian, 
		//BadBigEndian};
private:
  	// All instances share the same value representation dictionary
	static VRHT *dicom_vr;
	static gdcmDictSet* Dicts;  // Global dictionary container
	gdcmDict* RefPubDict;       // Public Dictionary
	gdcmDict* RefShaDict;       // Shadow Dictionary (optional)
	ElValSet PubElVals;     // Element Values parsed with Public Dictionary
	ElValSet ShaElVals;     // Element Values parsed with Shadow Dictionary
	// In order to inspect/navigate through the file
	string filename;
	FILE * fp;
	// The tag Image Location ((0028,0200) containing the adress of
	// the pixels) is not allways present. When we store this information
	// FIXME
	// outside of the elements:
	guint16 grPixel;
	guint16 numPixel;
	int sw;

	guint16 ReadInt16(void);
	guint32 ReadInt32(void);
	guint16 SwapShort(guint16);
	guint32 SwapLong(guint32);
	void Initialise(void);
	void CheckSwap(void);
	void FindLength(ElValue *);
	void FindVR(ElValue *);
	void LoadElementValue(ElValue *);
	void SkipElementValue(ElValue *);
	void InitVRDict(void);
	bool IsAnInteger(guint16, guint16, string, guint32);
	ElValue * ReadNextElement(void);
	gdcmDictEntry * IsInDicts(guint32, guint32);
	size_t GetPixelOffset(void);
protected:
	enum FileType {
		Unknown = 0,
		TrueDicom,
		ExplicitVR,
		ImplicitVR,
		ACR,
		ACR_LIBIDO};
	FileType filetype;
///// QUESTION: Maybe Print is a better name than write !?
	int write(ostream&);   
///// QUESTION: Maybe anonymize should be a friend function !?!?
/////           See below for an example of how anonymize might be implemented.
	int anonymize(ostream&);
public:
	void LoadElements(void);
	virtual void ParseHeader(void);
	gdcmHeader(const char* filename);
	virtual ~gdcmHeader();

	// TODO Swig int SetPubDict(string filename);
	// When some proprietary shadow groups are disclosed, whe can set
	// up an additional specific dictionary to access extra information.
	// TODO Swig int SetShaDict(string filename);

	// Retrieve all potentially available tag [tag = (group, element)] names
	// from the standard (or public) dictionary (hence static). Typical usage:
	// enable the user of a GUI based interface to select his favorite fields
	// for sorting or selection.
	// TODO Swig string* GetPubTagNames();
	// Get the element values themselves:
	string GetPubElValByName(string TagName);
	string GetPubElValByNumber(guint16 group, guint16 element);
	// Get the element value representation: (VR) might be needed by caller
	// to convert the string typed content to caller's native type (think
	// of C/C++ vs Python).
	// TODO Swig string GetPubElValRepByName(string TagName);
	// TODO Swig string GetPubElValRepByNumber(guint16 group, guint16 element);
	TagElValueHT & GetPubElVal(void) { return PubElVals.GetTagHt(); };
	void   PrintPubElVal(ostream & os = cout);
	void   PrintPubDict(ostream &);
	  
	// Same thing with the shadow :
	// TODO Swig string* GetShaTagNames(); 
	// TODO Swig string GetShaElValByName(string TagName);
	// TODO Swig string GetShaElValByNumber(guint16 group, guint16 element);
	// TODO Swig string GetShaElValRepByName(string TagName);
	// TODO Swig string GetShaElValRepByNumber(guint16 group, guint16 element);

	// Wrappers of the above (both public and shadow) to avoid bugging the
	// caller with knowing if ElVal is from the public or shadow dictionary.
	// TODO Swig string GetElValByName(string TagName);
	// TODO Swig string GetElValByNumber(guint16 group, guint16 element);
	// TODO Swig string GetElValRepByName(string TagName);
	// TODO Swig string GetElValRepByNumber(guint16 group, guint16 element);

	// TODO Swig int SetPubElValByName(string content, string TagName);
	// TODO Swig int SetPubElValByNumber(string content, guint16 group, guint16 element);
	// TODO Swig int SetShaElValByName(string content, string ShadowTagName);
	// TODO Swig int SetShaElValByNumber(string content, guint16 group, guint16 element);

	// TODO Swig int GetSwapCode();
};

// In addition to Dicom header exploration, this class is designed
// for accessing the image/volume content. One can also use it to
// write Dicom files.
////// QUESTION: this looks still like an open question wether the
//////           relationship between a gdcmFile and gdcmHeader is of
//////           type IS_A or HAS_A !
class GDCM_EXPORT gdcmFile: gdcmHeader
{
private:
	void* Data;
	int Parsed;				// weather allready parsed
	string OrigFileName;	// To avoid file overwrite
public:
	// Constructor dedicated to writing a new DICOMV3 part10 compliant
	// file (see SetFileName, SetDcmTag and Write)
	// TODO Swig gdcmFile();
	// Opens (in read only and when possible) an existing file and checks
	// for DICOM compliance. Returns NULL on failure.
	// Note: the in-memory representation of all available tags found in
	//    the DICOM header is post-poned to first header information access.
	//    This avoid a double parsing of public part of the header when
	//    one sets an a posteriori shadow dictionary (efficiency can be
	//    seen a a side effect).
	gdcmFile(string & filename);
	// For promotion (performs a deepcopy of pointed header object)
	// TODO Swig gdcmFile(gdcmHeader* header);
	// TODO Swig ~gdcmFile();

	// On writing purposes. When instance was created through
	// gdcmFile(string filename) then the filename argument MUST be different
	// from the constructor's one (no overwriting aloud).
	// TODO Swig int SetFileName(string filename);

	// Allocates necessary memory, copies the data (image[s]/volume[s]) to
	// newly allocated zone and return a pointer to it:
	// TODO Swig void * GetImageData();
	// Returns size (in bytes) of required memory to contain data
	// represented in this file.
	// TODO Swig size_t GetImageDataSize();
	// Copies (at most MaxSize bytes) of data to caller's memory space.
	// Returns an error code on failure (if MaxSize is not big enough)
	// TODO Swig int PutImageDataHere(void* destination, size_t MaxSize );
	// Allocates ExpectedSize bytes of memory at this->Data and copies the
	// pointed data to it.
	// TODO Swig int SetImageData(void * Data, size_t ExpectedSize);
	// Push to disk.
	// TODO Swig int Write();
};

//class gdcmSerie : gdcmFile;
//class gdcmMultiFrame : gdcmFile;

//
//Examples:
// * gdcmFile WriteDicom;
//   WriteDicom.SetFileName("MyDicomFile.dcm");
//	string * AllTags = gdcmHeader.GetDcmTagNames();
//   WriteDicom.SetDcmTag(AllTags[5], "253");
//   WriteDicom.SetDcmTag("Patient Name", "bozo");
//   WriteDicom.SetDcmTag("Patient Name", "bozo");
//	WriteDicom.SetImageData(Image);
//   WriteDicom.Write();
//
//
//   Anonymize(ostream& output) {
//   a = gdcmFile("toto1");
//   a.SetPubValueByName("Patient Name", "");
//   a.SetPubValueByName("Date", "");
//   a.SetPubValueByName("Study Date", "");
//   a.write(output);
//   }
