// Terminology : 
// * in DCMlib     tag = (group, element, name, value representation) e.g.
//   (0x0010, 0x0010, "Patient_Name",    _ID_Patient_Name
//   group   = 16 bit integer
//   element = 16 bit integer
//   name    = char * ("Patient_Name")
//   value representation  (e.g. "AE" = "Application Entity",
//         "FD", "Floating Point Double")
// * in DCMTK  tag = (group, element) but they are many more fields in
//   the implementation (see below). What are the relevant ones ?
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

//
// Open questions:
// * in libido a dictionary entry goes :
//   {0x0018,0x1251,"SH","ACQ","Transmitting Coil"}. What does the fourth
//   entry refer to ? 
// * what type of Dicom files can this library read ? ACR/NEMA/Dicom v2
//   or v3 part10 ? dcmtk/dcmdata/docs/datadict.txt mentions :all
//   standard DICOM tags (including those found in supplements 1, 2, 3, 4, 5,
//   6, 7, 8, 9, 10), obsolete ACR/NEMA version 2 tags, obsolete SPI tags, and
//   the tags used by Papyrus version 3...
//   Note en DCMTK the dictionary contains a standard version entry, like
//   "dicom98", dicom99", "ACR/NEMA2"
// * the convertion from VR to native types depends on native types of
//   target language (Python, Tcl a priori don't share the same native
//   representations). Where should this go ?
// * the dcmHeader::Set*Tag* family members cannot be defined as protected
//   (Swig limitations for as Has_a dependency between dcmFile and dcmHeader)
//   
// Plateforms: Un*xes and Win32/VC++6.0 or Win32/Cygwin


#include <stddef.h>    // For size_t
// Dummy declaration for the time being
typedef int Dict;
typedef int gint16;    // We shall need glib.h !

// The typical usage of objects of this class is to classify a set of
// dicom files according to header information e.g. to create a file hierachy
// reflecting the Patient/Study/Serie informations, or extracting a given
// SerieId. Accesing the content (image[s] or volume[s]) is beyond the
// functionality of this class (see dmcFile below).
class dcmHeader {
private:
	Dict* PubDict;		// Public Dictionary
	Dict* ShaDict;    // Shadow Dictionary (optional)
	int swapcode;
public:
	dcmHeader();
	dcmHeader(char* filename);
	~dcmHeader();

	// Retrieve all potentially available tag [tag = (group, element)] names
	// from the standard (or public) dictionary (hence static). Typical usage:
	// enable the user of a GUI based interface to select his favorite fields
	// for sorting or selection.
	static char ** GetDcmTagNames();
	char*   GetDcmTag(char* TagName);
	// Value Representation (VR) might be needed by caller to convert the
	// string typed content to caller's native type (think of C/C++ vs
	// Python).
	char*   GetDcmTagValRep(char* TagName);

	// When some proprietary shadow groups are disclosed, whe can set
	// up an additional specific dictionary to access extra information.
	int SetShadowDict(char* filename);
	int SetShadowDict(char** dictionary);  //????????
	int AddShadowDict(char* filename);     //????????              
	int DelShadowDict();

	// Retrieve all potentially available shadowed tag names
	char** GetShadowTagNames(); 
	char*  GetShadowTag(char* TagName);

	int SetTag(char* content, gint16 group, gint16 element);
	int SetTagByName(char* content, char* TagName);
	int SetShadowTag(char* content, gint16 group, gint16 element);
	int SetShadowTagByName(char* content, char* ShadowTagName);

	// Enable caller's low-level manual access to shadowed info
	char*   GetDcmTagByNumber(gint16 group, gint16 element);
	// Does this make ANY sense ?
	char*   GetDcmTagByNumberValRep(char* TagName);
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
	int Parsed;            // weather allready parsed
	char* OrigalFileName;  // To avoid file overwrite
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
	dcmFile(char* filename);
	// For promotion (performs a deepcopy of pointed header object)
	dcmFile(dcmHeader* header);
	~dcmFile();

	// On writing purposes. When instance was created through
	// dcmFile(char* filename) then the filename argument MUST be different
	// from the constructor's one (no overwriting aloud).
	int SetFileName(char* filename);

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
