/*=========================================================================
  
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2010/04/09 15:23:40 $
  Version:   $Revision: 1.198 $
  
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
  
=========================================================================*/

//-----------------------------------------------------------------------------
//  For full DICOMDIR description, see:
//  PS 3.3-2003, pages 731-750
//-----------------------------------------------------------------------------
#include "gdcmDicomDir.h"
#include "gdcmDicomDirObject.h"
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirVisit.h"
#include "gdcmDicomDirImage.h"
#include "gdcmDicomDirPrivate.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirMeta.h"
#include "gdcmDicomDirElement.h"
#include "gdcmDirList.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"
#include "gdcmGlobal.h"
#include "gdcmFile.h"
#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmDataEntry.h"
#include "gdcmCommandManager.h"

#include <fstream>
#include <string>
#include <algorithm>
#include <sys/types.h>

#ifdef _MSC_VER
#   define getcwd _getcwd
#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)
#   include <direct.h>
#else
#   include <unistd.h>
#endif

#if defined(__BORLANDC__)
   #include <mem.h> // for memset
#endif

// ----------------------------------------------------------------------------
//         Note for future developpers
// ----------------------------------------------------------------------------
//
//  Dicom PS 3.3 describes the relationship between Directory Records, as follow
//    (see also PS 4.3, 2004, page 50 for Entity-Relationship model)
//
//  Directory Record Type      Directory Record Types which may be included
//                                in the next lower-level directory Entity
//
// (Root directory Entity)     PATIENT, TOPIC, PRIVATE
//
// PATIENT                     STUDY, PRIVATE
//
// STUDY                       SERIES, VISIT, RESULTS, STUDY COMPONENT, PRIVATE
//
// SERIES                      IMAGE, OVERLAYS, MODALITY LUT, VOI LUT,
//                             CURVE, STORED PRINT, RT DOSE, RT STRUCTURE SET
//                             RT PLAN, RT TREAT RECORD, PRESENTATION, WAVEFORM,
//                             SR DOCUMENT, KEY OBJECT DOC, SPECTROSCOPY,
//                             RAW DATA, REGISTRATION, FIDUCIAL, PRIVATE,
//                             ENCAP DOC
// IMAGE
// OVERLAY
// MODALITY LUT
// VOI LUT
// CURVE
// STORED PRINT
// RT DOSE
// RT STRUCTURE SET
// RT PLAN
// RT TREAT RECORD
// PRESENTATION
// WAVEFORM
// SR DOCUMENT
// KEY OBJECT DOC
// SPECTROSCOPY
// RAW DATA
// REGISTRATION
// FIDUCIAL
// PRIVATE
// ENCAP DOC
// 

/*

// see also : ftp://medical.nema.org/medical/dicom/final/cp343_ft.doc

RELATIONSHIP BETWEEN DIRECTORY RECORDS

Directory Record Type      Directory Record Types which may be included 
                           in the next lower-level directory Entity

(Root Directory Entity)    PATIENT, TOPIC, PRIVATE

PATIENT                    STUDY, PRIVATE

STUDY                      SERIES, VISIT, RESULTS, STUDY COMPONENT, PRIVATE

SERIES                     IMAGE, OVERLAY, MODALITY LUT, VOI LUT, CURVE, 
                           STORED PRINT, RT DOSE, RT STRUCTURE SET, RT PLAN, 
                           RT TREAT RECORD, PRESENTATION, WAVEFORM, SR DOCUMENT,
                            KEY OBJECT DOC, SPECTROSCOPY, RAW DATA, PRIVATE

IMAGE                      PRIVATE
OVERLAY                    PRIVATE
MODALITY LUT               PRIVATE
VOI LUT                    PRIVATE
CURVE                      PRIVATE
STORED PRINT               PRIVATE
RT DOSE                    PRIVATE
RT STRUCTURE SET           PRIVATE
RT PLAN                    PRIVATE
RT TREAT RECORD            PRIVATE
PRESENTATION               PRIVATE
WAVEFORM                   PRIVATE
SR DOCUMENT                PRIVATE
KEY OBJECT DOC             PRIVATE
SPECTROSCOPY               PRIVATE
RAW DATA                   PRIVATE

TOPIC                      STUDY, SERIES, IMAGE, OVERLAY, MODALITY LUT, VOI LUT,
                           CURVE, STORED PRINT, RT DOSE, RT STRUCTURE SET, 
                           RT PLAN, RT TREAT RECORD, PRESENTATION, WAVEFORM, 
                           SR DOCUMENT, KEY OBJECT DOC, SPECTROSCOPY, RAW DATA, 
                           PRIVATE

VISIT                      PRIVATE

RESULTS                    INTERPRETATION, PRIVATE

INTERPRETATION             PRIVATE
STUDY COMPONENT            PRIVATE
PRIVATE                    PRIVATE, (any of the above as privately defined)
MRDR                      (Not applicable)

Note :   Directory Record Types PRINT QUEUE, FILM SESSION, FILM BOX, and 
         IMAGE BOX  were previously defined in DICOM.  They have been retired.  
         See PS 3.3-1998.
*/

// ----------------------
// The current gdcm version only deals with :
//
// (Root directory Entity)     PATIENT
// PATIENT                     STUDY
// STUDY                       SERIES
// STUDY                       VISIT 
// SERIES                      IMAGE
// IMAGE                       /
//
// DicomDir::CreateDicomDir will have to be completed
// Treelike structure management will have to be upgraded
// ----------------------------------------------------------------------------
    
namespace GDCM_NAME_SPACE 
{
//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor : creates an empty DicomDir
 */
DicomDir::DicomDir()
{
   Initialize();  // sets all private fields to NULL
   ParseDir = false;
   NewMeta();
}

//#ifndef GDCM_LEGACY_REMOVE
/**
 * \brief Constructor Parses recursively the directory and creates the DicomDir
 *        or uses an already built DICOMDIR, depending on 'parseDir' value.
 * @param fileName  name 
 *                      - of the root directory (parseDir = true)
 *                      - of the DICOMDIR       (parseDir = false)
 * @param parseDir boolean
 *                      - true if user passed an entry point 
 *                        and wants to explore recursively the directories
 *                      - false if user passed an already built DICOMDIR file
 *                        and wants to use it 
 * @deprecated use : new DicomDir() + [ SetLoadMode(lm) + ] SetDirectoryName(name)
 *              or : new DicomDir() + SetFileName(name)
 */
 /*
DicomDir::DicomDir(std::string const &fileName, bool parseDir ):
   Document( )
{
   // At this step, Document constructor is already executed,
   // whatever user passed (either a root directory or a DICOMDIR)
   // and whatever the value of parseDir was.
   // (nothing is checked in Document constructor, to avoid overhead)

   ParseDir = parseDir;
   SetLoadMode (LD_ALL); // concerns only dicom files
   SetFileName( fileName );
   Load( );
}
*/
//#endif

/**
 * \brief  Canonical destructor 
 */
DicomDir::~DicomDir() 
{
   ClearPatient();
   if ( MetaElems )
   {
      MetaElems->Delete();
   }
}

//-----------------------------------------------------------------------------
// Public

/**
 * \brief   Loader. use SetFileName(fn) 
 *                  or SetLoadMode(lm) + SetDirectoryName(dn)  before !  
 * @return false if file cannot be open or no swap info was found,
 *         or no tag was found.
 */
bool DicomDir::Load( ) 
{
   if (!ParseDir)
   {
      if ( ! this->Document::Load( ) )
         return false;
   }
   return DoTheLoadingJob( );   
}
//#ifndef GDCM_LEGACY_REMOVE
/**
 * \brief   Loader. (DEPRECATED : kept not to break the API)
 * @param   fileName file to be open for parsing
 * @return false if file cannot be open or no swap info was found,
 *         or no tag was found.
 * @deprecated use SetFileName(n) + Load() instead
 */
 /*
bool DicomDir::Load(std::string const &fileName ) 
{
   // We should clean out anything that already exists.
   Initialize();  // sets all private fields to NULL

   SetFileName( fileName );
   if (!ParseDir)
   {
      if ( ! this->Document::Load( ) )
         return false;
   }
   return DoTheLoadingJob( );
}
*/
//#endif

/**
 * \brief   Does the Loading Job (internal use only)
 * @return false if file cannot be open or no swap info was found,
 *         or no tag was found.
 */
bool DicomDir::DoTheLoadingJob( ) 
{
   Progress = 0.0f;
   Abort = false;

   if (!ParseDir)
   {
   // Only if user passed a DICOMDIR
   // ------------------------------
      Fp = 0;
      if (!Document::Load() )
      {
         return false;
      }

      if ( GetFirstEntry() == 0 ) // when user passed a Directory to parse
      {
         gdcmWarningMacro( "Entry HT empty for file: "<< GetFileName());
         return false;
      }
      // Directory record sequence
      DocEntry *e = GetDocEntry(0x0004, 0x1220);
      if ( !e )
      {
         gdcmWarningMacro( "NO 'Directory record sequence' (0x0004,0x1220)"
                          << " in file " << GetFileName());
         return false;
      }
      else
      {
         CreateDicomDir();
      }
   }
   else
   {
   // Only if user passed a root directory
   // ------------------------------------
      if ( GetFileName() == "." )
      {
         // user passed '.' as Name
         // we get current directory name
         char buf[2048];
         const char *cwd = getcwd(buf, 2048);
         if( cwd )
         {
            SetFileName( buf ); // will be converted into a string
         }
         else
         {
            gdcmErrorMacro( "Path was too long to fit on 2048 bytes" );
         }
      }
      NewMeta();
      gdcmDebugMacro( "Parse directory and create the DicomDir : " 
                         << GetFileName() );
      ParseDirectory();
   }
   return true;
}

/**
 * \brief  This predicate, based on hopefully reasonable heuristics,
 *         decides whether or not the current document was properly parsed
 *         and contains the mandatory information for being considered as
 *         a well formed and usable DicomDir.
 * @return true when Document is the one of a reasonable DicomDir,
 *         false otherwise. 
 */
bool DicomDir::IsReadable()
{
   if ( Filetype == Unknown )
   {
      gdcmErrorMacro( "Wrong filetype for " << GetFileName());
      return false;
   }
   if ( !MetaElems )
   {
      gdcmWarningMacro( "Meta Elements missing in DicomDir");
      return false;
   }
   if ( Patients.size() <= 0 )
   {
      gdcmWarningMacro( "NO Patient in DicomDir");
      return false;
   }

   return true;
}

/**
 * \brief   adds *the* Meta to a partially created DICOMDIR
 */  
DicomDirMeta *DicomDir::NewMeta()
{
   if ( MetaElems )
   {
      MetaElems->Delete();
   }
   DocEntry *entry = GetFirstEntry();
   if ( entry )
   { 
      MetaElems = DicomDirMeta::New(true); // true = empty

      entry = GetFirstEntry();
      while( entry )
      {
         if ( dynamic_cast<SeqEntry *>(entry) )
            break;

         MetaElems->AddEntry(entry);
         RemoveEntry(entry);

         entry = GetFirstEntry();
      }
   }
   else  // after root directory parsing
   {
      MetaElems = DicomDirMeta::New(false); // false = not empty
   }
   MetaElems->SetSQItemNumber(0); // To avoid further missprinting
   return MetaElems;  
}

/**
 * \brief   adds a new Patient (with the basic elements) to a partially created
 *          DICOMDIR
 */
DicomDirPatient *DicomDir::NewPatient()
{
   DicomDirPatient *dd = DicomDirPatient::New();
   AddPatientToEnd( dd );
   return dd;
}

/**
 * \brief   Remove all Patients
 */
void DicomDir::ClearPatient()
{
   for(ListDicomDirPatient::iterator cc = Patients.begin();
                                     cc!= Patients.end();
                                   ++cc)
   {
      (*cc)->Unregister();
   }
   Patients.clear();
}

/**
 * \brief   Get the first entry while visiting the DicomDirPatients
 * \return  The first DicomDirPatient if found, otherwhise NULL
 */ 
DicomDirPatient *DicomDir::GetFirstPatient()
{
   ItPatient = Patients.begin();
   if ( ItPatient != Patients.end() )
      return *ItPatient;
   return NULL;
}

/**
 * \brief   Get the next entry while visiting the DicomDirPatients
 * \note : meaningfull only if GetFirstEntry already called
 * \return  The next DicomDirPatient if found, otherwhise NULL
 */
DicomDirPatient *DicomDir::GetNextPatient()
{
   gdcmAssertMacro (ItPatient != Patients.end());

   ++ItPatient;
   if ( ItPatient != Patients.end() )
      return *ItPatient;
   return NULL;
}

/**
 * \brief  fills the whole structure, starting from a root Directory
 */
void DicomDir::ParseDirectory()
{
   CreateDicomDirChainedList( GetFileName() );
   CreateDicomDir();
}

/**
 * \brief    writes on disc a DICOMDIR
 * \ warning does NOT add the missing elements in the header :
 *           it's up to the user doing it !
 * @param  fileName file to be written to 
 * @return false only when fail to open
 */
 
bool DicomDir::Write(std::string const &fileName) 
{  
   int i;
   uint16_t sq[6] = { 0x0004, 0x1220, 0x5153, 0x0000, 0xffff, 0xffff };
   uint16_t sqt[4]= { 0xfffe, 0xe0dd, 0x0000, 0x0000 };

   std::ofstream *fp = new std::ofstream(fileName.c_str(),  
                                         std::ios::out | std::ios::binary);
   if ( !fp )
   {
      gdcmWarningMacro("Failed to open(write) File: " << fileName.c_str());
      return false;
   }

   char filePreamble[128];
   memset(filePreamble, 0, 128);
   fp->write(filePreamble, 128);
   binary_write( *fp, "DICM");
 
   DicomDirMeta *ptrMeta = GetMeta();
   ptrMeta->WriteContent(fp, ExplicitVR, true, false);
   
   // force writing 0004|1220 [SQ ], that CANNOT exist within DicomDirMeta
   for(i=0;i<6;++i)
   {
      binary_write(*fp, sq[i]);
   }
        
   for(ListDicomDirPatient::iterator cc  = Patients.begin();
                                     cc != Patients.end();
                                   ++cc )
   {
      (*cc)->WriteContent( fp, ExplicitVR, false, true );
   }
   
   // force writing Sequence Delimitation Item
   for(i=0;i<4;++i)
   {
      binary_write(*fp, sqt[i]);  // fffe e0dd 0000 0000 
   }

   fp->close();
   delete fp;

   return true;
}

/**
 * \brief    Anonymize a DICOMDIR
 * @return true 
 */
 
bool DicomDir::Anonymize() 
{
   DataEntry *v;
   // Something clever to be found to forge the Patient names
   std::ostringstream s;
   int i = 1;
   for(ListDicomDirPatient::iterator cc = Patients.begin();
                                     cc!= Patients.end();
                                   ++cc)
   {
      s << i;
      v = (*cc)->GetDataEntry(0x0010, 0x0010) ; // Patient's Name
      if (v)
      {
         v->SetString(s.str());
      }

      v = (*cc)->GetDataEntry(0x0010, 0x0020) ; // Patient ID
      if (v)
      {
         v->SetString(" ");
      }

      v = (*cc)->GetDataEntry(0x0010, 0x0030) ; // Patient's BirthDate
      if (v)
      {
         v->SetString(" ");
      }
      s << "";
      i++;
   }
   return true;
}

/**
 * \brief Copies all the attributes from an other DocEntrySet 
 * @param set entry to copy from
 * @remarks The contained DocEntries are not copied, only referenced
 */
void DicomDir::Copy(DocEntrySet *set)
{
   // Remove all previous childs
   ClearPatient();

   Document::Copy(set);

   DicomDir *dd = dynamic_cast<DicomDir *>(set);
   if( dd )
   {
      if(MetaElems)
         MetaElems->Unregister();
      MetaElems = dd->MetaElems;
      if(MetaElems)
         MetaElems->Register();

      Patients = dd->Patients;
      for(ItPatient = Patients.begin();ItPatient != Patients.end();++ItPatient)
         (*ItPatient)->Register();
   }
}

//-----------------------------------------------------------------------------
// Protected
/**
 * \brief create a Document-like chained list from a root Directory 
 * @param path entry point of the tree-like structure
 */
void DicomDir::CreateDicomDirChainedList(std::string const &path)
{
   CallStartMethod();
   DirList dirList(path,1); // gets recursively the file list
   unsigned int count = 0;
   VectDocument list;
   File *f;

   DirListType fileList = dirList.GetFilenames();
   unsigned int nbFile = fileList.size();
   for( DirListType::iterator it  = fileList.begin();
                              it != fileList.end();
                              ++it )
   {
      Progress = (float)(count+1)/(float)nbFile;
      CallProgressMethod();
      if ( Abort )
      {
         break;
      }

      f = File::New( );
      f->SetLoadMode(LoadMode); // we allow user not to load Sequences, 
                                //        or Shadow groups, or ......
      f->SetFileName( it->c_str() );
      f->Load( );

      if ( f->IsReadable() )
      {
         // Add the file to the chained list:
         list.push_back(f);
         gdcmDebugMacro( "Readable " << it->c_str() );
       }
       else
       {
          f->Delete();
       }
       count++;
   }
   // sorts Patient/Study/Serie/
   std::sort(list.begin(), list.end(), DicomDir::HeaderLessThan );
   
   std::string tmp = dirList.GetDirName();      
   //for each File of the chained list, add/update the Patient/Study/Serie/Image info
   SetElements(tmp, list);
   CallEndMethod();

   for(VectDocument::iterator itDoc=list.begin();
       itDoc!=list.end();
       ++itDoc)
   {
      dynamic_cast<File *>(*itDoc)->Delete();
   }
}


//-----------------------------------------------------------------------------
// Private
/**
 * \brief Sets all fields to NULL
 */
void DicomDir::Initialize()
{
   Progress = 0.0;
   Abort = false;

   MetaElems = NULL;   
}

/**
 * \brief create a 'gdcm::DicomDir' from a DICOMDIR Header 
 */
void DicomDir::CreateDicomDir()
{
   // The SeqEntries of "Directory Record Sequence" are parsed. 
   //  When a DicomDir tag ("PATIENT", "STUDY", "SERIE", "IMAGE") is found :
   //                    N.B. :  VISIT, PRIVATE not fully dealt with
   //  1 - we save the beginning iterator
   //  2 - we continue to parse
   //  3 - we find an other tag
   //       + we create the object for the precedent tag
   //       + loop to 1 -
   gdcmDebugMacro("Create DicomDir");

   // Directory record sequence
   DocEntry *e = GetDocEntry(0x0004, 0x1220);
   if ( !e )
   {
      gdcmWarningMacro( "No Directory Record Sequence (0004,1220) found");
      return;         
   }
   
   SeqEntry *s = dynamic_cast<SeqEntry *>(e);
   if ( !s )
   {
      gdcmWarningMacro( "Element (0004,1220) is not a Sequence ?!?");
      return;
   }

   NewMeta();
   
   DocEntry *d;
   std::string v;
   SQItem *si;

   SQItem *tmpSI=s->GetFirstSQItem();
   while(tmpSI)
   {
      d = tmpSI->GetDocEntry(0x0004, 0x1430); // Directory Record Type
      if ( DataEntry *dataEntry = dynamic_cast<DataEntry *>(d) )
      {
         v = dataEntry->GetString();
      }
      else
      {
         gdcmWarningMacro( "(0004,1430) not a DataEntry ?!?");
         continue;
      }

      // A decent DICOMDIR has much more images than series,
      // more series than studies, and so on.
      // This is the right order to perform the tests

      if ( v == "IMAGE " ) 
      {
         si = DicomDirImage::New(true); // true = empty
         if ( !AddImageToEnd( static_cast<DicomDirImage *>(si)) )
         {
            si->Delete();
            si = NULL;
            gdcmErrorMacro( "Add AddImageToEnd failed");
         }
      }
      else if ( v == "SERIES" )
      {
         si = DicomDirSerie::New(true);  // true = empty
         if ( !AddSerieToEnd( static_cast<DicomDirSerie *>(si)) )
         {
            si->Delete();
            si = NULL;
            gdcmErrorMacro( "Add AddSerieToEnd failed");
         }
      }
      else if ( v == "VISIT " )
      {
         si = DicomDirVisit::New(true);  // true = empty
         if ( !AddVisitToEnd( static_cast<DicomDirVisit *>(si)) )
         {
            si->Delete();
            si = NULL;
            gdcmErrorMacro( "Add AddVisitToEnd failed");
         }
      }
      else if ( v == "STUDY " )
      {
         si = DicomDirStudy::New(true);  // true = empty
         if ( !AddStudyToEnd( static_cast<DicomDirStudy *>(si)) )
         {
            si->Delete();
            si = NULL;
            gdcmErrorMacro( "Add AddStudyToEnd failed");
         }
      }
      else if ( v == "PATIENT " )
      {
         si = DicomDirPatient::New(true);  // true = empty
         if ( !AddPatientToEnd( static_cast<DicomDirPatient *>(si)) )
         {
            si->Delete();
            si = NULL;
            gdcmErrorMacro( "Add PatientToEnd failed");
         }
      }
      /// \todo : deal with PRIVATE (not so easy, since PRIVATE appears 
      ///                           at different levels ?!? )
      
      else if ( v == "PRIVATE " ) // for SIEMENS 'CSA Non Image'      
      {
      
         gdcmWarningMacro( " -------------------------------------------"
              << "a PRIVATE SQItem was found : " << v);
         si = DicomDirPrivate::New(true);  // true = empty
         if ( !AddPrivateToEnd( static_cast<DicomDirPrivate *>(si)) )
         {
            si->Delete();
            si = NULL;
            gdcmErrorMacro( "Add PrivateToEnd failed");
         }
      }      
      else
      {
         // It was neither a 'PATIENT', nor a 'STUDY', nor a 'SERIE',
         // nor an 'IMAGE' SQItem. Skip to next item.
         gdcmWarningMacro( " -------------------------------------------"
         << "a non PATIENT/STUDY/SERIE/IMAGE /VISIT/PRIVATE SQItem was found : "
         << v);

        // FIXME : deal with other item types !
        tmpSI=s->GetNextSQItem(); // To avoid infinite loop
        continue;
      }
      if ( si )
         si->Copy(tmpSI);

      tmpSI=s->GetNextSQItem();
   }
   ClearEntry();
}

/**
 * \brief  AddPatientToEnd 
 * @param   dd SQ Item to enqueue to the DicomPatient chained List
 */
bool DicomDir::AddPatientToEnd(DicomDirPatient *dd)
{
   Patients.push_back(dd);
   return true;
}

/**
 * \brief  AddStudyToEnd 
 * @param   dd SQ Item to enqueue to the DicomDirStudy chained List
 */
bool DicomDir::AddStudyToEnd(DicomDirStudy *dd)
{
   if ( Patients.size() > 0 )
   {
      ListDicomDirPatient::iterator itp = Patients.end();
      itp--;
      (*itp)->AddStudy(dd);
      return true;
   }
   return false;
}

/**
 * \brief  AddSerieToEnd 
 * @param   dd SQ Item to enqueue to the DicomDirSerie chained List
 */
bool DicomDir::AddSerieToEnd(DicomDirSerie *dd)
{
   if ( Patients.size() > 0 )
   {
      ListDicomDirPatient::iterator itp = Patients.end();
      itp--;

      DicomDirStudy *study = (*itp)->GetLastStudy();
      if ( study )
      {
         study->AddSerie(dd);
         return true;
      }
   }
   return false;
}

/**
 * \brief  AddVisitToEnd 
 * @param   dd SQ Item to enqueue to the DicomDirVisit chained List
 */
bool DicomDir::AddVisitToEnd(DicomDirVisit *dd)
{
   if ( Patients.size() > 0 )
   {
      ListDicomDirPatient::iterator itp = Patients.end();
      itp--;

      DicomDirStudy *study = (*itp)->GetLastStudy();
      if ( study )
      {
         study->AddVisit(dd);
         return true;
      }
   }
   return false;
}
/**
 * \brief   AddImageToEnd
 * @param   dd SQ Item to enqueue to the DicomDirImage chained List
 */
bool DicomDir::AddImageToEnd(DicomDirImage *dd)
{
   if ( Patients.size() > 0 )
   {
      ListDicomDirPatient::iterator itp = Patients.end();
      itp--;

      DicomDirStudy *study = (*itp)->GetLastStudy();
      if ( study )
      {
         DicomDirSerie *serie = study->GetLastSerie();
         if ( serie )
         {
            serie->AddImage(dd);
            return true;
         }
      }
   }
   return false;
}

/**
 * \brief   AddPrivateToEnd
 * @param   dd SQ Item to enqueue to the DicomDirPrivate chained List
 *          (checked for SIEMENS 'CSA non image')
 */
bool DicomDir::AddPrivateToEnd(DicomDirPrivate *dd)
{
   if ( Patients.size() > 0 )
   {
      ListDicomDirPatient::iterator itp = Patients.end();
      itp--;

      DicomDirStudy *study = (*itp)->GetLastStudy();
      if ( study )
      {
         DicomDirSerie *serie = study->GetLastSerie();
         if ( serie )
         {
            serie->AddPrivate(dd);
            return true;
         }
      }
   }
   return false;
}

/**
 * \brief  for each Header of the chained list, 
 *         add/update the Patient/Study/Serie/Image info 
 * @param   path path of the root directory
 * @param   list chained list of Headers
 */
void DicomDir::SetElements(std::string const &path, VectDocument const &list)
{
   ClearEntry();
   ClearPatient();

   std::string patPrevName         = "", patPrevID  = "";
   std::string studPrevInstanceUID = "", studPrevID = "";
   std::string serPrevInstanceUID  = "", serPrevID  = "";

   std::string patCurName,         patCurID;
   std::string studCurInstanceUID, studCurID;
   std::string serCurInstanceUID,  serCurID;

   bool first = true;
   for( VectDocument::const_iterator it = list.begin();
                                     it != list.end(); 
                                   ++it )
   {
      // get the current file characteristics
      patCurName         = (*it)->GetEntryString(0x0010,0x0010);
      if (patCurName == "") // to prevent further troubles when wild anonymization was performed
         patCurName = "gdcm^Patient";
      patCurID           = (*it)->GetEntryString(0x0010,0x0011);
      studCurInstanceUID = (*it)->GetEntryString(0x0020,0x000d);
      studCurID          = (*it)->GetEntryString(0x0020,0x0010);
      serCurInstanceUID  = (*it)->GetEntryString(0x0020,0x000e);
      serCurID           = (*it)->GetEntryString(0x0020,0x0011);

      if ( patCurName != patPrevName || patCurID != patPrevID || first )
      {
         SetElement(path, GDCM_DICOMDIR_PATIENT, *it);
         first = true;
      }

      // if new Study, deal with 'STUDY' Elements   
      if ( studCurInstanceUID != studPrevInstanceUID || studCurID != studPrevID 
         || first )
      {
         SetElement(path, GDCM_DICOMDIR_STUDY, *it);
         first = true;
      }

      // if new Serie, deal with 'SERIE' Elements   
      if ( serCurInstanceUID != serPrevInstanceUID || serCurID != serPrevID
         || first )
      {
         SetElement(path, GDCM_DICOMDIR_SERIE, *it);
      }
      
      // Always Deal with 'IMAGE' Elements  
      SetElement(path, GDCM_DICOMDIR_IMAGE, *it);

      patPrevName         = patCurName;
      patPrevID           = patCurID;
      studPrevInstanceUID = studCurInstanceUID;
      studPrevID          = studCurID;
      serPrevInstanceUID  = serCurInstanceUID;
      serPrevID           = serCurID;
      first = false;
   }
}

/**
 * \brief   adds to the HTable 
 *          the Entries (Dicom Elements) corresponding to the given type
 * @param   path full path file name (only used when type = GDCM_DICOMDIR_IMAGE
 * @param   type DicomDirObject type to create (GDCM_DICOMDIR_PATIENT,
 *          GDCM_DICOMDIR_STUDY, GDCM_DICOMDIR_SERIE ...)
 * @param   header Header of the current file
 */
void DicomDir::SetElement(std::string const &path, DicomDirType type,
                          Document *header)
{
   ListDicomDirElem elemList;
   ListDicomDirElem::const_iterator it;
   uint16_t tmpGr, tmpEl;
   //DictEntry *dictEntry;
   DataEntry *entry;
   std::string val;
   SQItem *si;
   switch( type )
   {
      case GDCM_DICOMDIR_IMAGE:
         elemList = Global::GetDicomDirElements()->GetDicomDirImageElements();
         si = DicomDirImage::New(true);
         if ( !AddImageToEnd(static_cast<DicomDirImage *>(si)) )
         {
            si->Delete();
            gdcmErrorMacro( "Add ImageToEnd failed");
         }
         break;
      case GDCM_DICOMDIR_SERIE:
         elemList = Global::GetDicomDirElements()->GetDicomDirSerieElements();
         si = DicomDirSerie::New(true);
         if ( !AddSerieToEnd(static_cast<DicomDirSerie *>(si)) )
         {
            si->Delete();
            gdcmErrorMacro( "Add SerieToEnd failed");
         }
         break;
      case GDCM_DICOMDIR_STUDY:
         elemList = Global::GetDicomDirElements()->GetDicomDirStudyElements();
         si = DicomDirStudy::New(true);
         if ( !AddStudyToEnd(static_cast<DicomDirStudy *>(si)) )
         {
            si->Delete();
            gdcmErrorMacro( "Add StudyToEnd failed");
         }
         break;
      case GDCM_DICOMDIR_PATIENT:
         elemList = Global::GetDicomDirElements()->GetDicomDirPatientElements();
         si = DicomDirPatient::New(true);
         if ( !AddPatientToEnd(static_cast<DicomDirPatient *>(si)) )
         {
            si->Delete();
            gdcmErrorMacro( "Add PatientToEnd failed");
         }
         break;
      case GDCM_DICOMDIR_META:  // never used ?!? --> Done within DoTheLoadingJob
         if ( MetaElems )
         {
            MetaElems->Delete();
            gdcmErrorMacro( "MetaElements already exist, they will be destroyed");
         }
         elemList = Global::GetDicomDirElements()->GetDicomDirMetaElements();
         MetaElems = DicomDirMeta::New(true);
         si = MetaElems;
         break;
      default:
         return;
   }

   // FIXME : troubles found when it's a SeqEntry

   // removed all the seems-to-be-useless stuff about Referenced Image Sequence
   // to avoid further troubles
   // imageElem 0008 1140 "" // Referenced Image Sequence
   // imageElem fffe e000 "" // 'no length' item : length to be set to 0xffffffff later
   // imageElem 0008 1150 "" // Referenced SOP Class UID    : to be set/forged later
   // imageElem 0008 1155 "" // Referenced SOP Instance UID : to be set/forged later
   // imageElem fffe e00d "" // Item delimitation : length to be set to ZERO later
 
   std::string referencedVal;
   // for all the relevant elements found in their own spot of the DicomDir.dic
   for( it = elemList.begin(); it != elemList.end(); ++it)
   {
      tmpGr     = it->Group;
      tmpEl     = it->Elem;
       
      entry     = DataEntry::New(tmpGr, tmpEl, it->VR); // dicomelements file was modified, to store VR
      entry->SetOffset(0); // just to avoid further missprinting

      if ( header )
      {
         // NULL when we Build Up (ex nihilo) a DICOMDIR
         //   or when we add the META elems
         val = header->GetEntryString(tmpGr, tmpEl); 
      }
      else
      {
         val = GDCM_UNFOUND;
      }

      if ( val == GDCM_UNFOUND) 
      {
         if ( tmpGr == 0x0004 ) // never present in File !     
         {
            switch (tmpEl)
            {
            case 0x1130: // File-set ID
               // force to the *end* File Name
               val = Util::GetName( path );
               break;
      
            case 0x1500: // Only used for image    
               if ( header->GetFileName().substr(0, path.length()) != path )
               { 
                 gdcmWarningMacro( "The base path of file name is incorrect");
                 val = header->GetFileName();
               }
               else
               { 
                 // avoid the first '/' in File name !
                 if ( header->GetFileName().c_str()[path.length()] 
                                                      == GDCM_FILESEPARATOR )
                    val = &(header->GetFileName().c_str()[path.length()+1]);
                 else  
                    val = &(header->GetFileName().c_str()[path.length()]);   
               }
               break;
    
             case 0x1510:  // Referenced SOP Class UID in File
               referencedVal = header->GetEntryString(0x0008, 0x0016);
               // FIXME : probabely something to check
               val = referencedVal;
               break;
       
             case 0x1511: // Referenced SOP Instance UID in File
               referencedVal = header->GetEntryString(0x0008, 0x0018);
               // FIXME : probabely something to check
               val = referencedVal;
               break;
    
            case 0x1512: // Referenced Transfer Syntax UID in File
               referencedVal = header->GetEntryString(0x0002, 0x0010);
               // FIXME : probabely something to check
               val = referencedVal;
               break;
    
            default :
               val = it->Value;   
            } 
         }
         else
         {
            // If the entry is not found in the Header, don't write its 'value' in the DICOMDIR !
            entry->Delete();
            continue;
          }
      }
      else
      {
         if ( header->GetEntryLength(tmpGr,tmpEl) == 0 )
         {
            val = it->Value;
            // Don't polute the DICOMDIR with empty fields
            if (val == "")
            {
               entry->Delete();
               continue;
            }  
         }    
      }

/* FIX later the pb of creating the 'Implementation Version Name'!

      if (val == GDCM_UNFOUND)
         val = "";

      if ( tmpGr == 0x0002 && tmpEl == 0x0013)
      { 
         // 'Implementation Version Name'
         std::string val = "GDCM ";
         val += Util::GetVersion();
      }
*/ 

      entry->SetString( val ); // troubles expected when vr=SQ ...

      if ( type == GDCM_DICOMDIR_META ) // fusible : should never print !
      {
         gdcmDebugMacro("GDCM_DICOMDIR_META ?!? should never print that");
      }
      
      si->AddEntry(entry);
      entry->Delete();
   }
}

/**
 * \brief   Move the content of the source SQItem to the destination SQItem
 *          Only DocEntry's are moved
 * @param dst destination SQItem
 * @param src source SQItem
 */
void DicomDir::MoveSQItem(DocEntrySet *dst, DocEntrySet *src)
{ 
   DocEntry *entry;
// todo : rewrite the whole stuff, without using RemoveEntry an AddEntry,
//        to save time
   entry = src->GetFirstEntry();
   while(entry)
   {
      dst->AddEntry(entry);  // use it, *before* removing it!
      src->RemoveEntry(entry);
      // we destroyed -> the current iterator is not longer valid
      entry = src->GetFirstEntry();
   }
}

/**
 * \brief   compares two files
 */
bool DicomDir::HeaderLessThan(Document *header1, Document *header2)
{
   return *header1 < *header2;
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief  Canonical Printer 
 * @param   os ostream we want to print in
 * @param indent Indentation string to be prepended during printing
 */
void DicomDir::Print(std::ostream &os, std::string const & )
{
   if ( MetaElems )
   {
      MetaElems->SetPrintLevel(PrintLevel);
      MetaElems->Print(os);   
   }   
   for(ListDicomDirPatient::iterator cc  = Patients.begin();
                                     cc != Patients.end();
                                   ++cc)
   {
     (*cc)->SetPrintLevel(PrintLevel);
     (*cc)->Print(os);
   }
}

//-----------------------------------------------------------------------------
} // end namespace gdcm
