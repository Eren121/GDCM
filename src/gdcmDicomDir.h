// gdcmDicomDir.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMDIR_H
#define GDCMDICOMDIR_H

//#include "gdcmHeader.h"
#include "gdcmCommon.h"
#include "gdcmDocument.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirMeta.h"
#include "gdcmDicomDirElement.h"

#include <list>
#include <vector>

//-----------------------------------------------------------------------------
typedef std::list<gdcmDicomDirPatient *>   ListDicomDirPatient;
typedef std::vector<gdcmDocument *>  VectDocument;

typedef GDCM_EXPORT void(gdcmMethod)(void * = NULL);
//-----------------------------------------------------------------------------

/**
 * \ingroup gdcmDicomDir
 * \brief    gdcmDicomDir defines an object representing a DICOMDIR in memory.
 *
 */
class GDCM_EXPORT gdcmDicomDir: public gdcmDocument
{
public:
//   gdcmDicomDir(ListTag *l,
//                bool exception_on_error = false);
   gdcmDicomDir(const char *FileName, 
                bool parseDir = false,
                bool exception_on_error = false);
   gdcmDicomDir(bool exception_on_error = false); 
                   
   ~gdcmDicomDir(void);

   /**
    * \ingroup gdcmDicomDir
    * \brief   Sets the print level for the Dicom Header 
    * \note    0 for Light Print; 1 for 'medium' Print, 2 for Heavy
    */
   void SetPrintLevel(int level) 
      { printLevel = level; };
  /**
    * \ingroup gdcmDicomDir
    * \brief   canonical Printer 
    * \sa    SetPrintLevel
  */     
   virtual void Print(std::ostream &os = std::cout);

// Informations contained in the parser
   virtual bool IsReadable(void);

/// \brief   returns a pointer to the gdcmDicomDirMeta for this DICOMDIR. 
   inline gdcmDicomDirMeta   *GetDicomDirMeta()      
      {return metaElems;};

 /// \brief   returns the PATIENT chained List for this DICOMDIR.    
   inline ListDicomDirPatient &GetDicomDirPatients() 
      {return patients;};

// Parsing
   void ParseDirectory(void);
   
   void SetStartMethod(gdcmMethod *,void * =NULL,gdcmMethod * =NULL);
   void SetStartMethodArgDelete(gdcmMethod *);
   void SetProgressMethod(gdcmMethod *,void * =NULL,gdcmMethod * =NULL);
   void SetProgressMethodArgDelete(gdcmMethod *);
   void SetEndMethod(gdcmMethod *,void * =NULL,gdcmMethod * =NULL);
   void SetEndMethodArgDelete(gdcmMethod *);

/// \brief GetProgress GetProgress
   inline float GetProgress(void)  
      {return(progress);};

/// \brief AbortProgress AbortProgress
   inline void  AbortProgress(void)
      {abort=true;      };

/// \brief IsAborted IsAborted
      inline bool  IsAborted(void)
      {return(abort);   };
   
// Adding
  gdcmDicomDirMeta *    NewMeta(void);
  gdcmDicomDirPatient * NewPatient(void);

// Write
   void WriteEntries(FILE *_fp);   
   bool Write(std::string fileName);

/// Types of the gdcmObject within the gdcmDicomDir
   typedef enum
   {
      GDCM_DICOMDIR_NONE,
      GDCM_DICOMDIR_META,
      GDCM_DICOMDIR_PATIENT,
      GDCM_DICOMDIR_STUDY,
      GDCM_DICOMDIR_SERIE,
      GDCM_DICOMDIR_IMAGE,
   } gdcmDicomDirType;
   
protected:
   void CreateDicomDirChainedList(std::string path);

   void CallStartMethod(void);
   void CallProgressMethod(void);
   void CallEndMethod(void);

private:
   void CreateDicomDir(void);
//   void AddObjectToEnd(gdcmDicomDirType type, gdcmSQItem *s);
   void AddDicomDirMeta   ();
   void AddDicomDirPatientToEnd(gdcmSQItem *s);
   void AddDicomDirStudyToEnd  (gdcmSQItem *s);
   void AddDicomDirSerieToEnd  (gdcmSQItem *s);
   void AddDicomDirImageToEnd  (gdcmSQItem *s);

   void SetElements(std::string &path, VectDocument &list);
   void SetElement (std::string &path,gdcmDicomDirType type,
                    gdcmDocument *header);
   
   void UpdateDirectoryRecordSequenceLength(void);

   static bool HeaderLessThan(gdcmDocument *header1,gdcmDocument *header2);
   
// Variables

/// \brief pointer on *the* gdcmObject 'DicomDirMeta Elements'
   gdcmDicomDirMeta *metaElems;

/// \brief chained list of DicomDirPatient (to be exploited recursively) 
   ListDicomDirPatient patients;

/// pointer to the initialisation method for any progress bar   
   gdcmMethod *startMethod;
/// pointer to the incrementation method for any progress bar
    gdcmMethod *progressMethod;
/// pointer to the tremination method for any progress bar
   gdcmMethod *endMethod;
/// pointer to the ??? method for any progress bar   
   gdcmMethod *startMethodArgDelete;
/// pointer to the ??? method for any progress bar
   gdcmMethod *progressMethodArgDelete;
/// pointer to the ??? method for any progress bar
   gdcmMethod *endMethodArgDelete;
/// pointer to the ??? for any progress bar   
   void *startArg;
/// pointer to the ??? for any progress bar
   void *progressArg;
/// pointer to the ??? for any progress bar   
   void *endArg;
/// value of the ??? for any progress bar
   float progress;
/// value of the ??? for any progress bar   
   bool abort;
};

//-----------------------------------------------------------------------------
#endif
