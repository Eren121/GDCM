// gdcmDicomDir.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMDIR_H
#define GDCMDICOMDIR_H

#include "gdcmHeader.h"
#include "gdcmCommon.h"
#include "gdcmPatient.h"
#include "gdcmMeta.h"
#include "gdcmDicomDirElement.h"

#include <list>
#include <vector>

//-----------------------------------------------------------------------------
typedef std::list<gdcmPatient *>   ListPatient;
typedef std::vector<gdcmHeader *>  ListHeader;

typedef GDCM_EXPORT void( gdcmMethod)(void * =NULL);
//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmDicomDir
 * \brief    gdcmDicomDir defines an object representing a DICOMDIR in memory.
 *
 */
class GDCM_EXPORT gdcmDicomDir: public gdcmParser
{
public:
//   gdcmDicomDir(ListTag *l,         bool exception_on_error = false);
   gdcmDicomDir(const char *FileName, 
                bool parseDir = false,
                bool exception_on_error = false);
   
   ~gdcmDicomDir(void);

   void SetPrintLevel(int level) { printLevel = level; };
   virtual void Print(std::ostream &os = std::cout);

// Informations contained in the parser
   virtual bool IsReadable(void);
   inline gdcmMeta   *GetMeta()      {return metaElems;};
   inline ListPatient &GetPatients() {return patients;};

// Parsing
   void ParseDirectory(void);

   inline void SetStartMethod(gdcmMethod *method,void *arg=NULL)    {startMethod=method;startArg=arg;};
   inline void SetProgressMethod(gdcmMethod *method,void *arg=NULL) {progressMethod=method;progressArg=arg;};
   inline void SetEndMethod(gdcmMethod *method,void *arg=NULL)      {endMethod=method;endArg=arg;};

   inline float GetProgress(void)                   {return(progress);};

   inline void AbortProgress(void)                  {abort=true;};
   inline bool IsAborted(void)                      {return(abort);};

// Write
   bool Write(std::string fileName);

// Types
   typedef enum
   {
      GDCM_NONE,
      GDCM_META,
      GDCM_PATIENT,
      GDCM_STUDY,
      GDCM_SERIE,
      GDCM_IMAGE,
   } gdcmDicomDirType;
   
protected:
   void NewDicomDir(std::string path);
   std::string GetPath(void);

   void CallStartMethod(void);
   void CallProgressMethod(void);
   void CallEndMethod(void);

private:
   void CreateDicomDir(void);
   void AddObjectToEnd(gdcmDicomDirType type,
                        ListTag::iterator begin,ListTag::iterator end);
   void AddMetaToEnd   (ListTag::iterator begin,ListTag::iterator end);
   void AddPatientToEnd(ListTag::iterator begin,ListTag::iterator end);
   void AddStudyToEnd  (ListTag::iterator begin,ListTag::iterator end);
   void AddSerieToEnd  (ListTag::iterator begin,ListTag::iterator end);
   void AddImageToEnd  (ListTag::iterator begin,ListTag::iterator end);

   void SetElements(std::string &path,ListHeader &list);
   void SetElement (std::string &path,gdcmDicomDirType type,gdcmHeader *header);

   static bool HeaderLessThan(gdcmHeader *header1,gdcmHeader *header2);

   gdcmMeta *metaElems;
   ListPatient patients;

   gdcmMethod *startMethod;
   gdcmMethod *progressMethod;
   gdcmMethod *endMethod;
   void *startArg;
   void *progressArg;
   void *endArg;

   float progress;
   bool abort;
};

//-----------------------------------------------------------------------------
#endif
