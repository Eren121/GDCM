/*=========================================================================
  
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDir.h,v $
  Language:  C++
  Date:      $Date: 2004/12/16 13:46:36 $
  Version:   $Revision: 1.42 $
  
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
  
=========================================================================*/

#ifndef GDCMDICOMDIR_H
#define GDCMDICOMDIR_H

#include "gdcmDocument.h"

#include <list>
#include <vector>

namespace gdcm 
{
//-----------------------------------------------------------------------------
class DicomDirPatient;
class DicomDirMeta;
class DicomDirElement;
class DicomDirStudy;
class DicomDirSerie;
class DicomDirImage;
class SQItem;

typedef std::list<DicomDirPatient*>   ListDicomDirPatient;
typedef std::vector<Document*>  VectDocument;

//-----------------------------------------------------------------------------

/**
 * \ingroup DicomDir
 * \brief    DicomDir defines an object representing a DICOMDIR in memory.
 *
 */
class GDCM_EXPORT DicomDir: public Document
{
public:
   typedef void(Method)(void*);

   DicomDir( std::string const & filename, bool parseDir = false );
   DicomDir(); 
                   
   ~DicomDir();

   /// \brief   canonical Printer 
   void Print(std::ostream &os = std::cout);

   /// Informations contained in the parser
   virtual bool IsReadable();

   /// Returns a pointer to the DicomDirMeta for this DICOMDIR. 
   DicomDirMeta* GetDicomDirMeta() { return MetaElems; };

   /// Returns the PATIENT chained List for this DICOMDIR.    
   ListDicomDirPatient const & GetDicomDirPatients() const { return Patients; };

   /// Parsing
   void ParseDirectory();
   
   // Note: the DicomDir:: namespace prefix is needed by Swig in the 
   //       following method declarations. Refer to gdcmPython/gdcm.i
   //       for the reasons of this unecessary notation at C++ level.
   void SetStartMethod(    DicomDir::Method*,
                           void* = NULL,
                           DicomDir::Method* = NULL );
   void SetProgressMethod( DicomDir::Method*, 
                           void* = NULL,
                           DicomDir::Method* = NULL );
   void SetEndMethod(      DicomDir::Method*,
                           void* = NULL, 
                           DicomDir::Method* = NULL );
   void SetStartMethodArgDelete( DicomDir::Method* );
   void SetProgressMethodArgDelete( DicomDir::Method* );
   void SetEndMethodArgDelete( DicomDir::Method* );

   /// GetProgress GetProgress
   float GetProgress()  { return Progress; };

   /// AbortProgress AbortProgress
   void  AbortProgress() { Abort = true; };

   /// IsAborted IsAborted
   bool  IsAborted() { return Abort; };
   
   /// Adding
   DicomDirMeta*    NewMeta();
   DicomDirPatient* NewPatient();

   /// Write  
   bool WriteDicomDir(std::string const & fileName);

   /// Types of the DicomDirObject within the DicomDir
   typedef enum
   {
      GDCM_DICOMDIR_NONE,
      GDCM_DICOMDIR_META,
      GDCM_DICOMDIR_PATIENT,
      GDCM_DICOMDIR_STUDY,
      GDCM_DICOMDIR_SERIE,
      GDCM_DICOMDIR_IMAGE
   } DicomDirType;
   
protected:
   void CreateDicomDirChainedList(std::string const & path);
   void CallStartMethod();
   void CallProgressMethod();
   void CallEndMethod();

private:
   void Initialize();
   void CreateDicomDir();

   bool AddDicomDirMeta();
   bool AddDicomDirPatientToEnd(DicomDirPatient* dd);
   bool AddDicomDirStudyToEnd  (DicomDirStudy* dd);
   bool AddDicomDirSerieToEnd  (DicomDirSerie* dd);
   bool AddDicomDirImageToEnd  (DicomDirImage* dd);

   void SetElements(std::string const & path, VectDocument const &list);
   void SetElement (std::string const & path, DicomDirType type,
                    Document* header);
   void MoveSQItem(SQItem* dst,SQItem *src);

   static bool HeaderLessThan(Document* header1, Document* header2);
   
// Variables

   /// Pointer on *the* DicomDirObject 'DicomDirMeta Elements'
   DicomDirMeta* MetaElems;

   /// Chained list of DicomDirPatient (to be exploited recursively) 
   ListDicomDirPatient Patients;

   /// pointer to the initialisation method for any progress bar   
   Method* StartMethod;
   /// pointer to the incrementation method for any progress bar
   Method* ProgressMethod;
   /// pointer to the termination method for any progress bar
   Method* EndMethod;
   /// pointer to the ??? method for any progress bar   
   Method* StartMethodArgDelete;
   /// pointer to the ??? method for any progress bar
   Method* ProgressMethodArgDelete;
   /// pointer to the ??? method for any progress bar
   Method* EndMethodArgDelete;
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
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
