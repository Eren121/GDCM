/*=========================================================================
  
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDir.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/28 14:29:52 $
  Version:   $Revision: 1.54 $
  
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
#include <errno.h>

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

void gdcmDicomDir::Initialize(void)
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

   metaElems = (gdcmDicomDirMeta *)0;   
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
 * @param exception_on_error whether we want to throw an exception or not
 */
gdcmDicomDir::gdcmDicomDir(const char *FileName, bool parseDir,
                           bool exception_on_error):
   gdcmDocument(FileName,exception_on_error,true) // true : enable SeQuences
{
   // que l'on ai passe un root directory ou un DICOMDIR
   // et quelle que soit la valeur de parseDir,
   // on a deja lance gdcmDocument 
   Initialize();

   // gdcmDocument already executed
   // if user passed a root directory, sure we didn't get anything

   if( GetEntry().begin() == GetEntry().end() ) 
   {
      dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : entry HT empty");

      if(strlen(FileName)==1 && FileName[0]=='.') { // user passed '.' as Name
                                            // we get current directory name
         char* dummy= new char[1000];
         getcwd(dummy, (size_t)1000);
         SetFileName(dummy); // will be converted into a string
         delete[] dummy;     // no longer needed   
      }

      if(parseDir)
      {
         dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : Parse directory"
                        " and create the DicomDir");
         ParseDirectory();
      } else {
         /// \todo if parseDir == false, it should be tagged as an error
      }
   }
   else
   {
      // Directory record sequence
      gdcmDocEntry *e = GetDocEntryByNumber(0x0004, 0x1220);
      if (e==NULL) {
         dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : NO Directory record"
                        " sequence (0x0004,0x1220)");
         /// \todo FIXME : what do we do when the parsed file IS NOT a
         ///       DICOMDIR file ?         
      }      
      CreateDicomDir();
   } 
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Constructor : creates an empty gdcmDicomDir
 * @param   exception_on_error whether we want to throw an exception or not
 */
gdcmDicomDir::gdcmDicomDir(bool exception_on_error):                           
   gdcmDocument(exception_on_error)
{ 

   Initialize();

   std::string pathBidon = "Bidon"; // Sorry, NULL not allowed ...
   SetElement(pathBidon, GDCM_DICOMDIR_META, NULL); // Set the META elements
   AddDicomDirMeta();
}

/**
 * \brief  Canonical destructor 
 */
gdcmDicomDir::~gdcmDicomDir() 
{
   SetStartMethod(NULL);
   SetProgressMethod(NULL);
   SetEndMethod(NULL);

   if(metaElems)
      delete metaElems;
   
   for(ListDicomDirPatient::iterator cc = patients.begin();
                                     cc!= patients.end();
                                   ++cc)
   {
      delete *cc;
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
      metaElems->SetPrintLevel(printLevel);
      metaElems->Print(os);   
   }   
   for(ListDicomDirPatient::iterator cc  = patients.begin();
                                     cc != patients.end();
                                   ++cc)
   {
     (*cc)->SetPrintLevel(printLevel);
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
 * @return true when gdcmDocument is the one of a reasonable DicomDir,
 *         false otherwise. 
 */
bool gdcmDicomDir::IsReadable(void)
{
   if(!gdcmDocument::IsReadable())
      return(false);
   if(!metaElems)
      return(false);
   if(patients.size()<=0)
      return(false);

   return(true);
}

/**
 * \ingroup gdcmDicomDir
 * \brief  fills the whole structure, starting from a root Directory
 */
void gdcmDicomDir::ParseDirectory(void)
{
   CreateDicomDirChainedList(GetFileName());
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
void gdcmDicomDir::SetStartMethod(gdcmMethod *method,void *arg,gdcmMethod *argDelete)
{
   if((startArg)&&(startMethodArgDelete))
      startMethodArgDelete(startArg);

   startMethod=method;
   startArg=arg;
   startMethodArgDelete=argDelete;
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
   startMethodArgDelete=method;
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Set the progress method to call when the parsing of the directory progress
 * @param   method Method to call
 * @param   arg    Argument to pass to the method
 * @param   argDelete    Argument  
 * \warning In python : the arg parameter isn't considered
 */
void gdcmDicomDir::SetProgressMethod(gdcmMethod *method,void *arg,gdcmMethod *argDelete)
{
   if((progressArg)&&(progressMethodArgDelete))
      progressMethodArgDelete(progressArg);

   progressMethod=method;
   progressArg=arg;
   progressMethodArgDelete=argDelete;
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
   progressMethodArgDelete=method;
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Set the end method to call when the parsing of the directory ends
 * @param   method Method to call
 * @param   arg    Argument to pass to the method
 * @param   argDelete    Argument 
 * \warning In python : the arg parameter isn't considered
 */
void gdcmDicomDir::SetEndMethod(gdcmMethod *method, void *arg, gdcmMethod *argDelete)
{
   if((endArg)&&(endMethodArgDelete))
      endMethodArgDelete(endArg);

   endMethod=method;
   endArg=arg;
   endMethodArgDelete=argDelete;
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
   endMethodArgDelete=method;
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
 
bool gdcmDicomDir::Write(std::string fileName) 
{
   FILE * fp1;

   fp1=fopen(fileName.c_str(),"wb");
   if(fp1==NULL) 
   {
      printf("Failed to open(write) File [%s] \n",fileName.c_str());
      return(false);
   }

   char * filePreamble = new char[128];
   fwrite(filePreamble,128,1,fp1);
   fwrite("DICM",4,1,fp1);
   delete[] filePreamble;
   UpdateDirectoryRecordSequenceLength();
   WriteEntries(fp1);

   fclose(fp1);
   return true;
}

/**
 * \brief   Writes in a file using the tree-like structure.
 * @param   _fp already open file pointer
 */

void gdcmDicomDir::WriteEntries(FILE *) //_fp
{   
   /// \todo (?) tester les echecs en ecriture 
   ///          (apres chaque fwrite, dans le WriteEntry)


/* TODO : to go on compiling

   gdcmDicomDirMeta *ptrMeta;
   ListDicomDirPatient::iterator  itPatient;
   ListDicomDirStudy::iterator    itStudy;
   ListDicomDirSerie::iterator    itSerie;
   ListDicomDirImage::iterator    itImage; 
   ListTag::iterator i; 
   
   ptrMeta= GetDicomDirMeta();
   for(i=ptrMeta->debut();i!=ptrMeta->fin();++i) {
      WriteEntry(*i,_fp, gdcmExplicitVR);
   }   
    
   itPatient = GetDicomDirPatients().begin(); 
   while ( itPatient != GetDicomDirPatients().end() ) {
      for(i=(*itPatient)->debut();i!=(*itPatient)->fin();++i) {
         WriteEntry(*i,_fp, gdcmExplicitVR);
      }
      itStudy = ((*itPatient)->GetDicomDirStudies()).begin();     
      while (itStudy != (*itPatient)->GetDicomDirStudies().end() ) {
         for(i=(*itStudy)->debut();i!=(*itStudy)->fin();++i) {
            WriteEntry(*i,_fp, gdcmExplicitVR);
         } 
         itSerie = ((*itStudy)->GetDicomDirSeries()).begin();
         while (itSerie != (*itStudy)->GetDicomDirSeries().end() ) {
            for(i=(*itSerie)->debut();i!=(*itSerie)->fin();++i) {
               WriteEntry(*i,_fp, gdcmExplicitVR);
            }
            itImage = ((*itSerie)->GetDicomDirImages()).begin();
            while (itImage != (*itSerie)->GetDicomDirImages().end() ) {
               for(i=(*itImage)->debut();i!=(*itImage)->fin();++i) {
                  WriteEntry(*i,_fp, gdcmExplicitVR);
               }
               ++itImage;
            }
            ++itSerie;
         }
         ++itStudy;
      } 
      ++itPatient;     
   }
   */
}   
   
//-----------------------------------------------------------------------------
// Protected

/**
 * \ingroup gdcmDicomDir
 * \brief create a gdcmDocument-like chained list from a root Directory 
 * @param path entry point of the tree-like structure
 */
void gdcmDicomDir::CreateDicomDirChainedList(std::string path)
{
   CallStartMethod();
   gdcmDirList fileList(path,1); // gets recursively the file list
   unsigned int count=0;
   VectDocument list;
   gdcmHeader *header;

   tagHT.clear();
   patients.clear();

   for(gdcmDirList::iterator it=fileList.begin(); 
                             it!=fileList.end(); 
                             ++it) 
   {
      progress=(float)(count+1)/(float)fileList.size();
      CallProgressMethod();
      if(abort)
          break;

      header=new gdcmHeader(it->c_str(),false,true);
      if(!header) {
         std::cout << "echec new Header " << it->c_str() << std::endl; // JPR
      }
      if(header->IsReadable()) {
         list.push_back(header);  // adds the file header to the chained list
         std::cout << "readable : " <<it->c_str() << std::endl; // JPR
       }
      else
         delete header;

      count++;

   }
   // sorts Patient/Study/Serie/
   std::sort(list.begin(),list.end(),gdcmDicomDir::HeaderLessThan);

   std::string tmp=fileList.GetDirName();
      
   //for each Header of the chained list, add/update the Patient/Study/Serie/Image info
   SetElements(tmp,list);
      
   CallEndMethod();
}

/**
 * \ingroup gdcmDicomDir
 * \brief   adds *the* Meta to a partially created DICOMDIR
 */
 
 // FIXME : Heuuuuu ! Il prend les Entries du Document deja parse,
 //                   il ne fabrique rien !
  
gdcmDicomDirMeta * gdcmDicomDir::NewMeta(void) {
   gdcmDicomDirMeta *m = new gdcmDicomDirMeta(&tagHT);   
   for (TagDocEntryHT::iterator cc = tagHT.begin();cc != tagHT.end();++cc) {
      m->AddDocEntry(cc->second);
   }
   return m;  
}


/**
 * \ingroup gdcmDicomDir
 * \brief   adds a new Patient (with the basic elements) to a partially created DICOMDIR
 */
gdcmDicomDirPatient * gdcmDicomDir::NewPatient(void) {
   std::list<gdcmElement> elemList;
   std::list<gdcmElement>::iterator it;
   guint16 tmpGr,tmpEl;
   gdcmDictEntry *dictEntry;
   gdcmValEntry *entry;
   
   gdcmSQItem *s = new gdcmSQItem(0);
   
   elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirPatientElements();  
   
   // TODO : use FillObject !!!

   // for all the DicomDirPatient Elements 
     
   for(it=elemList.begin();it!=elemList.end();++it) 
   {
      tmpGr=it->group;
      tmpEl=it->elem;
      dictEntry=GetPubDict()->GetDictEntryByNumber(tmpGr,tmpEl);      
      entry=new gdcmValEntry(dictEntry);
      entry->SetOffset(0); // just to avoid further missprinting
      entry->SetValue(it->value); 

      // dealing with value length ...
      
      if(dictEntry->GetGroup()==0xfffe) 
         { 
            entry->SetLength(entry->GetValue().length());
         }
      else if( (dictEntry->GetVR()=="UL") || (dictEntry->GetVR()=="SL") ) 
         {
            entry->SetLength(4);
         } 
      else if( (dictEntry->GetVR()=="US") || (dictEntry->GetVR()=="SS") ) 
         {
            entry->SetLength(2); 
         } 
      else if(dictEntry->GetVR()=="SQ") 
         {
            entry->SetLength(0xffffffff);
         }
      else
         {
            entry->SetLength(entry->GetValue().length());
         } 
      s->AddDocEntry(entry);
   }

   gdcmDicomDirPatient *p = new gdcmDicomDirPatient(s, &tagHT);
   patients.push_front(p);
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
   guint16 tmpGr, tmpEl;
   gdcmDictEntry *dictEntry;
   gdcmValEntry *entry;
   std::string val;

   switch(type){
      case GDCM_DICOMDIR_PATIENT:
         elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirPatientElements();
         break;
      case GDCM_DICOMDIR_STUDY:
         elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirStudyElements();
         break;
      case GDCM_DICOMDIR_SERIE:
         elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirSerieElements();
         break;
      case GDCM_DICOMDIR_IMAGE:
         elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirImageElements();
         break;
      case GDCM_DICOMDIR_META:
         elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirMetaElements();
         break;
      default:
         return;
   }

   for(it=elemList.begin();it!=elemList.end();++it) {
      tmpGr=it->group;
      tmpEl=it->elem;
      dictEntry=GetPubDict()->GetDictEntryByNumber(tmpGr,tmpEl);
      entry=new gdcmValEntry(dictEntry); // Be sure it's never a BinEntry !

      entry->SetOffset(0); // just to avoid further missprinting

      if(header)
         val=header->GetEntryByNumber(tmpGr,tmpEl);
      else
         val=GDCM_UNFOUND;

      if(val==GDCM_UNFOUND) 
      {
         if((tmpGr==0x0004) &&(tmpEl==0x1130) ) // File-set ID
         {
           // force to the *end* File Name
           val=GetName(path);
         }
         else if( (tmpGr==0x0004) && (tmpEl==0x1500) ) // Only used for image
         {
            if(header->GetFileName().substr(0,path.length())!=path)
            {
               dbg.Verbose(0, "gdcmDicomDir::SetElement : the base path"
                              " of file name is incorrect");
               val=header->GetFileName();
            }
            else
            {
               val=&(header->GetFileName().c_str()[path.length()]);
            }   
         }
         else
         {
            val=it->value;
         }
      } 
      else
      {
         if (header->GetEntryLengthByNumber(tmpGr,tmpEl)== 0)
            val=it->value;
      }
            
      entry->SetValue(val);

      if(dictEntry)
      {
         if(dictEntry->GetGroup()==0xfffe)  {
            entry->SetLength(entry->GetValue().length());
      }
     else if( (dictEntry->GetVR()=="UL") || (dictEntry->GetVR()=="SL") ) 
         {
            entry->SetLength(4);
         } 
      else if( (dictEntry->GetVR()=="US") || (dictEntry->GetVR()=="SS") ) 
         {
            entry->SetLength(2); 
         } 
      else if(dictEntry->GetVR()=="SQ") 
         {
            entry->SetLength(0xffffffff);
         }
      else
         {
            entry->SetLength(entry->GetValue().length());
         }
      }
      //AddDocEntry(entry); // both in H Table and in chained list
      tagHT[entry->GetKey()] = entry;          // FIXME : use a SEQUENCE !
   }     
}

/**
 * \brief   CallStartMethod
 */
void gdcmDicomDir::CallStartMethod(void)
{
   progress=0.0f;
   abort=false;
   if(startMethod)
      startMethod(startArg);
}
/**
 * \ingroup gdcmDicomDir
 * \brief   CallProgressMethod
 */
void gdcmDicomDir::CallProgressMethod(void)
{
   if(progressMethod)
      progressMethod(progressArg);
}
/**
 * \ingroup gdcmDicomDir
 * \brief   CallEndMethod
 */
void gdcmDicomDir::CallEndMethod(void)
{
   progress=1.0f;
   if(endMethod)
      endMethod(endArg);
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

   gdcmDicomDirType type=gdcmDicomDir::GDCM_DICOMDIR_META;
   
   // Directory record sequence
   gdcmDocEntry *e = GetDocEntryByNumber(0x0004, 0x1220);
   if (e==NULL)
   {
      dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : NO Directory record"
                     " sequence (0x0004,0x1220)");
      /// \todo FIXME: what to do when the parsed file IS NOT a DICOMDIR file ? 
      return;         
   }
   
   gdcmSeqEntry* s = dynamic_cast<gdcmSeqEntry*>(e);
   if (!s)
   {
      dbg.Verbose(0, "gdcmDicomDir::CreateDicomDir: no SeqEntry present");
      return;
   }

   ListSQItem listItems = s->GetSQItems();
   gdcmDicomDirMeta *m = new gdcmDicomDirMeta(&tagHT);
   (void)m; //??
   
   gdcmDocEntry * d;   
   std::string v;
   for(ListSQItem::iterator i=listItems.begin(); i !=listItems.end();++i) 
   {  
      d=(*i)->GetDocEntryByNumber(0x0004, 0x1430); // Directory Record Type   
      if (gdcmValEntry* ValEntry = dynamic_cast< gdcmValEntry* >(d) )
      {
         v = ValEntry->GetValue();
      }
      else 
      {
         dbg.Verbose(0, "gdcmDicomDir::CreateDicomDir: not a ValEntry.");
         continue;
      } 

      if(v=="PATIENT ") 
      {  
         AddDicomDirPatientToEnd(*i);
         //AddObjectToEnd(type,*i);
         type=gdcmDicomDir::GDCM_DICOMDIR_PATIENT;
      }

      else if(v=="STUDY ")
      {
         AddDicomDirStudyToEnd(*i);
        // AddObjectToEnd(type,*i);
         type=gdcmDicomDir::GDCM_DICOMDIR_STUDY;
      }

      else if(v=="SERIES") 
      {
         AddDicomDirSerieToEnd(*i);
       //  AddObjectToEnd(type,*i);
         type=gdcmDicomDir::GDCM_DICOMDIR_SERIE;
      }

      else if(v=="IMAGE ") 
      {
         AddDicomDirImageToEnd(*i);
      //   AddObjectToEnd(type,*i);
         type=gdcmDicomDir::GDCM_DICOMDIR_IMAGE;
      }
      
      else
         // It was not a 'PATIENT', nor a 'STUDY', nor a 'SERIE',
         // neither an 'IMAGE' SQItem. Skip to next item.
         continue;
   }
}
/**
 * \ingroup gdcmDicomDir
 * \brief   AddObjectToEnd
 * @param   type
 * @param   begin iterator on the first DocEntry within the chained List
 * @param   end iterator on the last DocEntry within the chained List
 */
 
// now  useless ?

/*void gdcmDicomDir::AddObjectToEnd(gdcmDicomDirType type,gdcmSQItem *s)
{
   if(s==NULL) // ??
      return;

   switch(type)
   {
      case gdcmDicomDir::GDCM_DICOMDIR_META:
         AddDicomDirMeta();
         break;      
      case gdcmDicomDir::GDCM_DICOMDIR_PATIENT:
         AddDicomDirPatientToEnd(s);
         break;
      case gdcmDicomDir::GDCM_DICOMDIR_STUDY:
         AddDicomDirStudyToEnd(s);
         break;
      case gdcmDicomDir::GDCM_DICOMDIR_SERIE:
         AddDicomDirSerieToEnd(s);
         break;
      case gdcmDicomDir::GDCM_DICOMDIR_IMAGE:
         AddDicomDirImageToEnd(s);
         break;
      case gdcmDicomDir::GDCM_DICOMDIR_NONE:
         AddDicomDirImageToEnd(s);        //FIXME
         break;
   }
}

*/

/**
 * \ingroup gdcmDicomDir
 * \brief Well ... there is only one occurence  
*/
void gdcmDicomDir::AddDicomDirMeta()
{
   if(metaElems)
      delete metaElems;
   metaElems = new gdcmDicomDirMeta(&tagHT);
}

/**
 * \ingroup gdcmDicomDir
 * \brief  AddDicomDirPatientToEnd 
 * @param   s SQ Item to enqueue to the DicomPatient chained List
*/
void gdcmDicomDir::AddDicomDirPatientToEnd(gdcmSQItem *s)
{
   patients.push_back(new gdcmDicomDirPatient(s, &tagHT));
}

/**
 * \ingroup gdcmDicomDir
 * \brief  AddDicomDirStudyToEnd 
 * @param   s SQ Item to enqueue to the DicomDirStudy chained List
 */
 void gdcmDicomDir::AddDicomDirStudyToEnd(gdcmSQItem *s)
{
   if(patients.size()>0)
   {
      ListDicomDirPatient::iterator itp=patients.end();
      itp--;
     (*itp)->AddDicomDirStudy(new gdcmDicomDirStudy(s, &tagHT));
   }
}
/**
 * \ingroup gdcmDicomDir
 * \brief  AddDicomDirSerieToEnd 
 * @param   s SQ Item to enqueue to the DicomDirSerie chained List
 */
void gdcmDicomDir::AddDicomDirSerieToEnd(gdcmSQItem *s)
{
   if(patients.size()>0)
   {
      ListDicomDirPatient::iterator itp=patients.end();
      itp--;

      if((*itp)->GetDicomDirStudies().size()>0)
      {
         ListDicomDirStudy::iterator itst=(*itp)->GetDicomDirStudies().end();
         itst--;
        (*itst)->AddDicomDirSerie(new gdcmDicomDirSerie(s, &tagHT));
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
   if(patients.size()>0)
   {
      ListDicomDirPatient::iterator itp=patients.end();
      itp--;

      if((*itp)->GetDicomDirStudies().size()>0)
      {
         ListDicomDirStudy::iterator itst=(*itp)->GetDicomDirStudies().end();
         itst--;

         if((*itst)->GetDicomDirSeries().size()>0)
         {
            ListDicomDirSerie::iterator its=(*itst)->GetDicomDirSeries().end();
            its--;
           (*its)->AddDicomDirImage(new gdcmDicomDirImage(s, &tagHT));
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
   std::string patPrevName="",         patPrevID="";
   std::string studPrevInstanceUID="", studPrevID="";
   std::string serPrevInstanceUID="",  serPrevID="";

   std::string patCurName,         patCurID;
   std::string studCurInstanceUID, studCurID;
   std::string serCurInstanceUID,  serCurID;

   SetElement(path,GDCM_DICOMDIR_META,NULL);

   for(VectDocument::iterator it=list.begin();it!=list.end();++it) 
   {
      // get the current file characteristics
      patCurName=        (*it)->GetEntryByNumber(0x0010,0x0010); 
      patCurID=          (*it)->GetEntryByNumber(0x0010,0x0011); 
      studCurInstanceUID=(*it)->GetEntryByNumber(0x0020,0x000d);            
      studCurID=         (*it)->GetEntryByNumber(0x0020,0x0010);            
      serCurInstanceUID= (*it)->GetEntryByNumber(0x0020,0x000e);            
      serCurID=          (*it)->GetEntryByNumber(0x0020,0x0011);

      if(patCurName!=patPrevName || patCurID!=patPrevID) 
         SetElement(path,GDCM_DICOMDIR_PATIENT,*it);

      // if new Study Deal with 'STUDY' Elements   
      if(studCurInstanceUID!=studPrevInstanceUID || studCurID!=studPrevID) 
         SetElement(path,GDCM_DICOMDIR_STUDY,*it);

      // if new Serie Deal with 'SERIE' Elements   
      if(serCurInstanceUID!=serPrevInstanceUID || serCurID!=serPrevID) 
         SetElement(path,GDCM_DICOMDIR_SERIE,*it);
      
      // Always Deal with 'IMAGE' Elements  
      SetElement(path,GDCM_DICOMDIR_IMAGE,*it);

      patPrevName=        patCurName;
      patPrevID=          patCurID;
      studPrevInstanceUID=studCurInstanceUID;
      studPrevID=         studCurID;
      serPrevInstanceUID= serCurInstanceUID;
      serPrevID=          serCurID;
   }
}

/**
 * \ingroup gdcmDicomDir
 * \brief   compares two dgcmHeaders
 */
bool gdcmDicomDir::HeaderLessThan(gdcmDocument *header1,gdcmDocument *header2)
{
   return(*header1<*header2);
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Sets the accurate value for the (0x0004,0x1220) element of a DICOMDIR
 */

void gdcmDicomDir::UpdateDirectoryRecordSequenceLength() {

// FIXME : to go on compiling

// to be re written !
/*
   int offset = 0;
   ListTag::iterator it;
   guint16 gr, el;
   std::string vr;
   for(it=listEntries.begin();it!=listEntries.end();++it) {
      gr = (*it)->GetGroup();
      el = (*it)->GetElement();
      vr = (*it)->GetVR();      
      if (gr !=0xfffe) {
         if ( (vr == "OB") || (vr == "OW") || (vr == "SQ") ) {    
            offset +=  4; // explicit VR AND OB, OW, SQ : 4 more bytes
         }         
         offset += 2 + 2 + 4 + (*it)->GetLength(); 
      } else {
         offset +=  4; // delimiters don't have a value.     
      }            
   }   
   //bool res=SetEntryLengthByNumber(offset, 0x0004, 0x1220); // Hope there is no dupps.
    SetEntryLengthByNumber(offset, 0x0004, 0x1220); // Hope there is no dupps.
   return;
   */
}

//-----------------------------------------------------------------------------
