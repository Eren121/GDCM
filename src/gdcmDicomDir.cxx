/*=========================================================================
  
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/16 04:50:41 $
  Version:   $Revision: 1.102 $
  
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
  
=========================================================================*/

#include "gdcmDicomDir.h"
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirImage.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirMeta.h"
#include "gdcmDicomDirElement.h"
#include "gdcmDirList.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"
#include "gdcmGlobal.h"
#include "gdcmHeader.h"
#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmValEntry.h"

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

namespace gdcm 
{

//-----------------------------------------------------------------------------
//  For full DICOMDIR description, see:
//  PS 3.3-2003, pages 731-750
//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup DicomDir
 * \brief   Constructor : creates an empty DicomDir
 */
DicomDir::DicomDir()
   :Document( )
{ 
   Initialize();  // sets all private fields to NULL
   MetaElems = NewMeta();
}

/**
 * \brief Constructor Parses recursively the directory and creates the DicomDir
 *        or uses an already built DICOMDIR, depending on 'parseDir' value.
 * @param fileName        name 
 *                      - of the root directory (parseDir = true)
 *                      - of the DICOMDIR       (parseDir = false)
 * @param parseDir boolean
 *                      - true if user passed an entry point 
 *                        and wants to explore recursively the directories
 *                      - false if user passed an already built DICOMDIR file
 *                        and wants to use it 
 */
DicomDir::DicomDir(std::string const &fileName, bool parseDir ):
   Document( fileName )
{
   // At this step, Document constructor is already executed,
   // whatever user passed (a root directory or a DICOMDIR)
   // and whatever the value of parseDir was.
   // (nothing is cheked in Document constructor, to avoid overhead)

   Initialize();  // sets all private fields to NULL

   // if user passed a root directory, sure we didn't get anything
   if ( TagHT.begin() == TagHT.end() ) // when user passed a Directory to parse
   {
      if (!parseDir)
         gdcmVerboseMacro( "Entry HT empty for file: "<<fileName);

   // Only if user passed a root directory
   // ------------------------------------
      if ( fileName == "." )
      {
         // user passed '.' as Name
         // we get current directory name
         char dummy[1000];
         getcwd(dummy, (size_t)1000);
         SetFileName( dummy ); // will be converted into a string
      }

      if ( parseDir ) // user asked for a recursive parsing of a root directory
      {
         MetaElems = NewMeta();

         gdcmVerboseMacro( "Parse directory and create the DicomDir");
         ParseDirectory();
      }
      else
      {
         /// \todo if parseDir == false, it should be tagged as an error
         // NO ! user may just call ParseDirectory() *after* constructor
      }
   }
   // Only if user passed a DICOMDIR
   // ------------------------------
   else 
   {
      // Directory record sequence
      DocEntry *e = GetDocEntry(0x0004, 0x1220);
      if ( !e )
      {
         gdcmVerboseMacro( "NO 'Directory record sequence' (0x0004,0x1220)"
                          << " in file " << fileName);
         /// \todo FIXME : what do we do when the parsed file IS NOT a
         ///       DICOMDIR file ?         
      }
      else
         CreateDicomDir();
   }
}

/**
 * \brief  Canonical destructor 
 */
DicomDir::~DicomDir() 
{
   SetStartMethod(NULL);
   SetProgressMethod(NULL);
   SetEndMethod(NULL);

   for(ListDicomDirPatient::iterator cc = Patients.begin();
                                     cc!= Patients.end();
                                   ++cc)
   {
      delete *cc;
   }
   if ( MetaElems )
   {
      delete MetaElems;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief  Canonical Printer 
 */
void DicomDir::Print(std::ostream &os, std::string const & )
{
   if( MetaElems )
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
// Public
/**
 * \brief  This predicate, based on hopefully reasonable heuristics,
 *         decides whether or not the current header was properly parsed
 *         and contains the mandatory information for being considered as
 *         a well formed and usable DicomDir.
 * @return true when Document is the one of a reasonable DicomDir,
 *         false otherwise. 
 */
bool DicomDir::IsReadable()
{
   if( Filetype == Unknown)
   {
      gdcmVerboseMacro( "Wrong filetype");
      return false;
   }
   if( !MetaElems )
   {
      gdcmVerboseMacro( "Meta Elements missing in DicomDir");
      return false;
   }
   if( Patients.size() <= 0 )
   {
      gdcmVerboseMacro( "NO Patient in DicomDir");
      return false;
   }

   return true;
}

/**
 * \brief Sets all fields to NULL
 */

void DicomDir::Initialize()
{
   StartMethod             = NULL;
   ProgressMethod          = NULL;
   EndMethod               = NULL;
   StartMethodArgDelete    = NULL;
   ProgressMethodArgDelete = NULL;
   EndMethodArgDelete      = NULL;
   StartArg                = NULL;
   ProgressArg             = NULL;
   EndArg                  = NULL;

   Progress = 0.0;
   Abort = false;

   MetaElems = NULL;   
}


/**
 * \ingroup DicomDir
 * \brief  fills the whole structure, starting from a root Directory
 */
void DicomDir::ParseDirectory()
{
   CreateDicomDirChainedList( GetFileName() );
   CreateDicomDir();
}

/**
 * \ingroup DicomDir
 * \brief   Set the start method to call when the parsing of the
 *          directory starts.
 * @param   method Method to call
 * @param   arg    Argument to pass to the method
 * @param   argDelete    Argument 
 * \warning In python : the arg parameter isn't considered
 */
void DicomDir::SetStartMethod( DicomDir::Method *method, void *arg, 
                               DicomDir::Method *argDelete )
{
   if( StartArg && StartMethodArgDelete )
   {
      StartMethodArgDelete( StartArg );
   }

   StartMethod          = method;
   StartArg             = arg;
   StartMethodArgDelete = argDelete;
}

/**
 * \ingroup DicomDir
 * \brief   Set the method to delete the argument
 *          The argument is destroyed when the method is changed or when the
 *          class is destroyed
 * @param   method Method to call to delete the argument
 */
void DicomDir::SetStartMethodArgDelete( DicomDir::Method *method ) 
{
   StartMethodArgDelete = method;
}

/**
 * \ingroup DicomDir
 * \brief   Set the progress method to call when the parsing of the
 *          directory progress
 * @param   method Method to call
 * @param   arg    Argument to pass to the method
 * @param   argDelete    Argument  
 * \warning In python : the arg parameter isn't considered
 */
void DicomDir::SetProgressMethod( DicomDir::Method *method, void *arg, 
                                  DicomDir::Method *argDelete )
{
   if( ProgressArg && ProgressMethodArgDelete )
   {
      ProgressMethodArgDelete( ProgressArg );
   }

   ProgressMethod          = method;
   ProgressArg             = arg;
   ProgressMethodArgDelete = argDelete;
}

/**
 * \ingroup DicomDir
 * \brief   Set the method to delete the argument
 *          The argument is destroyed when the method is changed or when the 
 *          class is destroyed          
 * @param   method Method to call to delete the argument
 */
void DicomDir::SetProgressMethodArgDelete( DicomDir::Method *method )
{
   ProgressMethodArgDelete = method;
}

/**
 * \ingroup DicomDir
 * \brief   Set the end method to call when the parsing of the directory ends
 * @param   method Method to call
 * @param   arg    Argument to pass to the method
 * @param   argDelete    Argument 
 * \warning In python : the arg parameter isn't considered
 */
void DicomDir::SetEndMethod( DicomDir::Method *method, void *arg, 
                             DicomDir::Method *argDelete )
{
   if( EndArg && EndMethodArgDelete )
   {
      EndMethodArgDelete( EndArg );
   }

   EndMethod          = method;
   EndArg             = arg;
   EndMethodArgDelete = argDelete;
}

/**
 * \ingroup DicomDir
 * \brief   Set the method to delete the argument
 *          The argument is destroyed when the method is changed or when
 *          the class is destroyed
 * @param   method Method to call to delete the argument
 */
void DicomDir::SetEndMethodArgDelete( DicomDir::Method *method )
{
   EndMethodArgDelete = method;
}

/**
 * \ingroup DicomDir
 * \brief    writes on disc a DICOMDIR
 * \ warning does NOT add the missing elements in the header :
 *           it's up to the user doing it !
 * \todo : to be re-written using the DICOMDIR tree-like structure
 *         *not* the chained list
 *         (does NOT exist if the DICOMDIR is user-forged !)
 * @param  fileName file to be written to 
 * @return false only when fail to open
 */
 
bool DicomDir::WriteDicomDir(std::string const &fileName) 
{  
   int i;
   uint16_t sq[4] = { 0x0004, 0x1220, 0xffff, 0xffff };
   uint16_t sqt[4]= { 0xfffe, 0xe0dd, 0xffff, 0xffff };

   std::ofstream *fp = new std::ofstream(fileName.c_str(),  
                                         std::ios::out | std::ios::binary);
   if( !fp ) 
   {
      gdcmVerboseMacro("Failed to open(write) File: " << fileName.c_str());
      return false;
   }

   char filePreamble[128];
   memset(filePreamble, 0, 128);
   fp->write(filePreamble, 128); //FIXME
   binary_write( *fp, "DICM");
 
   DicomDirMeta *ptrMeta = GetDicomDirMeta();
   ptrMeta->WriteContent(fp, ExplicitVR);
   
   // force writing 0004|1220 [SQ ], that CANNOT exist within DicomDirMeta
   for(i=0;i<4;++i)
   {
      binary_write(*fp, sq[i]);
   }
        
   for(ListDicomDirPatient::iterator cc  = Patients.begin();
                                     cc != Patients.end();
                                   ++cc )
   {
      (*cc)->WriteContent( fp, ExplicitVR );
   }
   
   // force writing Sequence Delimitation Item
   for(i=0;i<4;++i)
   {
      binary_write(*fp, sqt[i]);  // fffe e0dd ffff ffff 
   }

   fp->close();
   delete fp;

   return true;
}

//-----------------------------------------------------------------------------
// Protected

/**
 * \ingroup DicomDir
 * \brief create a Document-like chained list from a root Directory 
 * @param path entry point of the tree-like structure
 */
void DicomDir::CreateDicomDirChainedList(std::string const & path)
{
   CallStartMethod();
   DirList dirList(path,1); // gets recursively the file list
   unsigned int count = 0;
   VectDocument list;
   Header *header;

   DirListType fileList = dirList.GetFilenames();

   for( DirListType::iterator it  = fileList.begin();
                              it != fileList.end();
                              ++it )
   {
      Progress = (float)(count+1)/(float)fileList.size();
      CallProgressMethod();
      if( Abort )
      {
         break;
      }

      header = new Header( it->c_str() );
      if( !header )
      {
         gdcmVerboseMacro( "Failure in new Header " << it->c_str() );
         continue;
      }
      
      if( header->IsReadable() )
      {
         // Add the file header to the chained list:
         list.push_back(header);
         gdcmVerboseMacro( "Readable " << it->c_str() );
       }
       else
       {
          delete header;
       }
       count++;
   }
   // sorts Patient/Study/Serie/
   std::sort(list.begin(), list.end(), DicomDir::HeaderLessThan );
   
   std::string tmp = dirList.GetDirName();      
   //for each Header of the chained list, add/update the Patient/Study/Serie/Image info
   SetElements(tmp, list);
   CallEndMethod();

   for(VectDocument::iterator itDoc=list.begin();
       itDoc!=list.end();
       ++itDoc)
   {
      delete dynamic_cast<Header *>(*itDoc);
   }
}

/**
 * \ingroup DicomDir
 * \brief   adds *the* Meta to a partially created DICOMDIR
 */
  
DicomDirMeta * DicomDir::NewMeta()
{
   DicomDirMeta *m = new DicomDirMeta();
  
   if ( TagHT.begin() != TagHT.end() ) // after Document Parsing
   { 
      TagDocEntryHT::iterator lastOneButSequence = TagHT.end();
      lastOneButSequence --;
      // ALL the 'out of Sequence' Tags belong to Meta Elems
      // (we skip 0004|1220 [Directory record sequence] )
      for ( TagDocEntryHT::iterator cc  = TagHT.begin(); 
                                    cc != lastOneButSequence;
                                   ++cc)
      {
         m->AddEntry( cc->second );
      }
   }
   else  // after root directory parsing
   {
      ListDicomDirMetaElem const &elemList = 
         Global::GetDicomDirElements()->GetDicomDirMetaElements();
      m->FillObject(elemList);
   }
   m->SetSQItemNumber(0); // To avoid further missprinting
   return m;  
}

/**
 * \brief   adds a new Patient (with the basic elements) to a partially created DICOMDIR
 */
DicomDirPatient *DicomDir::NewPatient()
{
   ListDicomDirPatientElem::const_iterator it;
   uint16_t tmpGr,tmpEl;
   DictEntry *dictEntry;
   ValEntry *entry;

   ListDicomDirPatientElem const & elemList =
      Global::GetDicomDirElements()->GetDicomDirPatientElements(); 
   DicomDirPatient *p = new DicomDirPatient();

   // for all the DicomDirPatient Elements      
   for( it = elemList.begin(); it != elemList.end(); ++it ) 
   {
      tmpGr     = it->Group;
      tmpEl     = it->Elem;
      dictEntry = GetPubDict()->GetDictEntry(tmpGr, tmpEl);
      entry     = new ValEntry( dictEntry );
      entry->SetOffset(0); // just to avoid further missprinting
      entry->SetValue( it->Value );

      p->AddEntry( entry );
   }

   Patients.push_front( p );

   return p;
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
   ListDicomDirElem elemList; //FIXME this is going to be a by copy operation
   ListDicomDirElem::const_iterator it;
   uint16_t tmpGr, tmpEl;
   DictEntry *dictEntry;
   ValEntry *entry;
   std::string val;
   SQItem *si;

   switch( type )
   {
      case GDCM_DICOMDIR_IMAGE:
         elemList = Global::GetDicomDirElements()->GetDicomDirImageElements();
         si = new DicomDirImage();
         if( !AddDicomDirImageToEnd(static_cast<DicomDirImage *>(si)) )
         {
            gdcmVerboseMacro( "Add DicomDirImageToEnd failed");
         }
         break;
      case GDCM_DICOMDIR_SERIE:
         elemList = Global::GetDicomDirElements()->GetDicomDirSerieElements();
         si = new DicomDirSerie();
         if( !AddDicomDirSerieToEnd(static_cast<DicomDirSerie *>(si)) )
         {
            gdcmVerboseMacro( "Add DicomDirSerieToEnd failed");
         }
         break;
      case GDCM_DICOMDIR_STUDY:
         elemList = Global::GetDicomDirElements()->GetDicomDirStudyElements();
         si = new DicomDirStudy();
         if( !AddDicomDirStudyToEnd(static_cast<DicomDirStudy *>(si)) )
         {
            gdcmVerboseMacro( "Add DicomDirStudyToEnd failed");
         }
         break;
      case GDCM_DICOMDIR_PATIENT:
         elemList = Global::GetDicomDirElements()->GetDicomDirPatientElements();
         si = new DicomDirPatient();
         if( !AddDicomDirPatientToEnd(static_cast<DicomDirPatient *>(si)) )
         {
            gdcmVerboseMacro( "Add DicomDirPatientToEnd failed");
         }
         break;
      case GDCM_DICOMDIR_META:
         elemList = Global::GetDicomDirElements()->GetDicomDirMetaElements();
         si = new DicomDirMeta();
         if( MetaElems )
         {
            gdcmVerboseMacro( "MetaElements already exist, they will be destroyed");
            delete MetaElems;
         }
         MetaElems = static_cast<DicomDirMeta *>(si);
         break;
      default:
         return;
   }
   // removed all the seems-to-be-useless stuff about Referenced Image Sequence
   // to avoid further troubles
   // imageElem 0008 1140 "" // Referenced Image Sequence
   // imageElem fffe e000 "" // 'no length' item : length to be set to 0xffffffff later
   // imageElem 0008 1150 "" // Referenced SOP Class UID    : to be set/forged later
   // imageElem 0008 1155 "" // Referenced SOP Instance UID : to be set/forged later
   // imageElem fffe e00d "" // Item delimitation : length to be set to ZERO later
   // for all the relevant elements found in their own spot of the DicomDir.dic
   // FIXME : troubles found when it's a SeqEntry

   for( it = elemList.begin(); it != elemList.end(); ++it)
   {
      tmpGr     = it->Group;
      tmpEl     = it->Elem;
      dictEntry = GetPubDict()->GetDictEntry(tmpGr, tmpEl);

      entry     = new ValEntry( dictEntry ); // Be sure it's never a BinEntry !

      entry->SetOffset(0); // just to avoid further missprinting

      if( header )
      {
         // NULL when we Build Up (ex nihilo) a DICOMDIR
         //   or when we add the META elems
         val = header->GetEntry(tmpGr, tmpEl);
      }
      else
      {
         val = GDCM_UNFOUND;
      }

      if( val == GDCM_UNFOUND) 
      {
         if( tmpGr == 0x0004 && tmpEl == 0x1130 ) // File-set ID
         {
           // force to the *end* File Name
           val = Util::GetName( path );
         }
         else if( tmpGr == 0x0004 && tmpEl == 0x1500 ) // Only used for image
         {
            if( header->GetFileName().substr(0, path.length()) != path )
            {
               gdcmVerboseMacro( "The base path of file name is incorrect");
               val = header->GetFileName();
            }
            else
            {
               val = &(header->GetFileName().c_str()[path.length()]);
            }
         }
         else
         {
            val = it->Value;
         }
      }
      else
      {
         if ( header->GetEntryLength(tmpGr,tmpEl) == 0 )
            val = it->Value;
      }

      entry->SetValue( val ); // troubles expected when vr=SQ ...

      if ( type == GDCM_DICOMDIR_META ) // fusible : should never print !
      {
         std::cout << "GDCM_DICOMDIR_META ?!? should never print that" 
                   << std::endl;
      }
      si->AddEntry(entry);
   }
}

//-----------------------------------------------------------------------------
/**
 * \brief   CallStartMethod
 */
void DicomDir::CallStartMethod()
{
   Progress = 0.0f;
   Abort    = false;
   if( StartMethod )
   {
      StartMethod( StartArg );
   }
}

//-----------------------------------------------------------------------------
/**
 * \ingroup DicomDir
 * \brief   CallProgressMethod
 */
void DicomDir::CallProgressMethod()
{
   if( ProgressMethod )
   {
      ProgressMethod( ProgressArg );
   }
}

//-----------------------------------------------------------------------------
/**
 * \ingroup DicomDir
 * \brief   CallEndMethod
 */
void DicomDir::CallEndMethod()
{
   Progress = 1.0f;
   if( EndMethod )
   {
      EndMethod( EndArg );
   }
}

//-----------------------------------------------------------------------------
// Private
/**
 * \ingroup DicomDir
 * \brief create a 'DicomDir' from a DICOMDIR Header 
 */
void DicomDir::CreateDicomDir()
{
   // The list is parsed. 
   //  When a DicomDir tag ("PATIENT", "STUDY", "SERIE", "IMAGE") is found :
   //  1 - we save the beginning iterator
   //  2 - we continue to parse
   //  3 - we find an other tag
   //       + we create the object for the precedent tag
   //       + loop to 1 -

   // Directory record sequence
   DocEntry *e = GetDocEntry(0x0004, 0x1220);
   if ( !e )
   {
      gdcmVerboseMacro( "NO Directory record sequence (0x0004,0x1220)");
      /// \todo FIXME: what to do when the parsed file IS NOT a DICOMDIR file ? 
      return;         
   }
   
   SeqEntry *s = dynamic_cast<SeqEntry *>(e);
   if ( !s )
   {
      gdcmVerboseMacro( "No SeqEntry present");
      // useless : (0x0004,0x1220) IS a Sequence !
      return;
   }

   MetaElems = NewMeta();

   ListSQItem listItems = s->GetSQItems();
   
   DocEntry *d;
   std::string v;
   SQItem *si;
   for( ListSQItem::iterator i = listItems.begin(); 
                             i !=listItems.end(); ++i ) 
   {
      d = (*i)->GetDocEntry(0x0004, 0x1430); // Directory Record Type
      if ( ValEntry* valEntry = dynamic_cast<ValEntry *>(d) )
      {
         v = valEntry->GetValue();
      }
      else
      {
         gdcmVerboseMacro( "Not a ValEntry.");
         continue;
      }

      if( v == "PATIENT " )
      {
         si = new DicomDirPatient();
         AddDicomDirPatientToEnd( static_cast<DicomDirPatient *>(si) );
      }
      else if( v == "STUDY " )
      {
         si = new DicomDirStudy();
         AddDicomDirStudyToEnd( static_cast<DicomDirStudy *>(si) );
      }
      else if( v == "SERIES" )
      {
         si = new DicomDirSerie();
         AddDicomDirSerieToEnd( static_cast<DicomDirSerie *>(si) );
      }
      else if( v == "IMAGE " ) 
      {
         si = new DicomDirImage();
         AddDicomDirImageToEnd( static_cast<DicomDirImage *>(si) );
      }
      else
      {
         // It was not a 'PATIENT', nor a 'STUDY', nor a 'SERIE',
         // neither an 'IMAGE' SQItem. Skip to next item.
         continue;
      }
      MoveSQItem(si,*i);
   }
   TagHT.clear();
}

/**
 * \ingroup DicomDir
 * \brief Well ... there is only one occurence  
 */
bool DicomDir::AddDicomDirMeta()
{
   if( MetaElems )
   {
      delete MetaElems;
   }
   MetaElems = new DicomDirMeta();
   return true;
}

/**
 * \ingroup DicomDir
 * \brief  AddDicomDirPatientToEnd 
 * @param   dd SQ Item to enqueue to the DicomPatient chained List
 */
bool DicomDir::AddDicomDirPatientToEnd(DicomDirPatient *dd)
{
   Patients.push_back(dd);
   return true;
}

/**
 * \ingroup DicomDir
 * \brief  AddDicomDirStudyToEnd 
 * @param   dd SQ Item to enqueue to the DicomDirStudy chained List
 */
bool DicomDir::AddDicomDirStudyToEnd(DicomDirStudy *dd)
{
   if( Patients.size() > 0 )
   {
      ListDicomDirPatient::iterator itp = Patients.end();
      itp--;
      (*itp)->AddDicomDirStudy(dd);
      return true;
   }
   return false;
}

/**
 * \ingroup DicomDir
 * \brief  AddDicomDirSerieToEnd 
 * @param   dd SQ Item to enqueue to the DicomDirSerie chained List
 */
bool DicomDir::AddDicomDirSerieToEnd(DicomDirSerie *dd)
{
   if( Patients.size() > 0 )
   {
      ListDicomDirPatient::iterator itp = Patients.end();
      itp--;

      if( (*itp)->GetDicomDirStudies().size() > 0 )
      {
         ListDicomDirStudy::const_iterator itst = 
            (*itp)->GetDicomDirStudies().end();
         itst--;
         (*itst)->AddDicomDirSerie(dd);
         return true;
      }
   }
   return false;
}

/**
 * \ingroup DicomDir
 * \brief   AddDicomDirImageToEnd
 * @param   dd SQ Item to enqueue to the DicomDirImage chained List
 */
bool DicomDir::AddDicomDirImageToEnd(DicomDirImage *dd)
{
   if( Patients.size() > 0 )
   {
      ListDicomDirPatient::iterator itp = Patients.end();
      itp--;

      if( (*itp)->GetDicomDirStudies().size() > 0 )
      {
         ListDicomDirStudy::const_iterator itst = 
            (*itp)->GetDicomDirStudies().end();
         itst--;

         if( (*itst)->GetDicomDirSeries().size() > 0 )
         {
            ListDicomDirSerie::const_iterator its = (*itst)->GetDicomDirSeries().end();
            its--;
            (*its)->AddDicomDirImage(dd);
            return true;
         }
      }
   }
   return false;
}

/**
 * \ingroup DicomDir
 * \brief  for each Header of the chained list, add/update the Patient/Study/Serie/Image info 
 * @param   path path of the root directory
 * @param   list chained list of Headers
 */
void DicomDir::SetElements(std::string const & path, VectDocument const &list)
{
   TagHT.clear();
   Patients.clear();

   std::string patPrevName         = "", patPrevID  = "";
   std::string studPrevInstanceUID = "", studPrevID = "";
   std::string serPrevInstanceUID  = "", serPrevID  = "";

   std::string patCurName,         patCurID;
   std::string studCurInstanceUID, studCurID;
   std::string serCurInstanceUID,  serCurID;

   bool first = true;
   for( VectDocument::const_iterator it = list.begin();
                                     it != list.end(); ++it )
   {
      // get the current file characteristics
      patCurName         = (*it)->GetEntry(0x0010,0x0010);
      patCurID           = (*it)->GetEntry(0x0010,0x0011);
      studCurInstanceUID = (*it)->GetEntry(0x0020,0x000d);
      studCurID          = (*it)->GetEntry(0x0020,0x0010);
      serCurInstanceUID  = (*it)->GetEntry(0x0020,0x000e);
      serCurID           = (*it)->GetEntry(0x0020,0x0011);

      if( patCurName != patPrevName || patCurID != patPrevID || first )
      {
         SetElement(path, GDCM_DICOMDIR_PATIENT, *it);
         first = true;
      }

      // if new Study Deal with 'STUDY' Elements   
      if( studCurInstanceUID != studPrevInstanceUID || studCurID != studPrevID 
         || first )
      {
         SetElement(path, GDCM_DICOMDIR_STUDY, *it);
         first = true;
      }

      // if new Serie Deal with 'SERIE' Elements   
      if( serCurInstanceUID != serPrevInstanceUID || serCurID != serPrevID
         || first )
      {
         SetElement(path, GDCM_DICOMDIR_SERIE, *it);
         first = true;
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
 * \ingroup DicomDir
 * \brief   Move the content of the src SQItem to the dst SQItem
 *          Only DocEntry's are moved
 * 
 */
void DicomDir::MoveSQItem(SQItem *dst,SQItem *src)
{
   DocEntry *entry;

   src->Initialize();
   entry = src->GetNextEntry();
   while(entry)
   {
      src->RemoveEntryNoDestroy(entry);
      dst->AddEntry(entry);

      src->Initialize();
      entry = src->GetNextEntry();
   }
}

/**
 * \ingroup DicomDir
 * \brief   compares two dgcmHeaders
 */
bool DicomDir::HeaderLessThan(Document *header1, Document *header2)
{
   return *header1 < *header2;
}
} // end namespace gdcm

//-----------------------------------------------------------------------------
