/*=========================================================================
  
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDir.h,v $
  Language:  C++
  Date:      $Date: 2004/07/17 22:36:55 $
  Version:   $Revision: 1.24 $
  
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
  
=========================================================================*/

#ifndef GDCMDICOMDIR_H
#define GDCMDICOMDIR_H

#include "gdcmCommon.h"
#include "gdcmDocument.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirMeta.h"
#include "gdcmDicomDirElement.h"

#include <list>
#include <vector>

//-----------------------------------------------------------------------------
typedef std::list<gdcmDicomDirPatient*>   ListDicomDirPatient;
typedef std::vector<gdcmDocument*>  VectDocument;

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
   gdcmDicomDir(std::string const & fileName, 
                bool parseDir = false,
                bool exception_on_error = false);
   gdcmDicomDir(bool exception_on_error = false); 
                   
   ~gdcmDicomDir();

   /// \brief Sets the print level for the Dicom Header 
   /// \note    0 for Light Print; 1 for 'medium' Print, 2 for Heavy
   void SetPrintLevel(int level) 
      { printLevel = level; };

   /// \brief   canonical Printer 
   /// \sa    SetPrintLevel
   virtual void Print(std::ostream &os = std::cout);

// Informations contained in the parser
   virtual bool IsReadable();

   /// Returns a pointer to the gdcmDicomDirMeta for this DICOMDIR. 
   gdcmDicomDirMeta* GetDicomDirMeta() { return metaElems; };

   /// Returns the PATIENT chained List for this DICOMDIR.    
   ListDicomDirPatient &GetDicomDirPatients() { return patients; };

// Parsing
   void ParseDirectory();
   
   void SetStartMethod(gdcmMethod *,void * =NULL,gdcmMethod * =NULL);
   void SetStartMethodArgDelete(gdcmMethod *);
   void SetProgressMethod(gdcmMethod *,void * =NULL,gdcmMethod * =NULL);
   void SetProgressMethodArgDelete(gdcmMethod *);
   void SetEndMethod(gdcmMethod *,void * =NULL,gdcmMethod * =NULL);
   void SetEndMethodArgDelete(gdcmMethod *);

   /// GetProgress GetProgress
   float GetProgress()  { return progress; };

   /// AbortProgress AbortProgress
   void  AbortProgress() {abort = true; };

   /// IsAborted IsAborted
   bool  IsAborted() { return abort; };
   
// Adding
   gdcmDicomDirMeta*    NewMeta();
   gdcmDicomDirPatient* NewPatient();

// Write
   void WriteEntries(FILE *_fp);   
   bool Write(std::string const & fileName);

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
   void CreateDicomDirChainedList(std::string const & path);
   void CallStartMethod();
   void CallProgressMethod();
   void CallEndMethod();

private:
   void Initialize();
   void CreateDicomDir();
   void AddDicomDirMeta();
   void AddDicomDirPatientToEnd(gdcmSQItem *s);
   void AddDicomDirStudyToEnd  (gdcmSQItem *s);
   void AddDicomDirSerieToEnd  (gdcmSQItem *s);
   void AddDicomDirImageToEnd  (gdcmSQItem *s);

   void SetElements(std::string &path, VectDocument &list);
   void SetElement (std::string &path,gdcmDicomDirType type,
                    gdcmDocument *header);
   
   void UpdateDirectoryRecordSequenceLength();

   static bool HeaderLessThan(gdcmDocument *header1,gdcmDocument *header2);
   
// Variables

   /// Pointer on *the* gdcmObject 'DicomDirMeta Elements'
   gdcmDicomDirMeta *metaElems;

   /// Chained list of DicomDirPatient (to be exploited recursively) 
   ListDicomDirPatient patients;

/// pointer to the initialisation method for any progress bar   
   gdcmMethod *startMethod;
/// pointer to the incrementation method for any progress bar
    gdcmMethod *progressMethod;
/// pointer to the termination method for any progress bar
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
