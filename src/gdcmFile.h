// gdcmFile.h

#ifndef GDCMFILE_H
#define GDCMFILE_H

#include "gdcmCommon.h"
#include "gdcmHeader.h"

////////////////////////////////////////////////////////////////////////////
// In addition to Dicom header exploration, this class is designed
// for accessing the image/volume content. One can also use it to
// write Dicom files.

class GDCM_EXPORT gdcmFile: public gdcmHeader
{
private:
   void* PixelData;
   size_t lgrTotale;
   int Parsed;               // weather already parsed
   std::string OrigFileName; // To avoid file overwrite
   void SwapZone(void* im, int swap, int lgr, int nb);
   bool ReadPixelData(void * destination);
   
   int gdcm_read_JPEG_file (void * image_buffer);   
  
protected:
   int WriteBase(std::string FileName, FileType type);
public:
   gdcmFile(std::string & filename);
   gdcmFile(const char * filename);
	
	// For promotion (performs a deepcopy of pointed header object)
	// TODO Swig gdcmFile(gdcmHeader* header);
	// TODO Swig ~gdcmFile();

	// On writing purposes. When instance was created through
	// gdcmFile(std::string filename) then the filename argument MUST be
        // different from the constructor's one (no overwriting allowed).
	// TODO Swig int SetFileName(std::string filename);

   void   SetPixelDataSizeFromHeader(void);
   size_t GetImageDataSize();
   void * GetImageData();
   size_t GetImageDataIntoVector(void* destination, size_t MaxSize );
	
      // Allocates ExpectedSize bytes of memory at this->Data and copies the
      // pointed data to it. Copying the image might look useless but
      // the caller might destroy it's image (without knowing it: think
      // of a complicated interface where display is done with a library
      // e.g. VTK) before calling the Write
   int SetImageData     (void * Data, size_t ExpectedSize);
      // When the caller is aware we simply point to the data:
      // TODO int SetImageDataNoCopy (void * Data, size_t ExpectedSize);
   void SetImageDataSize (size_t ExpectedSize);
	
	// Push to disk.
	// A NE PAS OUBLIER : que fait-on en cas de Transfert Syntax (dans l'entete)
	// incohérente avec l'ordre des octets en mémoire ? 
	// TODO Swig int Write();
	
	// Ecrit sur disque les pixels d'UNE image
	// Aucun test n'est fait sur l'"Endiannerie" du processeur.
	// Ca sera à l'utilisateur d'appeler son Reader correctement
		
   int WriteRawData  (std::string nomFichier);
   int WriteDcmImplVR(std::string nomFichier);
   int WriteDcmImplVR(const char * nomFichier);
   int WriteDcmExplVR(std::string nomFichier);
   int WriteAcr      (std::string nomFichier);
};

#endif
