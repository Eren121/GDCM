/*=========================================================================
  
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2004/08/27 15:48:44 $
  Version:   $Revision: 1.65 $
  
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
  
=========================================================================*/

#include <string>
#include <algorithm>
#include <sys/types.h>

#ifdef _MSC_VER 
   #include <direct.h>
#else
   #include <unistd.h>
#endif

#include "gdcmDicomDir.h"
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirImage.h"
#include "gdcmDirList.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"
#include "gdcmGlobal.h"
#include "gdcmHeader.h"
#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmValEntry.h"

//-----------------------------------------------------------------------------
//  For full DICOMDIR description, see:
//  PS 3.3-2003, pages 731-750
//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup gdcmDicomDir
 * \brief   Constructor : creates an empty gdcmDicomDir
 */
gdcmDicomDir::gdcmDicomDir()
   :gdcmDocument( )
{ 
   Initialize();  // sets all private fields to NULL

   std::string pathBidon = "Bidon"; // Sorry, NULL not allowed ...
   SetElement(pathBidon, GDCM_DICOMDIR_META, NULL); // Set the META elements
   AddDicomDirMeta();
}

/**
 * \brief Constructor Parses recursively the directory and creates the DicomDir
 *        or uses an already built DICOMDIR, depending on 'parseDir' value.
 * @param FileName        name 
 *                      - of the root directory (parseDir = true)
 *                      - of the DICOMDIR       (parseDir = false)
 * @param parseDir boolean
 *                      - true if user passed an entry point 
 *                        and wants to explore recursively the directories
 *                      - false if user passed an already built DICOMDIR file
 *                        and wants to use it 
 */
gdcmDicomDir::gdcmDicomDir(std::string const & fileName, bool parseDir ):
   gdcmDocument( fileName )
{
   // que l'on ai passe un root directory ou un DICOMDIR
   // et quelle que soit la valeur de parseDir,
   // on a deja lance gdcmDocument 
   Initialize();  // sets all private fields to NULL

   // gdcmDocument already executed
   // if user passed a root directory, sure we didn't get anything

   if ( TagHT.begin() == TagHT.end() ) // when user passed a Directory to parse
   {
      dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : entry HT empty");

      if ( fileName.size() == 1 && fileName[0] == '.' )
      {
         // user passed '.' as Name
         // we get current directory name
         char* dummy = new char[1000];
         getcwd(dummy, (size_t)1000);
         SetFileName( dummy ); // will be converted into a string
         delete[] dummy;       // no longer needed   
      }

      if ( parseDir ) // user asked for a recursive parsing of a root directory
      {
         metaElems = NewMeta();

         dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : Parse directory"
                        " and create the DicomDir");
         ParseDirectory();
      }
      else
      {
         /// \todo if parseDir == false, it should be tagged as an error
         // NON ! il suffit d'appeler ParseDirectory() 
         // apres le constructeur
      }
   }
   else // Only if user passed a DICOMDIR
   {
      // Directory record sequence
      gdcmDocEntry *e = GetDocEntryByNumber(0x0004, 0x1220);
      if ( !e )
      {
         dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : NO Directory record"
                        " sequence (0x0004,0x1220)");
         /// \todo FIXME : what do we do when the parsed file IS NOT a
         ///       DICOMDIR file ?         
      }
      CreateDicomDir();
   }
}

/**
 * \brief  Canonical destructor 
 */
gdcmDicomDir::~gdcmDicomDir() 
{
   SetStartMethod(NULL);
   SetProgressMethod(NULL);
   SetEndMethod(NULL);
   for(ListDicomDirPatient::iterator cc = patients.begin();
                                     cc!= patients.end();
                                   ++cc)
   {
      delete *cc;
   }
   if ( metaElems )
   {
      delete metaElems;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief  Canonical Printer 
 */
void gdcmDicomDir::Print(std::ostream &os)
{
   if(metaElems)
   {
      metaElems->SetPrintLevel(PrintLevel);
      metaElems->Print(os);   
   }   
   for(ListDicomDirPatient::iterator cc  = patients.begin();
                                     cc != patients.end();
                                   ++cc)
   {
     (*cc)->SetPrintLevel( PrintLevel );
     (*cc)->Print( os );
   }
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief  This predicate, based on hopefully reasonable heuristics,
 *         decides whether or not the current header was properly parsed
 *         and contains the mandatory information for being considered as
 *         a well formed and usable DicomDir.
 * @return true when gdcmDocument is the one of a reasonable DicomDir,
 *         false otherwise. 
 */
bool gdcmDicomDir::IsReadable()
{
   if( !gdcmDocument::IsReadable() )
   {
      return false;
   }
   if( !metaElems )
   {
      return false;
   }
   if( patients.size() <= 0 )
   {
      return false;
   }

   return true;
}

/**
 * \brief Sets all fields to NULL
 */

void gdcmDicomDir::Initialize()
{
   startMethod             = NULL;
   progressMethod          = NULL;
   endMethod               = NULL;
   startMethodArgDelete    = NULL;
   progressMethodArgDelete = NULL;
   endMethodArgDelete      = NULL;
   startArg                = NULL;
   progressArg             = NULL;
   endArg                  = NULL;

   progress = 0.0;
   abort = false;

   metaElems = 0;   
}


/**
 * \ingroup gdcmDicomDir
 * \brief  fills the whole structure, starting from a root Directory
 */
void gdcmDicomDir::ParseDirectory()
{
   CreateDicomDirChainedList( GetFileName() );
   CreateDicomDir();
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Set the start method to call when the parsing of the directory starts
 * @param   method Method to call
 * @param   arg    Argument to pass to the method
 * @param   argDelete    Argument 
 * \warning In python : the arg parameter isn't considered
 */
void gdcmDicomDir::SetStartMethod(gdcmMethod *method, void *arg, 
                                  gdcmMethod *argDelete )
{
   if( startArg && startMethodArgDelete )
   {
      startMethodArgDelete( startArg );
   }

   startMethod          = method;
   startArg             = arg;
   startMethodArgDelete = argDelete;
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Set the method to delete the argument
 *          The argument is destroyed when the method is changed or when the
 *          class is destroyed
 * @param   method Method to call to delete the argument
 */
void gdcmDicomDir::SetStartMethodArgDelete(gdcmMethod *method) 
{
   startMethodArgDelete = method;
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Set the progress method to call when the parsing of the directory progress
 * @param   method Method to call
 * @param   arg    Argument to pass to the method
 * @param   argDelete    Argument  
 * \warning In python : the arg parameter isn't considered
 */
void gdcmDicomDir::SetProgressMethod(gdcmMethod *method, void *arg, 
                                     gdcmMethod *argDelete )
{
   if( progressArg && progressMethodArgDelete )
   {
      progressMethodArgDelete( progressArg );
   }

   progressMethod          = method;
   progressArg             = arg;
   progressMethodArgDelete = argDelete;
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Set the method to delete the argument
 *          The argument is destroyed when the method is changed or when the 
 *          class is destroyed          
 * @param   method Method to call to delete the argument
 */
void gdcmDicomDir::SetProgressMethodArgDelete(gdcmMethod *method)
{
   progressMethodArgDelete = method;
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Set the end method to call when the parsing of the directory ends
 * @param   method Method to call
 * @param   arg    Argument to pass to the method
 * @param   argDelete    Argument 
 * \warning In python : the arg parameter isn't considered
 */
void gdcmDicomDir::SetEndMethod(gdcmMethod *method, void *arg, 
                                gdcmMethod *argDelete )
{
   if( endArg && endMethodArgDelete )
   {
      endMethodArgDelete( endArg );
   }

   endMethod          = method;
   endArg             = arg;
   endMethodArgDelete = argDelete;
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Set the method to delete the argument
 *          The argument is destroyed when the method is changed or when the class
 *          is destroyed
 * @param   method Method to call to delete the argument
 */
void gdcmDicomDir::SetEndMethodArgDelete(gdcmMethod *method)
{
   endMethodArgDelete = method;
}

/**
 * \ingroup gdcmDicomDir
 * \brief   writes on disc a DICOMDIR
 * \ warning does NOT add the missing elements in the header :
 *           it's up to the user doing it !
 * \todo : to be re-written using the DICOMDIR tree-like structure
 *         *not* the chained list
 *         (does NOT exist if the DICOMDIR is user-forged !)
 * @param  fileName file to be written to 
 * @return false only when fail to open
 */
 
bool gdcmDicomDir::Write(std::string const & fileName) 
{  
   uint16_t sq[5] = { 0x0004, 0x1220, 0x0000, 0xffff, 0xffff };
   uint16_t sqt[4]= { 0xfffe, 0xe0dd, 0xffff, 0xffff };

   FILE * fp = fopen(fileName.c_str(), "wb");
   if( !fp ) 
   {
      printf("Failed to open(write) File [%s] \n", fileName.c_str());
      return false;
   }

   uint8_t* filePreamble = new  uint8_t[128];
   memset(filePreamble, 0, 128);
   fwrite(filePreamble,128,1,fp);
   fwrite("DICM",4,1,fp);
   delete[] filePreamble;
   
 //  UpdateDirectoryRecordSequenceLength(); // TODO (if *really* usefull)
 
   gdcmDicomDirMeta *ptrMeta = GetDicomDirMeta();
   ptrMeta->Write(fp, gdcmExplicitVR);
   
   // force writing 0004|1220 [SQ ], that CANNOT exist within gdcmDicomDirMeta
   fwrite(&sq[0],4,1,fp);  // 0004 1220 
   //fwrite("SQ"  ,2,1,fp);  // SQ  // VR no written for 'No length' Sequences !
   //fwrite(&sq[2],6,1,fp);  // 00 ffffffff
   fwrite(&sq[3],4,1,fp);  // ffffffff 
        
   for(ListDicomDirPatient::iterator cc = patients.begin();cc!=patients.end();++cc)
   {
      (*cc)->Write( fp, gdcmExplicitVR );
   }
   
   // force writing Sequence Delimitation Item
   fwrite(&sqt[0],8,1,fp);  // fffe e0dd ffff ffff 

   fclose( fp );
   return true;
}

//-----------------------------------------------------------------------------
// Protected

/**
 * \ingroup gdcmDicomDir
 * \brief create a gdcmDocument-like chained list from a root Directory 
 * @param path entry point of the tree-like structure
 */
void gdcmDicomDir::CreateDicomDirChainedList(std::string const & path)
{
   CallStartMethod();
   gdcmDirList fileList(path,1); // gets recursively the file list
   unsigned int count = 0;
   VectDocument list;
   gdcmHeader *header;

   TagHT.clear();
   patients.clear();

   for( gdcmDirList::iterator it  = fileList.begin();
                              it != fileList.end();
                              ++it )
   {
      progress = (float)(count+1)/(float)fileList.size();
      CallProgressMethod();
      if( abort )
      {
         break;
      }

      header = new gdcmHeader( it->c_str() );
      if( !header )
      {
         dbg.Verbose( 1,
                      "gdcmDicomDir::CreateDicomDirChainedList: "
                      "failure in new Header ",
                      it->c_str() );
      }
      if( header->IsReadable() )
      {
         // Add the file header to the chained list:
         list.push_back(header);
         dbg.Verbose( 1,
                      "gdcmDicomDir::CreateDicomDirChainedList: readable ",
                      it->c_str() );
       }
       else
       {
          delete header;
       }
       count++;
   }

   // sorts Patient/Study/Serie/
   std::sort(list.begin(), list.end(), gdcmDicomDir::HeaderLessThan );

   std::string tmp = fileList.GetDirName();
      
   //for each Header of the chained list, add/update the Patient/Study/Serie/Image info
   SetElements(tmp, list);      
   CallEndMethod();
}

/**
 * \ingroup gdcmDicomDir
 * \brief   adds *the* Meta to a partially created DICOMDIR
 */
  
gdcmDicomDirMeta * gdcmDicomDir::NewMeta()
{
   gdcmDicomDirMeta *m = new gdcmDicomDirMeta( &TagHT );
  
   if ( TagHT.begin() != TagHT.end() ) // after Document Parsing
   { 
      TagDocEntryHT::iterator lastOneButSequence = TagHT.end();
      lastOneButSequence --;
      // This works because ALL the 'out of Sequence' Tags belong to Meta Elems
      // (we skip 0004|1220 [Directory record sequence] )
      for ( TagDocEntryHT::iterator cc  = TagHT.begin(); 
                                    cc != lastOneButSequence;
                                   ++cc)
      {
         m->AddDocEntry( cc->second );
      }
   }
   else  // after root directory parsing
   {
     //cout << "gdcmDicomDir::NewMeta avec FillObject" << endl;
     std::list<gdcmElement> elemList;
     elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirMetaElements();
     m->FillObject(elemList);
     // we create the Sequence manually
     //gdcmSeqEntry *se =NewSeqEntryByNumber(0x0004, 0x1220); // NOT YET!
     //m->AddEntry(se);
    }
   m->SetSQItemNumber(0); // To avoid further missprinting
   return m;  
}


/**
 * \brief   adds a new Patient (with the basic elements) to a partially created DICOMDIR
 */
gdcmDicomDirPatient * gdcmDicomDir::NewPatient()
{
   std::list<gdcmElement>::iterator it;
   uint16_t tmpGr,tmpEl;
   gdcmDictEntry *dictEntry;
   gdcmValEntry *entry;

   std::list<gdcmElement> elemList;   
   elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirPatientElements(); 
// Looks nice, but gdcmDicomDir IS NOT a gdcmObject ... 
//   gdcmDicomDirPatient *p = new gdcmDicomDirPatient(ptagHT);
//   FillObject(elemList);
//   patients.push_front( p );
//   return p;    
/// \todo TODO : find a trick to use FillObject !!!

   gdcmSQItem *s = new gdcmSQItem(0);

   // for all the DicomDirPatient Elements      
   for( it = elemList.begin(); it != elemList.end(); ++it ) 
   {
      tmpGr     = it->group;
      tmpEl     = it->elem;
      dictEntry = GetPubDict()->GetDictEntryByNumber(tmpGr, tmpEl);
      entry     = new gdcmValEntry( dictEntry );
      entry->SetOffset(0); // just to avoid further missprinting
      entry->SetValue( it->value );

      // dealing with value length ...
      
      if( dictEntry->GetGroup() == 0xfffe)
      {
         entry->SetLength(entry->GetValue().length());
      }
      else if( dictEntry->GetVR() == "UL" || dictEntry->GetVR() == "SL" )
      {
         entry->SetLength( 4 );
      } 
      else if( dictEntry->GetVR() == "US" || dictEntry->GetVR() == "SS" )
      {
         entry->SetLength(2); 
      } 
      else if( dictEntry->GetVR() == "SQ" )
      {
         entry->SetLength( 0xffffffff );
      }
      else
      {
         entry->SetLength( entry->GetValue().length() );
      }
      s->AddDocEntry( entry );
   }

   gdcmDicomDirPatient *p = new gdcmDicomDirPatient(s, &TagHT);
   patients.push_front( p );

   return p;   
}

/**
 * \brief   adds to the HTable 
 *          the gdcmEntries (Dicom Elements) corresponding to the given type
 * @param   path full path file name (only used when type = GDCM_DICOMDIR_IMAGE
 * @param   type gdcmObject type to create (GDCM_DICOMDIR_PATIENT,
 *          GDCM_DICOMDIR_STUDY, GDCM_DICOMDIR_SERIE ...)
 * @param   header gdcmHeader of the current file
 */
void gdcmDicomDir::SetElement(std::string &path,gdcmDicomDirType type,
                              gdcmDocument *header)
{
   std::list<gdcmElement> elemList;
   std::list<gdcmElement>::iterator it;
   uint16_t tmpGr, tmpEl;
   gdcmDictEntry *dictEntry;
   gdcmValEntry *entry;
   std::string val;
   gdcmSQItem *si = new gdcmSQItem(0); // all the items will be at level 1
   switch( type )
   {
      case GDCM_DICOMDIR_IMAGE:
         elemList = gdcmGlobal::GetDicomDirElements()->GetDicomDirImageElements();
         break;

      case GDCM_DICOMDIR_SERIE:
         elemList = gdcmGlobal::GetDicomDirElements()->GetDicomDirSerieElements();
         break;

      case GDCM_DICOMDIR_STUDY:
         elemList = gdcmGlobal::GetDicomDirElements()->GetDicomDirStudyElements();
         break;

      case GDCM_DICOMDIR_PATIENT:
         elemList = gdcmGlobal::GetDicomDirElements()->GetDicomDirPatientElements();
         break;
  
      case GDCM_DICOMDIR_META:
         elemList = gdcmGlobal::GetDicomDirElements()->GetDicomDirMetaElements();
         // add already done ?
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
      tmpGr     = it->group;
      tmpEl     = it->elem;
      dictEntry = GetPubDict()->GetDictEntryByNumber(tmpGr, tmpEl);

      entry     = new gdcmValEntry( dictEntry ); // Be sure it's never a BinEntry !

      entry->SetOffset(0); // just to avoid further missprinting
      entry->SetLength(0); // just to avoid further missprinting

      if( header ) // NULL when we Build Up (ex nihilo) a DICOMDIR
                   //   or when we add the META elems
         val = header->GetEntryByNumber(tmpGr, tmpEl);
      else
         val = GDCM_UNFOUND;

      if( val == GDCM_UNFOUND) 
      {
         if( tmpGr == 0x0004 && tmpEl == 0x1130 ) // File-set ID
         {
           // force to the *end* File Name
           val = GetName( path );
         }
         else if( tmpGr == 0x0004 && tmpEl == 0x1500 ) // Only used for image
         {
            if( header->GetFileName().substr(0, path.length()) != path )
            {
               dbg.Verbose(0, "gdcmDicomDir::SetElement : the base path"
                              " of file name is incorrect");
               val = header->GetFileName();
            }
            else
               val = &(header->GetFileName().c_str()[path.length()]);
         }
         else
            val = it->value;
      }
      else
      {
         if ( header->GetEntryLengthByNumber(tmpGr,tmpEl) == 0 )
            val = it->value;
      }

     // GDCM_UNFOUND or not !

      entry->SetValue( val ); // troubles expected when vr=SQ ...

      if( dictEntry )
      {
         if( dictEntry->GetGroup() == 0xfffe )
         {
            entry->SetLength( entry->GetValue().length() ); // FIXME 
         }
         else if( dictEntry->GetVR() == "UL" || dictEntry->GetVR() == "SL" )
         {
            entry->SetLength(4);
         }
         else if( dictEntry->GetVR() == "US" || dictEntry->GetVR() == "SS" )
         {
            entry->SetLength(2); 
         }
         else if( dictEntry->GetVR() == "SQ" )
         {
            entry->SetLength( 0xffffffff );
         }
         else
         {
            entry->SetLength( entry->GetValue().length() );
         }
      }

      if ( type == GDCM_DICOMDIR_META ) // fusible : should never print !
      {
         std::cout << " special Treatment for GDCM_DICOMDIR_META" << std::endl;
         entry->Print(); // just to see 
      }
      si->AddEntry(entry);
   }
   switch( type )
   {
      case GDCM_DICOMDIR_IMAGE:
         AddDicomDirImageToEnd(si);
         break;

      case GDCM_DICOMDIR_SERIE:
         AddDicomDirSerieToEnd(si);
         break;

      case GDCM_DICOMDIR_STUDY:
         AddDicomDirStudyToEnd(si);
         break;

      case GDCM_DICOMDIR_PATIENT:
         AddDicomDirPatientToEnd(si);
         break;

      default:
         return;
   }
   //int count=1;            // find a trick to increment
   //s->AddEntry(si, count); // Seg Faults 

}

/**
 * \brief   CallStartMethod
 */
void gdcmDicomDir::CallStartMethod()
{
   progress = 0.0f;
   abort    = false;
   if( startMethod )
   {
      startMethod( startArg );
   }
}

/**
 * \ingroup gdcmDicomDir
 * \brief   CallProgressMethod
 */
void gdcmDicomDir::CallProgressMethod()
{
   if( progressMethod )
   {
      progressMethod( progressArg );
   }
}

/**
 * \ingroup gdcmDicomDir
 * \brief   CallEndMethod
 */
void gdcmDicomDir::CallEndMethod()
{
   progress = 1.0f;
   if( endMethod )
   {
      endMethod( endArg );
   }
}

//-----------------------------------------------------------------------------
// Private
/**
 * \ingroup gdcmDicomDir
 * \brief create a 'gdcmDicomDir' from a DICOMDIR gdcmHeader 
 */
void gdcmDicomDir::CreateDicomDir()
{
   // The list is parsed. 
   //  When a DicomDir tag ("PATIENT", "STUDY", "SERIE", "IMAGE") is found :
   //  1 - we save the beginning iterator
   //  2 - we continue to parse
   //  3 - we find an other tag
   //       + we create the object for the precedent tag
   //       + loop to 1 -

   // Directory record sequence
   gdcmDocEntry *e = GetDocEntryByNumber(0x0004, 0x1220);
   if ( !e )
   {
      dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : NO Directory record"
                     " sequence (0x0004,0x1220)");
      /// \todo FIXME: what to do when the parsed file IS NOT a DICOMDIR file ? 
      return;         
   }
   
   gdcmSeqEntry* s = dynamic_cast<gdcmSeqEntry*>(e);
   if ( !s )
   {
      dbg.Verbose(0, "gdcmDicomDir::CreateDicomDir: no SeqEntry present");
      // useless : (0x0004,0x1220) IS a Sequence !
      return;
   }

   gdcmDicomDirType type = gdcmDicomDir::GDCM_DICOMDIR_META;
   metaElems = NewMeta();

   ListSQItem listItems = s->GetSQItems();
   
   gdcmDocEntry * d;
   std::string v;
   for( ListSQItem::iterator i = listItems.begin(); 
                             i !=listItems.end(); ++i ) 
   {
      d = (*i)->GetDocEntryByNumber(0x0004, 0x1430); // Directory Record Type
      if ( gdcmValEntry* ValEntry = dynamic_cast< gdcmValEntry* >(d) )
      {
         v = ValEntry->GetValue();
      }
      else
      {
         dbg.Verbose(0, "gdcmDicomDir::CreateDicomDir: not a ValEntry.");
         continue;
      }

      if( v == "PATIENT " )
      {
         AddDicomDirPatientToEnd( *i );
         type = gdcmDicomDir::GDCM_DICOMDIR_PATIENT;
      }
      else if( v == "STUDY " )
      {
         AddDicomDirStudyToEnd( *i );
         type = gdcmDicomDir::GDCM_DICOMDIR_STUDY;
      }
      else if( v == "SERIES" )
      {
         AddDicomDirSerieToEnd( *i );
         type = gdcmDicomDir::GDCM_DICOMDIR_SERIE;
      }
      else if( v == "IMAGE " ) 
      {
         AddDicomDirImageToEnd( *i );
         type = gdcmDicomDir::GDCM_DICOMDIR_IMAGE;
      }
      else
      {
         // It was not a 'PATIENT', nor a 'STUDY', nor a 'SERIE',
         // neither an 'IMAGE' SQItem. Skip to next item.
         continue;
      }
   }
}

/**
 * \ingroup gdcmDicomDir
 * \brief Well ... there is only one occurence  
 */
void gdcmDicomDir::AddDicomDirMeta()
{
   if( metaElems )
   {
      delete metaElems;
   }
   metaElems = new gdcmDicomDirMeta( &TagHT );
}

/**
 * \ingroup gdcmDicomDir
 * \brief  AddDicomDirPatientToEnd 
 * @param   s SQ Item to enqueue to the DicomPatient chained List
 */
void gdcmDicomDir::AddDicomDirPatientToEnd(gdcmSQItem *s)
{
   patients.push_back(new gdcmDicomDirPatient(s, &TagHT));
}

/**
 * \ingroup gdcmDicomDir
 * \brief  AddDicomDirStudyToEnd 
 * @param   s SQ Item to enqueue to the DicomDirStudy chained List
 */
 void gdcmDicomDir::AddDicomDirStudyToEnd(gdcmSQItem *s)
{
   if( patients.size() > 0 )
   {
      ListDicomDirPatient::iterator itp = patients.end();
      itp--;
      (*itp)->AddDicomDirStudy(new gdcmDicomDirStudy(s, &TagHT));
   }
}

/**
 * \ingroup gdcmDicomDir
 * \brief  AddDicomDirSerieToEnd 
 * @param   s SQ Item to enqueue to the DicomDirSerie chained List
 */
void gdcmDicomDir::AddDicomDirSerieToEnd(gdcmSQItem *s)
{
   if( patients.size() > 0 )
   {
      ListDicomDirPatient::iterator itp = patients.end();
      itp--;

      if( (*itp)->GetDicomDirStudies().size() > 0 )
      {
         ListDicomDirStudy::iterator itst=(*itp)->GetDicomDirStudies().end();
         itst--;
         (*itst)->AddDicomDirSerie(new gdcmDicomDirSerie(s, &TagHT));
      }
   }
}

/**
 * \ingroup gdcmDicomDir
 * \brief   AddDicomDirImageToEnd
 * @param   s SQ Item to enqueue to the DicomDirImage chained List
 */
 void gdcmDicomDir::AddDicomDirImageToEnd(gdcmSQItem *s)
{
   if( patients.size() > 0 )
   {
      ListDicomDirPatient::iterator itp = patients.end();
      itp--;

      if( (*itp)->GetDicomDirStudies().size() > 0 )
      {
         ListDicomDirStudy::iterator itst = (*itp)->GetDicomDirStudies().end();
         itst--;

         if( (*itst)->GetDicomDirSeries().size() > 0 )
         {
            ListDicomDirSerie::iterator its = (*itst)->GetDicomDirSeries().end();
            its--;
            (*its)->AddDicomDirImage(new gdcmDicomDirImage(s, &TagHT));
         }
      }
   }
}

/**
 * \ingroup gdcmDicomDir
 * \brief  for each Header of the chained list, add/update the Patient/Study/Serie/Image info 
 * @param   path path of the root directory
 * @param   list chained list of Headers
 */
void gdcmDicomDir::SetElements(std::string &path, VectDocument &list)
{
   std::string patPrevName         = "", patPrevID  = "";
   std::string studPrevInstanceUID = "", studPrevID = "";
   std::string serPrevInstanceUID  = "", serPrevID  = "";

   std::string patCurName,         patCurID;
   std::string studCurInstanceUID, studCurID;
   std::string serCurInstanceUID,  serCurID;

   //SetElement( path, GDCM_DICOMDIR_META,NULL); // already done (NewMeta) 

   for( VectDocument::iterator it = list.begin();
                              it != list.end(); ++it )
   {
      // get the current file characteristics
      patCurName         = (*it)->GetEntryByNumber(0x0010,0x0010); 
      patCurID           = (*it)->GetEntryByNumber(0x0010,0x0011); 
      studCurInstanceUID = (*it)->GetEntryByNumber(0x0020,0x000d);            
      studCurID          = (*it)->GetEntryByNumber(0x0020,0x0010);            
      serCurInstanceUID  = (*it)->GetEntryByNumber(0x0020,0x000e);            
      serCurID           = (*it)->GetEntryByNumber(0x0020,0x0011);

      if( patCurName != patPrevName || patCurID != patPrevID)
      {
         SetElement(path, GDCM_DICOMDIR_PATIENT, *it);
      }

      // if new Study Deal with 'STUDY' Elements   
      if( studCurInstanceUID != studPrevInstanceUID || studCurID != studPrevID )
      {
         SetElement(path, GDCM_DICOMDIR_STUDY, *it);
      }

      // if new Serie Deal with 'SERIE' Elements   
      if( serCurInstanceUID != serPrevInstanceUID || serCurID != serPrevID )
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
   }
}

/**
 * \ingroup gdcmDicomDir
 * \brief   compares two dgcmHeaders
 */
bool gdcmDicomDir::HeaderLessThan(gdcmDocument *header1, gdcmDocument *header2)
{
   return *header1 < *header2;
}

/**
 * \brief   Sets the accurate value for the (0x0004,0x1220) element of a DICOMDIR
 */
void gdcmDicomDir::UpdateDirectoryRecordSequenceLength()
{

/// \todo FIXME : to go on compiling
///
/// to be re written !
///   int offset = 0;
///   ListTag::iterator it;
///   uint16_t gr, el;
///   std::string vr;
///   for(it=listEntries.begin();it!=listEntries.end();++it) {
///      gr = (*it)->GetGroup();
///      el = (*it)->GetElement();
///      vr = (*it)->GetVR();      
///      if (gr !=0xfffe) {
///         if ( (vr == "OB") || (vr == "OW") || (vr == "SQ") ) {    
///            offset +=  4; // explicit VR AND OB, OW, SQ : 4 more bytes
///         }         
///         offset += 2 + 2 + 4 + (*it)->GetLength(); 
///      } else {
///         offset +=  4; // delimiters don't have a value.     
///      }            
///   }   
///   //bool res=SetEntryLengthByNumber(offset, 0x0004, 0x1220); // Hope there is no dupps.
///    SetEntryLengthByNumber(offset, 0x0004, 0x1220); // Hope there is no dupps.
///   return;
///
}

//-----------------------------------------------------------------------------
