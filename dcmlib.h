// Open questions:
// * End user API are prefixed with GDL (Gnu Dicom Librrary) ???
// * should RefPubDict be a key in a hashtable (implementation and not API)
//   or a pointer to a dictionary ?


// DCMlib general notes:  
// * Formats:DCMlib should be able to read ACR-NEMA v1 and v2, Dicom v3 (as
//   stated in part10). [cf dcmtk/dcmdata/docs/datadict.txt]
// * Targeted plateforms: Un*xes and Win32/VC++6.0 (and hopefully Win32/Cygwin)

#include <string>
#include <stddef.h>    // For size_t
// Dummy declaration for the time being
typedef int guint16;    // We shall need glib.h !

// Notes on the implemenation of the dictionary entry in DCMTK : 
//   They are many fields in this implementation (see below). What are the
//   relevant ones for us ?
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
//
//       static const DBI_SimpleEntry simpleBuiltinDict[] = {
//           { 0x0000, 0x0000, 0x0000, 0x0000,
//             EVR_UL, "CommandGroupLength", 1, 1, "dicom98",
//             DcmDictRange_Unspecified, DcmDictRange_Unspecified },...}
class DictEntry {
private:
	guint16 group;		// e.g. 0x0010
	guint16 element;	// e.g. 0x0010
	string  name;		// e.g. "Patient_Name"
	string  ValRep;	// Value Representation i.e. some clue about the nature
							// of the data represented e.g. "FD" short for
							// "Floating Point Double"
public:
	DictEntry();
	DictEntry(guint16 group, guint16 element, string  name, string  VR);
};

// A single DICOM dictionary i.e. a container for a collection of dictionary
// entries. There should be a single public dictionary (THE dictionary of
// the actual DICOM v3) but as many shadow dictionaries as imagers 
// combined with all software versions...
class Dict {
	string name;
	string filename;
	DictEntry* entries;
public:
	Dict();
	Dict(string filename);	// Read Dict from disk
	int AppendEntry(DictEntry* NewEntry);
};

// Container for managing a set of loaded dictionaries. Sharing dictionaries
// should avoid :
// * reloading an allready loaded dictionary.
// * having many in memory representations of the same dictionary.
class DictSet {
private:
	Dict* dicts;
	int AppendDict(Dict* NewDict);
public:
	DictSet();		// Default constructor loads THE DICOM v3 dictionary
	int LoadDictFromFile(string filename);
	int LoadDictFromName(string filename);
	int LoadAllDictFromDirectory(string directorynanme);
	string* GetAllDictNames();
	Dict* GetDict(string DictName);
};

// The dicom header of a Dicom file contains a set of such ELement VALUES
// (when successfuly parsed against a given Dicom dictionary)
class ElValue {
	guint16 group;		// e.g. 0x0010
	guint16 element;	// e.g. 0x0010
	string  value;
};

// Container for a set of succefully parsed ElValues.
class ElValSet {
	ElValue* values;
};

// The typical usage of objects of this class is to classify a set of
// dicom files according to header information e.g. to create a file hierachy
// reflecting the Patient/Study/Serie informations, or extracting a given
// SerieId. Accesing the content (image[s] or volume[s]) is beyond the
// functionality of this class (see dmcFile below).
// Notes:
// * the dcmHeader::Set*Tag* family members cannot be defined as protected
//   (Swig limitations for as Has_a dependency between dcmFile and dcmHeader)
class dcmHeader {
private:
	static DictSet* Dicts;	// Global dictionary container
	Dict* RefPubDict;			// Public Dictionary
	Dict* RefShaDict;			// Shadow Dictionary (optional)
	int swapcode;
	ElValSet PubElVals;		// Element Values parsed with Public Dictionary
	ElValSet ShaElVals;		// Element Values parsed with Shadow Dictionary
public:
	dcmHeader();
	dcmHeader(string filename);
	~dcmHeader();

	int SetPubDict(string filename);
	// When some proprietary shadow groups are disclosed, whe can set
	// up an additional specific dictionary to access extra information.
	int SetShaDict(string filename);

	// Retrieve all potentially available tag [tag = (group, element)] names
	// from the standard (or public) dictionary (hence static). Typical usage:
	// enable the user of a GUI based interface to select his favorite fields
	// for sorting or selection.
	string* GetPubTagNames();
	// Get the element values themselves:
	string GetPubElValByName(string TagName);
	string GetPubElValByNumber(guint16 group, guint16 element);
	// Get the element value representation: (VR) might be needed by caller
	// to convert the string typed content to caller's native type (think
	// of C/C++ vs Python).
	string GetPubElValRepByName(string TagName);
	string GetPubElValRepByNumber(guint16 group, guint16 element);
	  
	// Same thing with the shadow :
	string* GetShaTagNames(); 
	string GetShaElValByName(string TagName);
	string GetShaElValByNumber(guint16 group, guint16 element);
	string GetShaElValRepByName(string TagName);
	string GetShaElValRepByNumber(guint16 group, guint16 element);

	// Wrappers of the above (both public and shadow) to avoid bugging the
	// caller with knowing if ElVal is from the public or shadow dictionary.
	string GetElValByName(string TagName);
	string GetElValByNumber(guint16 group, guint16 element);
	string GetElValRepByName(string TagName);
	string GetElValRepByNumber(guint16 group, guint16 element);

	int SetPubElValByName(string content, string TagName);
	int SetPubElValByNumber(string content, guint16 group, guint16 element);
	int SetShaElValByName(string content, string ShadowTagName);
	int SetShaElValByNumber(string content, guint16 group, guint16 element);

	int GetSwapCode();
};

// In addition to Dicom header exploration, this class is designed
// for accessing the image/volume content. One can also use it to
// write Dicom files.
class dcmFile
{
private:
	dcmHeader* Header;
	void* Data;
	int Parsed;				// weather allready parsed
	string OrigFileName;	// To avoid file overwrite
public:
	// Constructor dedicated to writing a new DICOMV3 part10 compliant
	// file (see SetFileName, SetDcmTag and Write)
	dcmFile();
	// Opens (in read only and when possible) an existing file and checks
	// for DICOM compliance. Returns NULL on failure.
	// Note: the in-memory representation of all available tags found in
	//    the DICOM header is post-poned to first header information access.
	//    This avoid a double parsing of public part of the header when
	//    one sets an a posteriori shadow dictionary (efficiency can be
	//    seen a a side effect).
	dcmFile(string filename);
	// For promotion (performs a deepcopy of pointed header object)
	dcmFile(dcmHeader* header);
	~dcmFile();

	// On writing purposes. When instance was created through
	// dcmFile(string filename) then the filename argument MUST be different
	// from the constructor's one (no overwriting aloud).
	int SetFileName(string filename);

	// Allocates necessary memory, copies the data (image[s]/volume[s]) to
	// newly allocated zone and return a pointer to it:
	void * GetImageData();
	// Returns size (in bytes) of required memory to contain data
	// represented in this file.
	size_t GetImageDataSize();
	// Copies (at most MaxSize bytes) of data to caller's memory space.
	// Returns an error code on failure (if MaxSize is not big enough)
	int GetImageDataHere(void* destination, size_t MaxSize );
	// Allocates ExpectedSize bytes of memory at this->Data and copies the
	// pointed data to it.
	int SetImageData(void * Data, size_t ExpectedSize);
	// Push to disk.
	int Write();

///// Repeat here all the dcmHeader public members !!!
};

//class dcmSerie : dcmFile;
//class dcmMultiFrame : dcmFile;

//
//Examples:
// * dcmFile WriteDicom;
//   WriteDicom.SetFileName("MyDicomFile.dcm");
//	string * AllTags = dcmHeader.GetDcmTagNames();
//   WriteDicom.SetDcmTag(AllTags[5], "253");
//   WriteDicom.SetDcmTag("Patient Name", "bozo");
//   WriteDicom.SetDcmTag("Patient Name", "bozo");
//	WriteDicom.SetImageData(Image);
//   WriteDicom.Write();
