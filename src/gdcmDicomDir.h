// gdcmDicomDir.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMDIR_H
#define GDCMDICOMDIR_H

#include "gdcmHeader.h"
#include "gdcmCommon.h"
#include "gdcmPatient.h"
#include "gdcmDicomDirElement.h"

#include <list>
#include <vector>

//-----------------------------------------------------------------------------
typedef std::list<gdcmPatient *> ListPatient;
typedef std::vector<gdcmHeader *>  ListHeader;

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmDicomDir
 * \brief    gdcmDicomDir defines an object representing a DICOMDIR in memory.
 *
 */
class GDCM_EXPORT gdcmDicomDir: public gdcmParser
{
public:
//   gdcmDicomDir(ListTag *l,          bool exception_on_error = false);
   gdcmDicomDir(const char *FileName, bool parseDir = false,
                bool exception_on_error = false);
   
   ~gdcmDicomDir(void);

   void SetPrintLevel(int level) { printLevel = level; };
   virtual void Print(std::ostream &os = std::cout);

   inline ListPatient &GetPatients() {return patients;};

// Write
   bool Write(std::string fileName);
   void ParseDirectory(void);

// Types
   typedef enum
   {
      GDCM_NONE,
      GDCM_PATIENT,
      GDCM_STUDY,
      GDCM_SERIE,
      GDCM_IMAGE,
   } gdcmDicomDirType;
   
protected:
   void NewDicomDir(std::string path);
   std::string GetPath(void);

private:
   void CreateDicomDir(void);
   void AddObjectToEnd(gdcmDicomDirType type,ListTag::iterator begin,ListTag::iterator end);
   void AddPatientToEnd(ListTag::iterator begin,ListTag::iterator end);
   void AddStudyToEnd(ListTag::iterator begin,ListTag::iterator end);
   void AddSerieToEnd(ListTag::iterator begin,ListTag::iterator end);
   void AddImageToEnd(ListTag::iterator begin,ListTag::iterator end);

   void SetElements(std::string &path,ListHeader &list);
   void SetElement(std::string &path,gdcmDicomDirType type,gdcmHeader *header);

   static bool HeaderLessThan(gdcmHeader *header1,gdcmHeader *header2);

   ListPatient patients;
};

//-----------------------------------------------------------------------------
#endif
