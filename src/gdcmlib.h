// gdcmlib Intro:  
// * gdcmlib is a library dedicated to reading and writing dicom files.
// * LGPL for the license
// * lightweigth as opposed to CTN or DCMTK wich come bundled which try
//   to implement the full DICOM standard (networking...). gdcmlib concentrates
//   on reading and 
// * Formats: this lib should be able to read ACR-NEMA v1 and v2, Dicom v3 (as
//   stated in part10). [cf dcmtk/dcmdata/docs/datadict.txt]
// * Targeted plateforms: Un*xes and Win32/VC++6.0

#include <string>
#include <stddef.h>    // For size_t
#include <glib.h>
#include <stdio.h>

// The requirement for the hash table (or map) that we shall use:
// 1/ First, next, last (iterators)
// 2/ should be sortable (i.e. sorted by TagKey). This condition
//    shall be droped since the Win32/VC++ implementation doesn't look
//    a sorted one. Pffff....
// 3/ Make sure we can setup some default size value, which should be
//    around 4500 entries which is the average dictionary size (said JPR)
#include <map>

// Tag based hash tables.
// We shall use as keys the strings (as the C++ type) obtained by
// concatenating the group value and the element value (both of type
// unsigned 16 bit integers in Dicom) expressed in hexadecimal.
// Example: consider the tag given as (group, element) = (0x0010, 0x0010).
// Then the corresponding TagKey shall be the string 00100010 (or maybe
// 0x00100x0010, we need some checks here).
typedef string TagKey;
typedef map<TagKey, char*> TagHT;


class DictEntry {
private:
////// QUESTION: it is not sure that we need to store the group and
//////           and the element within a DictEntry. What is the point
//////           of storing the equivalent of a TagKey within the information
//////           accessed through that TagKey !?
	guint16 group;		// e.g. 0x0010
	guint16 element;	// e.g. 0x0010
	string  name;		// e.g. "Patient_Name"
	string  ValRep;	// Value Representation i.e. some clue about the nature
							// of the data represented e.g. "FD" short for
							// "Floating Point Double"
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
	TagHT entries;
public:
	Dict();
	Dict(string filename);	// Read Dict from disk
	int AppendEntry(DictEntry* NewEntry);
};

// Container for managing a set of loaded dictionaries. Sharing dictionaries
// should avoid :
// * reloading an allready loaded dictionary.
// * having many in memory representations of the same dictionary.
typedef int DictId;
class DictSet {
private:
	map<DictId, Dict*> dicts;
	int AppendDict(Dict* NewDict);
public:
	DictSet();		// Default constructor loads THE DICOM v3 dictionary
	int LoadDictFromFile(string filename);
///// QUESTION: the following function might not be thread safe !? Maybe
/////           we need some mutex here, to avoid concurent creation of
/////           the same dictionary !?!?!
	int LoadDictFromName(string filename);
	int LoadAllDictFromDirectory(string directorynanme);
	string* GetAllDictNames();
	Dict* GetDict(string DictName);
};

// The dicom header of a Dicom file contains a set of such ELement VALUES
// (when successfuly parsed against a given Dicom dictionary)
class ElValue {
	DictEntry entry;
	string  value;
};

// Container for a set of succefully parsed ElValues.
typedef map<TagKey, char*> TagHT;
class ElValSet {
	// We need both accesses with a TagKey and the Dicentry.Name
////// QUESTION: this leads to a double storage of a single ElValue
	map<TagKey, ElValue> tagHt;
	map<string, ElValue> NameHt;
public:
	int Add(ElValue);
};

// The typical usage of objects of this class is to classify a set of
// dicom files according to header information e.g. to create a file hierachy
// reflecting the Patient/Study/Serie informations, or extracting a given
// SerieId. Accesing the content (image[s] or volume[s]) is beyond the
// functionality of this class (see dmcFile below).
// Notes:
// * the gdcmHeader::Set*Tag* family members cannot be defined as protected
//   (Swig limitations for as Has_a dependency between gdcmFile and gdcmHeader)
typedef int _ID_DCM_ELEM;
class gdcmHeader {	
	//enum EndianType {
		//LittleEndian, 
		//BadLittleEndian,
		//BigEndian, 
		//BadBigEndian};
	enum FileType {
		Unknown = 0,
		TrueDicom,
		ExplicitVR,
		ImplicitVR,
		ACR,
		ACR_LIBIDO};
private:
	static DictSet* Dicts;  // Global dictionary container
	Dict* RefPubDict;       // Public Dictionary
	Dict* RefShaDict;       // Shadow Dictionary (optional)
	ElValSet PubElVals;     // Element Values parsed with Public Dictionary
	ElValSet ShaElVals;     // Element Values parsed with Shadow Dictionary
	FileType filetype;
	FILE * fp;
	long int offsetCourant;
	int sw;
	void CheckSwap(void);
	void setAcrLibido(void);
	long int RecupLgr(_ID_DCM_ELEM *pleCourant, int sw,
	                  int *skippedLength, int *longueurLue);
	guint32 SWAP_LONG(guint32);
protected:
///// QUESTION: Maybe Print is a better name than write !?
	int write(ostream&);   
///// QUESTION: Maybe anonymize should be a friend function !?!?
/////           See below for an example of how anonymize might be implemented.
	int anonymize(ostream&);
public:
	gdcmHeader();
	gdcmHeader(string filename);
	~gdcmHeader();

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
////// QUESTION: this looks still like an open question wether the
//////           relationship between a gdcmFile and gdcmHeader is of
//////           type IS_A or HAS_A !
class gdcmFile: gdcmHeader
{
private:
	void* Data;
	int Parsed;				// weather allready parsed
	string OrigFileName;	// To avoid file overwrite
public:
	// Constructor dedicated to writing a new DICOMV3 part10 compliant
	// file (see SetFileName, SetDcmTag and Write)
	gdcmFile();
	// Opens (in read only and when possible) an existing file and checks
	// for DICOM compliance. Returns NULL on failure.
	// Note: the in-memory representation of all available tags found in
	//    the DICOM header is post-poned to first header information access.
	//    This avoid a double parsing of public part of the header when
	//    one sets an a posteriori shadow dictionary (efficiency can be
	//    seen a a side effect).
	gdcmFile(string filename);
	// For promotion (performs a deepcopy of pointed header object)
	gdcmFile(gdcmHeader* header);
	~gdcmFile();

	// On writing purposes. When instance was created through
	// gdcmFile(string filename) then the filename argument MUST be different
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
	int PutImageDataHere(void* destination, size_t MaxSize );
	// Allocates ExpectedSize bytes of memory at this->Data and copies the
	// pointed data to it.
	int SetImageData(void * Data, size_t ExpectedSize);
	// Push to disk.
	int Write();
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
