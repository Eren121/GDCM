// gdcmDicomDir.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMDIR_H
#define GDCMDICOMDIR_H

#include "gdcmHeader.h"
#include "gdcmCommon.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirMeta.h"
#include "gdcmDicomDirElement.h"

#include <list>
#include <vector>

//-----------------------------------------------------------------------------
typedef std::list<gdcmDicomDirPatient *>   ListDicomDirPatient;
typedef std::vector<gdcmHeader *>  ListHeader;

typedef GDCM_EXPORT void(gdcmMethod)(void * = NULL);
//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmDicomDir
 * \brief    gdcmDicomDir defines an object representing a DICOMDIR in memory.
 *
 */
class GDCM_EXPORT gdcmDicomDir: public gdcmParser
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
    * \ingroup gdcmParser
    * \brief   Sets the print level for the Dicom Header 
    * \note    0 for Light Print; 1 for 'medium' Print, 2 for Heavy
    */
   void SetPrintLevel(int level) 
      { printLevel = level; };
  /**
    * \ingroup gdcmParser
    * \brief   canonical Printer 
    * \sa    SetPrintLevel
  */     
   virtual void Print(std::ostream &os = std::cout);

// Informations contained in the parser
   virtual bool IsReadable(void);
/**
 * \ingroup gdcmDicomDir
 * \brief   returns a pointer to the gdcmDicomDirMeta for this DICOMDIR.
 */   
   inline gdcmDicomDirMeta   *GetDicomDirMeta()      
      {return metaElems;};
/**
 * \ingroup gdcmDicomDir
 * \brief   returns the PATIENT chained List for this DICOMDIR.
 */      
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

   inline float GetProgress(void)  {return(progress);};
   inline void  AbortProgress(void){abort=true;      };
   inline bool  IsAborted(void)    {return(abort);   };
   
// Adding
  gdcmDicomDirPatient * NewPatient(void);

// Write
   bool Write(std::string fileName);

// Types
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
   void AddObjectToEnd(gdcmDicomDirType type,
                                ListTag::iterator begin,ListTag::iterator end);
   void AddDicomDirMetaToEnd   (ListTag::iterator begin,ListTag::iterator end);
   void AddDicomDirPatientToEnd(ListTag::iterator begin,ListTag::iterator end);
   void AddDicomDirStudyToEnd  (ListTag::iterator begin,ListTag::iterator end);
   void AddDicomDirSerieToEnd  (ListTag::iterator begin,ListTag::iterator end);
   void AddDicomDirImageToEnd  (ListTag::iterator begin,ListTag::iterator end);

   void SetElements(std::string &path,ListHeader &list);
   void SetElement (std::string &path,gdcmDicomDirType type,gdcmHeader *header);
   
   void UpdateDirectoryRecordSequenceLength(void);

   static bool HeaderLessThan(gdcmHeader *header1,gdcmHeader *header2);
   void WriteDicomDirEntries(FILE *_fp);   
   
// Variables
/**
* \brief pointer on *the* gdcmObject 'DicomDirMeta Elements'
*/
   gdcmDicomDirMeta *metaElems;
/**
* \brief chained list of DicomDirPatient (to be exploited recursively)
*/   
   ListDicomDirPatient patients;

   gdcmMethod *startMethod;
   gdcmMethod *progressMethod;
   gdcmMethod *endMethod;
   gdcmMethod *startMethodArgDelete;
   gdcmMethod *progressMethodArgDelete;
   gdcmMethod *endMethodArgDelete;
   void *startArg;
   void *progressArg;
   void *endArg;

   float progress;
   bool abort;
};

//-----------------------------------------------------------------------------
#endif
