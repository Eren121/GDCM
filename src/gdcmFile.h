// gdcmFile.h

#ifndef GDCMFILE_H
#define GDCMFILE_H

#include "gdcmCommon.h"
#include "gdcmHeader.h"

////////////////////////////////////////////////////////////////////////////
// In addition to Dicom header exploration, this class is designed
// for accessing the image/volume content. One can also use it to
// write Dicom files.

class GDCM_EXPORT gdcmFile
{
private:
   gdcmHeader *Header;   // Header to use to load the file
   bool SelfHeader;

   void* PixelData;
   size_t lgrTotaleRaw;  // Area length to receive the pixels
   size_t lgrTotale;     // Area length to receive the RGB pixels
                         // from Grey Plane + Palette Color

   int Parsed;               // weather already parsed
   std::string OrigFileName; // To avoid file overwrite
   void SwapZone(void* im, int swap, int lgr, int nb);
   
   bool ReadPixelData(void * destination);
   
   bool gdcm_read_JPEG_file     (FILE *fp,void * image_buffer); // For JPEG 8 Bits
   bool gdcm_read_JPEG_file12   (FILE *fp,void * image_buffer); // For JPEG 12 Bits
   bool gdcm_read_JPEG2000_file (FILE *fp,void * image_buffer); // For JPEG 2000 (TODO)

// For Run Length Encoding (TOCHECK)
   bool gdcm_read_RLE_file      (FILE *fp,void * image_buffer); 

     
protected:
   int WriteBase(std::string FileName, FileType type);

public:
   gdcmFile(gdcmHeader *header);
   gdcmFile(std::string & filename);
   gdcmFile(const char * filename);
   virtual ~gdcmFile(void);
	
   gdcmHeader *GetHeader(void);

	// For promotion (performs a deepcopy of pointed header object)
	// TODO Swig gdcmFile(gdcmHeader* header);
	// TODO Swig ~gdcmFile();

	// On writing purposes. When instance was created through
	// gdcmFile(std::string filename) then the filename argument MUST be
        // different from the constructor's one (no overwriting allowed).
	// TODO Swig int SetFileName(std::string filename);

   void   SetPixelDataSizeFromHeader(void);
   size_t GetImageDataSize();
   size_t GetImageDataSizeRaw();
   void * GetImageData();
   size_t GetImageDataIntoVector(void* destination, size_t MaxSize);
   void * GetImageDataRaw();
   size_t GetImageDataIntoVectorRaw(void* destination, size_t MaxSize);
	
      // Allocates ExpectedSize bytes of memory at this->Data and copies the
      // pointed data to it. Copying the image might look useless but
      // the caller might destroy it's image (without knowing it: think
      // of a complicated interface where display is done with a library
      // e.g. VTK) before calling the Write
   int SetImageData     (void * Data, size_t ExpectedSize);
      // When the caller is aware we simply point to the data:
      // TODO int SetImageDataNoCopy (void * Data, size_t ExpectedSize);
	
	// Push to disk.
	// A NE PAS OUBLIER : que fait-on en cas de Transfert Syntax (dans l'entete)
	// incohérente avec l'ordre des octets en mémoire ? 
	// TODO Swig int Write();
	
	// Ecrit sur disque les pixels d'UNE image
	// Aucun test n'est fait sur l'"Endiannerie" du processeur.
	// Ca sera à l'utilisateur d'appeler son Reader correctement
		
   int WriteRawData  (std::string fileName);
   int WriteDcmImplVR(std::string fileName);
   int WriteDcmImplVR(const char * fileName);
   int WriteDcmExplVR(std::string fileName);
   int WriteAcr      (std::string fileName);
   
   bool ParsePixelData(void);
};

#endif
