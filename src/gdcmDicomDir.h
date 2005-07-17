/*=========================================================================
  
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDir.h,v $
  Language:  C++
  Date:      $Date: 2005/07/17 04:28:55 $
  Version:   $Revision: 1.64 $
  
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
class DicomDirVisit;
class DicomDirImage;
class SQItem;

typedef std::list<DicomDirPatient *>   ListDicomDirPatient;
typedef std::vector<Document *>  VectDocument;

//-----------------------------------------------------------------------------
/**
 * \brief   DicomDir defines an object representing a DICOMDIR in memory
 *  as a tree-like structure DicomDirPatient 
 *                            -> DicomDirStudy 
 *                                -> DicomDirSerie
 *                                    -> DicomDirImage
 */
class GDCM_EXPORT DicomDir: public Document
{
public:
   typedef void Method(void*);

   DicomDir( std::string const &filename, bool parseDir = false ); // Deprecated
   DicomDir(); 
                   
   ~DicomDir();

   bool Load( std::string const &filename );
   bool Load( );
   void Print(std::ostream &os = std::cout, std::string const &indent = "" );
   
   /// Sets the root Directory name to parse, recursively
   void SetDirectoryName(std::string const &dirName) 
        { ParseDir = true; if (Filename != dirName)
                               Filename = dirName, IsDocumentModified = true; };
   /// Accessor to \ref Filename
   virtual void SetFileName(std::string const &fileName) 
                   { ParseDir = false; if (Filename != fileName)
                               Filename = fileName, IsDocumentModified = true; }

   /// DEPRECATED : use SetDirectoryName
   void SetParseDir(bool parseDir)  { ParseDir = parseDir; };
   
   // Informations contained in the parser
   virtual bool IsReadable();

   // Meta
   DicomDirMeta *NewMeta();
   /// Returns a pointer to the DicomDirMeta for this DICOMDIR. 
   DicomDirMeta *GetMeta() { return MetaElems; };

   // Patients
   DicomDirPatient *NewPatient();
   void ClearPatient();

   DicomDirPatient *GetFirstPatient();
   DicomDirPatient *GetNextPatient();

   // Parsing
   void ParseDirectory();

   // Note: the DicomDir:: namespace prefix is needed by Swig in the 
   //       following method declarations. Refer to gdcmPython/gdcm.i
   //       for the reasons of this unnecessary notation at C++ level.
   void SetStartMethod(    DicomDir::Method *method,
                           void *arg = NULL,
                           DicomDir::Method *argDelete = NULL );
   void SetProgressMethod( DicomDir::Method *method, 
                           void *arg = NULL,
                           DicomDir::Method *argDelete = NULL );
   void SetEndMethod(      DicomDir::Method *method,
                           void *arg = NULL, 
                           DicomDir::Method *argDelete = NULL );
   void SetStartMethodArgDelete   ( DicomDir::Method *m );
   void SetProgressMethodArgDelete( DicomDir::Method *m );
   void SetEndMethodArgDelete     ( DicomDir::Method *m );

   /// GetProgress GetProgress
   float GetProgress()   { return Progress; };
   /// AbortProgress AbortProgress
   void  AbortProgress() { Abort = true; };
   /// IsAborted IsAborted
   bool  IsAborted() { return Abort; };

   // Write
   bool WriteDicomDir(std::string const &fileName);

   bool AnonymizeDicomDir();

   /// Types of the DicomDirObject within the DicomDir
   typedef enum
   {
      GDCM_DICOMDIR_NONE,
      GDCM_DICOMDIR_META,
      GDCM_DICOMDIR_PATIENT,
      GDCM_DICOMDIR_STUDY,
      GDCM_DICOMDIR_SERIE,
      GDCM_DICOMDIR_VISIT,
      GDCM_DICOMDIR_IMAGE
   } DicomDirType;
   
protected:
   void CreateDicomDirChainedList(std::string const &path);
   void CallStartMethod();
   void CallProgressMethod();
   void CallEndMethod();

private:
   void Initialize();
   void CreateDicomDir();
   bool DoTheLoadingJob();
   bool AddPatientToEnd(DicomDirPatient *dd);
   bool AddStudyToEnd  (DicomDirStudy *dd);
   bool AddSerieToEnd  (DicomDirSerie *dd);
   bool AddVisitToEnd  (DicomDirVisit *dd);
   bool AddImageToEnd  (DicomDirImage *dd);

   void SetElements(std::string const &path, VectDocument const &list);
   void SetElement (std::string const &path, DicomDirType type,
                    Document *header);
   void MoveSQItem(DocEntrySet *dst, DocEntrySet *src);

   static bool HeaderLessThan(Document *header1, Document *header2);
   
// Variables

   /// Pointer on *the* DicomDirObject 'DicomDirMeta Elements'
   DicomDirMeta *MetaElems;

   /// Chained list of DicomDirPatient (to be exploited hierarchicaly) 
   ListDicomDirPatient Patients;
   ListDicomDirPatient::iterator ItPatient;

   /// pointer to the initialisation method for any progress bar   
   Method *StartMethod;
   /// pointer to the incrementation method for any progress bar
   Method *ProgressMethod;
   /// pointer to the termination method for any progress bar
   Method *EndMethod;
   /// pointer to the ??? method for any progress bar   
   Method *StartMethodArgDelete;
   /// pointer to the ??? method for any progress bar
   Method* ProgressMethodArgDelete;
   /// pointer to the ??? method for any progress bar
   Method *EndMethodArgDelete;
   /// pointer to the ??? for any progress bar   
   void *StartArg;
   /// pointer to the ??? for any progress bar
   void *ProgressArg;
   /// pointer to the ??? for any progress bar   
   void *EndArg;
   /// value of the ??? for any progress bar
   float Progress;
   /// value of the ??? for any progress bar   
   bool Abort;
   bool ParseDir;
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
