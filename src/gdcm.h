// gdcm.h

// gdcmlib Intro:  
// * gdcmlib is a library dedicated to reading and writing dicom files.
// * LGPL for the license
// * lightweigth as opposed to CTN or DCMTK which come bundled which try
//   to implement the full DICOM standard (networking...). gdcmlib concentrates
//   on reading and writing
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

#ifndef GDCM_H
#define GDCM_H

#include <string>
using namespace std;

#include <iostream>
#include <stddef.h>   // For size_t
#include <stdio.h>    // FIXME For FILE on GCC only
#include <list>
#include <map>
#include "gdcmException.h"


		      // The requirement for the hash table (or map) that
                      // we shall use:
                      // 1/ First, next, last (iterators)
                      // 2/ should be sortable (i.e. sorted by TagKey). This
                      //    condition shall be droped since the Win32/VC++
                      //    implementation doesn't look a sorted one. Pffff....
                      // 3/ Make sure we can setup some default size value,
                      //    which should be around 4500 entries which is the
                      //    average dictionary size (said JPR)
                      //
                      // En fait, je disais que dans LE Directory Dicom (dans son etat 2001)
                      // il y a +/- 1600 entrees.
                      // Une valeur raisonable pour un  majorant du nombre d'entrees
                      // dans une entete DICOM d'une image semble semble etre 300
                      // Si on 'decortique' les elements SQ (ce qui ne semble pas etre fait pour le moment)
                      // on risque en fait de depasser ... un nombre non previsible dans le cas d'une entree SQ
                      // contenant lui même un tres grand nombre d'entrees ?!?)
                      // Quant au nombre d'entrees dans un DICOMDIR, c'est encore pire : il n'est limité
                      // que par la taille d'un CD-ROM (les DVD-ROM ne sont pas encore pris en compte)
                      // On peut s'attendre a 30 entrees par fichier dicom présent sur le CD-ROM
                      // REMARQUE : il faudra se pencher sur le pb de la creation du DICOMDIR lorsqu'on voudra 
                      // exporter des images lisibles par les consoles cliniques 
                      // et pas seulement importables dans e-film. 

#ifdef __GNUC__
#include <stdint.h>
#define guint16 uint16_t
#define guint32 uint32_t
#endif

#ifdef _MSC_VER 
typedef  unsigned short guint16;
typedef  unsigned int   guint32;
#endif

#ifdef _MSC_VER
#define GDCM_EXPORT __declspec( dllexport )
#else
#define GDCM_EXPORT
#endif

////////////////////////////////////////////////////////////////////////////
// Tag based hash tables.
// We shall use as keys the strings (as the C++ type) obtained by
// concatenating the group value and the element value (both of type
// unsigned 16 bit integers in Dicom) expressed in hexadecimal.
// Example: consider the tag given as (group, element) = (0x0010, 0x0010).
// Then the corresponding TagKey shall be the string 0010|0010 (where
// the | (pipe symbol) acts as a separator). Refer to 
// gdcmDictEntry::TranslateToKey for this conversion function.

typedef string TagKey;
typedef string TagName;

class GDCM_EXPORT gdcmDictEntry {
private:
	guint16 group;    // e.g. 0x0010
	guint16 element;  // e.g. 0x0103
	string  vr;       // Value Representation i.e. some clue about the nature
	                  // of the data represented e.g. "FD" short for
	                  // "Floating Point Double"
	// CLEANME: find the official dicom name for this field !
	string  fourth;   // Fourth field containing some semantics.
	string  name;     // e.g. "Patient_Name"
	TagKey  key;      // Redundant with (group, element) but we add it
	                  // on efficiency purposes.
	// DCMTK has many fields for handling a DictEntry (see below). What are the
	// relevant ones for gdcmlib ?
	//      struct DBI_SimpleEntry {
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
	gdcmDictEntry(guint16 group, 
	              guint16 element,
	              string vr     = "Unknown",
	              string fourth = "Unknown",
	              string name   = "Unknown");
	// fabrique une 'clé' par concaténation du numGroupe et du numElement
	static TagKey TranslateToKey(guint16 group, guint16 element);
	
	guint16 GetGroup(void)  { return group; };
	guint16 GetElement(void){return element;};
	string  GetVR(void)     {return vr;     };
	void    SetVR(string);
	void    SetKey(string k){ key = k;     }
	bool    IsVrUnknown(void);
	string  GetFourth(void) {return fourth;};
	string  GetName(void)   {return name;  };
	string  GetKey(void)    {return key;   };
};
  
////////////////////////////////////////////////////////////////////////////
// A single DICOM dictionary i.e. a container for a collection of dictionary
// entries. There should be a single public dictionary (THE dictionary of
// the actual DICOM v3) but as many shadow dictionaries as imagers 
// combined with all software versions...

typedef map<TagKey,  gdcmDictEntry*> TagKeyHT;
typedef map<TagName, gdcmDictEntry*> TagNameHT;

class GDCM_EXPORT gdcmDict {
	string name;
	string filename;
	TagKeyHT  KeyHt;                // Both accesses with a TagKey or with a
	TagNameHT NameHt;               // TagName are required.
public:
	gdcmDict(const char* FileName);   // Reads Dict from ascii file
	int AddNewEntry (gdcmDictEntry* NewEntry);
	int ReplaceEntry(gdcmDictEntry* NewEntry);
	int RemoveEntry (TagKey key);
	int RemoveEntry (guint16 group, guint16 element);
	gdcmDictEntry * GetTagByKey(guint16 group, guint16 element);
	gdcmDictEntry * GetTagByName(TagName name);
	void Print(ostream&);
	void PrintByKey(ostream&);
	void PrintByName(ostream&);
	TagKeyHT & GetEntries(void) { return KeyHt; }
};


////////////////////////////////////////////////////////////////////////////
// Container for managing a set of loaded dictionaries. Sharing dictionaries
// should avoid :
// * reloading an allready loaded dictionary,
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
   // QUESTION: the following function might not be thread safe !? Maybe
   //           we need some mutex here, to avoid concurent creation of
   //           the same dictionary !?!?!
	// TODO Swig int LoadDictFromName(string filename);
	// TODO Swig int LoadAllDictFromDirectory(string DirectoryName);
	// TODO Swig string* GetAllDictNames();
	//
	// Question : ne faudra-t-il pas mettre LE dictionnaire DICOM dans un Directory
	// et les eventuels 'dictionnaires prives' dans un autre?
	// (pour eviter a un utilisateur mal dégourdi de tout saccager ?)
	//
	int LoadDicomV3Dict(void);
	void Print(ostream&);
	gdcmDict* GetDict(DictKey DictName);
	gdcmDict* GetDefaultPublicDict(void);
};

///////////////////////////////////////////////////////////////////////////
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
public:
	string  value;
	size_t Offset;     // Offset from the begining of file for direct user access
	
	ElValue(gdcmDictEntry*);
	void SetDictEntry(gdcmDictEntry *NewEntry) { entry = NewEntry; };
	bool   IsVrUnknown(void) { return entry->IsVrUnknown(); };
	void SetImplicitVr(void) { ImplicitVr = true; };
	bool  IsImplicitVr(void) { return ImplicitVr; };
	
	guint16 GetGroup(void)   { return entry->GetGroup();  };
	guint16 GetElement(void) { return entry->GetElement();};
	string  GetKey(void)     { return entry->GetKey();    };
	string  GetName(void)    { return entry->GetName();   };
	string  GetVR(void)      { return entry->GetVR();     };
	void    SetVR(string v)  { entry->SetVR(v);           }; 
	void SetLength(guint32 l){ LgrElem = l;               };
	guint32 GetLength(void)  { return LgrElem;            };
	
	// Question : SetLength est public 
	// (sinon, on ne pourrait pas l'appeler dans ElValSet)
	// alors que *personne* ne devrait s'en servir !
	// c'est *forcément* la lgr de la string 'value', non?

	void SetValue(string val){ value = val; };
	string  GetValue(void)   { return value;};

	void SetOffset(size_t of){ Offset = of; };
	size_t  GetOffset(void)  { return Offset;};
	// Question : SetOffset est public ...
	// Quel utilisateur serait ammené à modifier l'Offset ?
};


////////////////////////////////////////////////////////////////////////////
// Container for a set of successfully parsed ElValues.
typedef map<TagKey, ElValue*> TagElValueHT;
typedef map<string, ElValue*> TagElValueNameHT;

class GDCM_EXPORT ElValSet {
	TagElValueHT tagHt;             // Both accesses with a TagKey or with a
	TagElValueNameHT NameHt;        // the DictEntry.Name are required.
public:	
	void Add(ElValue*);		
	void Print(ostream &);
	void PrintByName(ostream &);
	int  Write(FILE *fp);
	ElValue* GetElementByNumber(guint32 group, guint32 element);
	ElValue* GetElementByName  (string);
	string   GetElValueByNumber(guint32 group, guint32 element);
	string   GetElValueByName  (string);
	
	TagElValueHT & GetTagHt(void);	
	
	int SetElValueByNumber(string content, guint32 group, guint32 element);
	int SetElValueByName  (string content, string TagName);
	
	int SetElValueLengthByNumber(guint32 l, guint32 group, guint32 element);
	int SetElValueLengthByName  (guint32 l, string TagName);

};

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

	static gdcmDictSet* Dicts;  // global dictionary container
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

	// Retrieve all potentially available tag [tag = (group, element)] names
	// from the standard (or public) dictionary. Typical usage : enable the
	// user of a GUI based interface to select his favorite fields for sorting
	// or selecting.
	list<string> * GetPubTagNames(void);
	map<string, list<string> > * GetPubTagNamesByCategory(void);
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
	void   PrintPubDict(ostream &);
	  
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

//
// ---------------------------------------------------- gdcmFile
//
//	un fichier EST_UNE entete, ou A_UNE entete ?
// 
//	On dit 'EST_UNE' ...


////////////////////////////////////////////////////////////////////////////
// In addition to Dicom header exploration, this class is designed
// for accessing the image/volume content. One can also use it to
// write Dicom files.
////// QUESTION: this looks still like an open question whether the
//////           relationship between a gdcmFile and gdcmHeader is of
//////           type IS_A or HAS_A !

class GDCM_EXPORT gdcmFile: public gdcmHeader
{
private:
	// QUESTION :
	// Data pointe sur quoi?
	// sur les Pixels lus?
	// --> j'ajoute un champ public : Pixels
	// (il faudra que l'utilisateur puisse modifier les pixels ?)
	
	void* Data;
	int Parsed;          // weather allready parsed
	string OrigFileName; // To avoid file overwrite
public:
	// je ne suis pas sur d'avoir compris *où* il serait légitime de ranger ca.
	// on pourra tjs le deplacer, et mettre des accesseurs
	void * Pixels;
	size_t lgrTotale;
	
	// Constructor dedicated to writing a new DICOMV3 part10 compliant
	// file (see SetFileName, SetDcmTag and Write)
	// TODO Swig gdcmFile();
	// Opens (in read only and when possible) an existing file and checks
	// for DICOM compliance. Returns NULL on failure.
	// Note: the in-memory representation of all available tags found in
	//    the DICOM header is post-poned to first header information access.
	//    This avoid a double parsing of public part of the header when
	//    one sets an a posteriori shadow dictionary (efficiency can be
	//    seen as a side effect).
	
	gdcmFile(string & filename);
	
	// For promotion (performs a deepcopy of pointed header object)
	// TODO Swig gdcmFile(gdcmHeader* header);
	// TODO Swig ~gdcmFile();

	// On writing purposes. When instance was created through
	// gdcmFile(string filename) then the filename argument MUST be different
	// from the constructor's one (no overwriting allowed).
	// TODO Swig int SetFileName(string filename);

	// Allocates necessary memory, copies the data (image[s]/volume[s]) to
	// newly allocated zone and return a pointer to it:
	
	 void * GetImageData();
	
	// Returns size (in bytes) of required memory to contain data
	// represented in this file.
	
	size_t GetImageDataSize();
	
	// Copies (at most MaxSize bytes) of data to caller's memory space.
	// Returns an error code on failure (if MaxSize is not big enough)
	
	int GetImageDataIntoVector(void* destination, size_t MaxSize );
	
	// Question :
	//
	//	GetImageData et GetImageDataIntoVector
	// Get et Put pour 2 fonctions qui font presque la meme chose :-(
	//
	
	// Allocates ExpectedSize bytes of memory at this->Data and copies the
	// pointed data to it.
	
	// Question :
	// Pourquoi dupliquer les pixels, alors qu'on les a deja en mémoire,
	// et que Data (dans le gdcmHeader) est un pointeur ?
	
	// TODO Swig int SetImageData(void * Data, size_t ExpectedSize);
	
	// Push to disk.
	// A NE PAS OUBLIER : que fait-on en cas de Transfert Syntax (dans l'entete)
	// incohérente avec l'ordre des octets en mémoire  
	// TODO Swig int Write();
	
	// Ecrit sur disque les pixels d'UNE image
	// Aucun test n'est fait sur l'"Endiannerie" du processeur.
	// Ca sera à l'utilisateur d'appeler son Reader correctement
		
	int WriteRawData (string nomFichier);
	int WriteDcm     (string nomFichier);
};

//
// ---------------------------------------------------- gdcmSerie
//
//	une serie EST_UN fichier ????
//
//

//class gdcmSerie : gdcmFile;

//
// ---------------------------------------------------- gdcmMultiFrame
//
//	un fichierMultiFrame EST_UN fichier 
//
//

//class gdcmMultiFrame : gdcmFile;


#endif // #ifndef GDCM_H
