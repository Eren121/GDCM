/*=========================================================================
  
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDir.h,v $
  Language:  C++
  Date:      $Date: 2004/10/09 03:21:55 $
  Version:   $Revision: 1.33 $
  
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
  
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

typedef GDCM_EXPORT void(gdcmMethod)(void* = NULL);
//-----------------------------------------------------------------------------

/**
 * \ingroup gdcmDicomDir
 * \brief    gdcmDicomDir defines an object representing a DICOMDIR in memory.
 *
 */
class GDCM_EXPORT gdcmDicomDir: public gdcmDocument
{
public:
   gdcmDicomDir( std::string const & fileName, bool parseDir = false );
   gdcmDicomDir(); 
                   
   ~gdcmDicomDir();

   /// \brief   canonical Printer 
   /// \sa    SetPrintLevel
   virtual void Print(std::ostream &os = std::cout);

   /// Informations contained in the parser
   virtual bool IsReadable();

   /// Returns a pointer to the gdcmDicomDirMeta for this DICOMDIR. 
   gdcmDicomDirMeta* GetDicomDirMeta() { return MetaElems; };

   /// Returns the PATIENT chained List for this DICOMDIR.    
   ListDicomDirPatient &GetDicomDirPatients() { return Patients; };

   /// Parsing
   void ParseDirectory();
   
   void SetStartMethod(gdcmMethod*, void* = NULL, gdcmMethod* = NULL);
   void SetStartMethodArgDelete(gdcmMethod*);
   void SetProgressMethod(gdcmMethod* ,void* = NULL, gdcmMethod* = NULL);
   void SetProgressMethodArgDelete(gdcmMethod*);
   void SetEndMethod(gdcmMethod*, void* = NULL,gdcmMethod* = NULL);
   void SetEndMethodArgDelete(gdcmMethod*);

   /// GetProgress GetProgress
   float GetProgress()  { return Progress; };

   /// AbortProgress AbortProgress
   void  AbortProgress() { Abort = true; };

   /// IsAborted IsAborted
   bool  IsAborted() { return Abort; };
   
   /// Adding
   gdcmDicomDirMeta*    NewMeta();
   gdcmDicomDirPatient* NewPatient();

   /// Write  
   bool WriteDicomDir(std::string const & fileName);

   /// Types of the gdcmDicomDirObject within the gdcmDicomDir
   typedef enum
   {
      GDCM_DICOMDIR_NONE,
      GDCM_DICOMDIR_META,
      GDCM_DICOMDIR_PATIENT,
      GDCM_DICOMDIR_STUDY,
      GDCM_DICOMDIR_SERIE,
      GDCM_DICOMDIR_IMAGE
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
   void AddDicomDirPatientToEnd(gdcmSQItem* s);
   void AddDicomDirStudyToEnd  (gdcmSQItem* s);
   void AddDicomDirSerieToEnd  (gdcmSQItem* s);
   void AddDicomDirImageToEnd  (gdcmSQItem* s);

   void SetElements(std::string &path, VectDocument &list);
   void SetElement (std::string &path,gdcmDicomDirType type,
                    gdcmDocument* header);

   static bool HeaderLessThan(gdcmDocument* header1,gdcmDocument* header2);
   
// Variables

   /// Pointer on *the* gdcmDicomDirObject 'DicomDirMeta Elements'
   gdcmDicomDirMeta* MetaElems;

   /// Chained list of DicomDirPatient (to be exploited recursively) 
   ListDicomDirPatient Patients;

   /// pointer to the initialisation method for any progress bar   
   gdcmMethod* StartMethod;
   /// pointer to the incrementation method for any progress bar
   gdcmMethod* ProgressMethod;
   /// pointer to the termination method for any progress bar
   gdcmMethod* EndMethod;
   /// pointer to the ??? method for any progress bar   
   gdcmMethod* StartMethodArgDelete;
   /// pointer to the ??? method for any progress bar
   gdcmMethod* ProgressMethodArgDelete;
   /// pointer to the ??? method for any progress bar
   gdcmMethod* EndMethodArgDelete;
   /// pointer to the ??? for any progress bar   
   void* StartArg;
   /// pointer to the ??? for any progress bar
   void* ProgressArg;
   /// pointer to the ??? for any progress bar   
   void* EndArg;
   /// value of the ??? for any progress bar
   float Progress;
   /// value of the ??? for any progress bar   
   bool Abort;
};

//-----------------------------------------------------------------------------
#endif
