// gdcmFile.h

#ifndef GDCMFILE_H
#define GDCMFILE_H

#include "gdcmCommon.h"
#include "gdcmHeader.h"

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
	gdcmFile(const char * filename);
	
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
	
	// Allocates ExpectedSize bytes of memory at this->Data and copies the
	// pointed data to it.
	
	// Question :
	// Pourquoi dupliquer les pixels, alors qu'on les a deja en mémoire,
	// et que Data (dans le gdcmHeader) est un pointeur ?
	
	int SetImageData     (void * Data, size_t ExpectedSize);
	void SetImageDataSize (size_t ExpectedSize);
	
	// Push to disk.
	// A NE PAS OUBLIER : que fait-on en cas de Transfert Syntax (dans l'entete)
	// incohérente avec l'ordre des octets en mémoire  
	// TODO Swig int Write();
	
	// Ecrit sur disque les pixels d'UNE image
	// Aucun test n'est fait sur l'"Endiannerie" du processeur.
	// Ca sera à l'utilisateur d'appeler son Reader correctement
		
	int WriteRawData 	(string nomFichier);
	int WriteDcm     	(string nomFichier);
	int WriteDcm     	(const char * nomFichier);
	int WriteDcmExplVR	(string nomFichier);
	int WriteAcr     	(string nomFichier);
};

#endif
