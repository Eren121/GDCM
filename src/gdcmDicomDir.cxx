// gdcmDicomDir.cxx
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
//  For full DICOMDIR description, see:
//  PS 3.3-2003, pages 731-750
//-----------------------------------------------------------------------------


// Constructor / Destructor

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
   gdcmParser(FileName,exception_on_error,true) // true : enable SeQuences
{
 // que l'on ai passe un root directory ou un DICOMDIR
 // et quelle que soit la valeur de parseDir,
 // on a lance gdcmParser 
      
   startMethod=            NULL;
   progressMethod=         NULL;
   endMethod=              NULL;
   startMethodArgDelete=   NULL;
   progressMethodArgDelete=NULL;
   endMethodArgDelete=     NULL;
   startArg=               NULL;
   progressArg=            NULL;
   endArg=                 NULL;

   progress=0.0;
   abort=false;

   metaElems=NULL;

// gdcmParser already executed
// if user passed a root directory, sure we didn't get anything

   if( GetListEntry().begin()==GetListEntry().end() ) 
   {
     // if parseDir == false, it should be tagged as an error
      dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : entry list empty");

      if(strlen(FileName)==1 && FileName[0]=='.') { // user passed '.' as Name
                                            // we get current directory name
         char* dummy= new char[1000];
         getcwd(dummy, (size_t)1000);
         SetFileName(dummy); // will be converted into a string
         delete[] dummy;        // no longer needed   
      }

      if(parseDir)
      {
         dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : Parse directory"
                        " and create the DicomDir");
         ParseDirectory();
      }
   }
   else {
      CreateDicomDir();
      CheckBoundaries(); // to maintain consistency between 
                         // home-made gdcmDicomDir 
                         // and the ones comming from a DICOMDIR file
   } 
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Constructor : creates an empty gdcmDicomDir
 * @param   exception_on_error whether we want to throw an exception or not
 */
gdcmDicomDir::gdcmDicomDir(bool exception_on_error):                           
   gdcmParser(exception_on_error)
{ 
   startMethod=            NULL;
   progressMethod=         NULL;
   endMethod=              NULL;
   startMethodArgDelete=   NULL;
   progressMethodArgDelete=NULL;
   endMethodArgDelete=     NULL;
   startArg=               NULL;
   progressArg=            NULL;
   endArg=                 NULL;

   progress=0.0;
   abort=false;
   std::string pathBidon = ""; // Sorry, NULL not allowed ...
   SetElement(pathBidon, GDCM_DICOMDIR_META, NULL); // Set the META elements
   
   AddDicomDirMetaToEnd(listEntries.begin(),--listEntries.end());
}


/**
 * \ingroup gdcmDicomDir
 * \brief  Canonical destructor 
 */
gdcmDicomDir::~gdcmDicomDir() 
{
   SetStartMethod(NULL);
   SetProgressMethod(NULL);
   SetEndMethod(NULL);

   if(metaElems)
      delete metaElems;
   
   for(ListDicomDirPatient::iterator cc=patients.begin();cc!=patients.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDicomDir
 * \brief  Canonical Printer 
 */
void gdcmDicomDir::Print(std::ostream &os)
{
   if(metaElems)
   {
      metaElems->SetPrintLevel(printLevel);
      metaElems->Print(os);   
   }   
   for(ListDicomDirPatient::iterator cc=patients.begin();cc!=patients.end();++cc)
   {
     (*cc)->SetPrintLevel(printLevel);
     (*cc)->Print(os);     
   }
}

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup gdcmDicomDir
 * \brief  This predicate, based on hopefully reasonable heuristics,
 *         decides whether or not the current header was properly parsed
 *         and contains the mandatory information for being considered as
 *         a well formed and usable DicomDir.
 * @return true when gdcmParser is the one of a reasonable DicomDir,
 *         false otherwise. 
 */
bool gdcmDicomDir::IsReadable(void)
{
   if(!gdcmParser::IsReadable())
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

void gdcmDicomDir::WriteEntries(FILE *_fp)
{   
   /// \todo (?) tester les echecs en ecriture 
   ///          (apres chaque fwrite, dans le WriteEntry)

   gdcmDicomDirMeta *ptrMeta;
   ListDicomDirPatient::iterator  itPatient;
   ListDicomDirStudy::iterator    itStudy;
   ListDicomDirSerie::iterator    itSerie;
   ListDicomDirImage::iterator    itImage; 
   ListTag::iterator i; 
   
   ptrMeta= GetDicomDirMeta();
   for(i=ptrMeta->debut();i!=ptrMeta->fin();++i) {
      WriteEntry(*i,_fp, ExplicitVR);
   }   
    
   itPatient = GetDicomDirPatients().begin(); 
   while ( itPatient != GetDicomDirPatients().end() ) {
      for(i=(*itPatient)->debut();i!=(*itPatient)->fin();++i) {
         WriteEntry(*i,_fp, ExplicitVR);
      }
      itStudy = ((*itPatient)->GetDicomDirStudies()).begin();	      
      while (itStudy != (*itPatient)->GetDicomDirStudies().end() ) {	
         for(i=(*itStudy)->debut();i!=(*itStudy)->fin();++i) {
            WriteEntry(*i,_fp, ExplicitVR);
         } 
         itSerie = ((*itStudy)->GetDicomDirSeries()).begin();
         while (itSerie != (*itStudy)->GetDicomDirSeries().end() ) {
            for(i=(*itSerie)->debut();i!=(*itSerie)->fin();++i) {
               WriteEntry(*i,_fp, ExplicitVR);
            }
            itImage = ((*itSerie)->GetDicomDirImages()).begin();
            while (itImage != (*itSerie)->GetDicomDirImages().end() ) {
               for(i=(*itImage)->debut();i!=(*itImage)->fin();++i) {
                  WriteEntry(*i,_fp, ExplicitVR);
               }
               ++itImage;   	       	    
	    }
	    ++itSerie;	    	     	 	      
         }
	 ++itStudy; 	       
      } 
      ++itPatient;     
   }
}   
   
//-----------------------------------------------------------------------------
// Protected

/**
 * \ingroup gdcmDicomDir
 * \brief create a gdcmHeader-like chained list from a root Directory 
 * @param path entry point of the tree-like structure
 */
void gdcmDicomDir::CreateDicomDirChainedList(std::string path)
{
   CallStartMethod();

   gdcmDirList fileList(path,1); // gets recursively the file list
   unsigned int count=0;
   ListHeader list;
   gdcmHeader *header;

   listEntries.clear();
   patients.clear();

   for(gdcmDirList::iterator it=fileList.begin(); 
                             it!=fileList.end(); 
                             ++it) 
   {
      progress=(float)(count+1)/(float)fileList.size();
      CallProgressMethod();
      if(abort)
         break;

      header=new gdcmHeader(it->c_str());
      if(header->IsReadable())
         list.push_back(header);  // adds the file header to the chained list
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
 * \brief modifies the limits of a gdcmObject, created from a DICOMDIR file  
 */

void gdcmDicomDir::CheckBoundaries()
{   
   ListDicomDirPatient::iterator  itPatient;
   ListDicomDirStudy::iterator    itStudy;
   ListDicomDirSerie::iterator    itSerie;
   ListDicomDirImage::iterator    itImage; 
   ListTag::iterator i,j; 
   
   GetDicomDirMeta()->ResetBoundaries(0);   

   itPatient = GetDicomDirPatients().begin(); 
   while ( itPatient != GetDicomDirPatients().end() ) {
      (*itPatient)->ResetBoundaries(1);            
      itStudy = ((*itPatient)->GetDicomDirStudies()).begin();	      
      while (itStudy != (*itPatient)->GetDicomDirStudies().end() ) {	
         (*itStudy)->ResetBoundaries(1); 
         itSerie = ((*itStudy)->GetDicomDirSeries()).begin();
         while (itSerie != (*itStudy)->GetDicomDirSeries().end() ) {
            (*itSerie)->ResetBoundaries(1);
            itImage = ((*itSerie)->GetDicomDirImages()).begin();
            while (itImage != (*itSerie)->GetDicomDirImages().end() ) {
               (*itImage)->ResetBoundaries(1);
              ++itImage;
            }
            ++itSerie;
        }
        ++itStudy;
     } 
     ++itPatient;     
   }
} 


/**
 * \ingroup gdcmDicomDir
 * \brief   adds a new Patient to a partially created DICOMDIR
 */
gdcmDicomDirPatient * gdcmDicomDir::NewPatient(void) {
   std::list<gdcmElement> elemList;
   std::list<gdcmElement>::iterator it;
   guint16 tmpGr,tmpEl;
   gdcmDictEntry *dictEntry;
   gdcmHeaderEntry *entry;
   
   elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirPatientElements();  
   std::list<gdcmHeaderEntry *>::iterator debInsertion, finInsertion, i,j; 
         
   debInsertion = metaElems->fin(); 
   ++debInsertion;
   finInsertion=debInsertion;

   // for all the DicomDirPatient Elements   
   for(it=elemList.begin();it!=elemList.end();++it) 
   {
      tmpGr=it->group;
      tmpEl=it->elem;
      dictEntry=GetPubDict()->GetDictEntryByNumber(tmpGr,tmpEl);      
      entry=new gdcmHeaderEntry(dictEntry);
      entry->SetOffset(0); // just to avoid missprinting
      entry->SetValue(it->value);

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
	                                      
      tagHT.insert( PairHT(entry->GetKey(),entry) ); // add in the (multimap) H Table
      listEntries.insert(debInsertion ,entry);       // en tete de liste des Patients				                 
      ++finInsertion;
   }

   i=metaElems->fin();
   i++;

   gdcmDicomDirPatient *p = new gdcmDicomDirPatient(i, --debInsertion,
                                                    &tagHT, &listEntries);
   patients.push_front(p);
   return p;   
}

/**
 * \ingroup gdcmDicomDir
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
   ListTag::iterator begin;
   ListTag::iterator end, end2;

   begin=listEntries.begin();
   end=begin;
   for(ListTag::iterator i=end;i !=listEntries.end();++i) 
   {
      std::string v=(*i)->GetValue();
      if(v=="PATIENT ") 
      {
         end=end2=i;
         AddObjectToEnd(type,begin,--end2);
         type=gdcmDicomDir::GDCM_DICOMDIR_PATIENT;
         begin=end;
      }	

      if(v=="STUDY ")
      {
         end=end2=i;
         AddObjectToEnd(type,begin,--end2);
         type=gdcmDicomDir::GDCM_DICOMDIR_STUDY;
         begin=end;
      }

      if(v=="SERIES") 
      {
         end=end2=i;
         AddObjectToEnd(type,begin,--end2);
         type=gdcmDicomDir::GDCM_DICOMDIR_SERIE;
         begin=end;
      }

      if(v=="IMAGE ") 
      {
         end=end2=i;
         AddObjectToEnd(type,begin,--end2);
         type=gdcmDicomDir::GDCM_DICOMDIR_IMAGE;
         begin=end;
      }
   }

   end=end2=listEntries.end();
   if(begin!=end)
      AddObjectToEnd(type,begin,--end2);
}
/**
 * \ingroup gdcmDicomDir
 * \brief   AddObjectToEnd
 * @param   type
 * @param   begin iterator on the first HeaderEntry within the chained List
 * @param   end iterator on the last HeaderEntry within the chained List
 */
void gdcmDicomDir::AddObjectToEnd(gdcmDicomDirType type,ListTag::iterator begin,ListTag::iterator end)
{
   if(begin==end)
      return;

   switch(type)
   {
      case gdcmDicomDir::GDCM_DICOMDIR_META:
         AddDicomDirMetaToEnd(begin,end);
         break;      
      case gdcmDicomDir::GDCM_DICOMDIR_PATIENT:
         AddDicomDirPatientToEnd(begin,end);
         break;
      case gdcmDicomDir::GDCM_DICOMDIR_STUDY:
         AddDicomDirStudyToEnd(begin,end);
         break;
      case gdcmDicomDir::GDCM_DICOMDIR_SERIE:
         AddDicomDirSerieToEnd(begin,end);
         break;
      case gdcmDicomDir::GDCM_DICOMDIR_IMAGE:
         AddDicomDirImageToEnd(begin,end);
         break;
			case gdcmDicomDir::GDCM_DICOMDIR_NONE:
				 AddDicomDirImageToEnd(begin,end);	//FIXME
				 break;
   }
}

/**
 * \ingroup gdcmDicomDir
 * \brief Well ... Not realy to end, there is only one occurence  
 * @param   begin iterator on the first HeaderEntry within the chained List
 * @param   end iterator on the last HeaderEntry within the chained List
*/
void gdcmDicomDir::AddDicomDirMetaToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   if(metaElems)
      delete metaElems;
   metaElems = new gdcmDicomDirMeta(begin,end,&tagHT,&listEntries);
}

/**
 * \ingroup gdcmDicomDir
 * \brief  AddDicomDirPatientToEnd 
 * @param   begin iterator on the first HeaderEntry within the chained List
 * @param   end iterator on the last HeaderEntry within the chained List
*/
void gdcmDicomDir::AddDicomDirPatientToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   patients.push_back(new gdcmDicomDirPatient(begin,end,&tagHT, &listEntries));
}

/**
 * \ingroup gdcmDicomDir
 * \brief  AddDicomDirStudyToEnd 
 * @param   begin iterator on the first HeaderEntry within the chained List
 * @param   end iterator on the last HeaderEntry within the chained List
 */
 void gdcmDicomDir::AddDicomDirStudyToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   if(patients.size()>0)
   {
      ListDicomDirPatient::iterator itp=patients.end();
      itp--;
     (*itp)->AddDicomDirStudy(new gdcmDicomDirStudy(begin,end,&tagHT, &listEntries));
   }
}
/**
 * \ingroup gdcmDicomDir
 * \brief  AddDicomDirSerieToEnd 
 * @param   begin iterator on the first HeaderEntry within the chained List
 * @param   end iterator on the last HeaderEntry within the chained List
 */
void gdcmDicomDir::AddDicomDirSerieToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   if(patients.size()>0)
   {
      ListDicomDirPatient::iterator itp=patients.end();
      itp--;

      if((*itp)->GetDicomDirStudies().size()>0)
      {
         ListDicomDirStudy::iterator itst=(*itp)->GetDicomDirStudies().end();
         itst--;
        (*itst)->AddDicomDirSerie(new gdcmDicomDirSerie(begin,end,&tagHT, &listEntries));
      }
   }
}

/**
 * \ingroup gdcmDicomDir
 * \brief   AddDicomDirImageToEnd
 * @param   begin iterator on the first HeaderEntry within the chained List
 * @param   end iterator on the last HeaderEntry within the chained List
 */
 void gdcmDicomDir::AddDicomDirImageToEnd(ListTag::iterator begin,ListTag::iterator end)
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
           (*its)->AddDicomDirImage(new gdcmDicomDirImage(begin,end,&tagHT, &listEntries));
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
void gdcmDicomDir::SetElements(std::string &path, ListHeader &list)
{
   std::string patPrevName="",         patPrevID="";
   std::string studPrevInstanceUID="", studPrevID="";
   std::string serPrevInstanceUID="",  serPrevID="";

   std::string patCurName,         patCurID;
   std::string studCurInstanceUID, studCurID;
   std::string serCurInstanceUID,  serCurID;

   SetElement(path,GDCM_DICOMDIR_META,NULL);

   ListTag::iterator debPat=listEntries.begin();
   for(ListHeader::iterator it=list.begin();it!=list.end();++it) 
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
 * \brief   adds to the HTable and at the end of the Chained List
 *          the gdcmEntries (Dicom Elements) corresponding to the given type
 * @param   path full path file name(only used when type = GDCM_DICOMDIR_IMAGE
 * @param   type gdcmObject type to create (GDCM_DICOMDIR_PATIENT, GDCM_DICOMDIR_STUDY, GDCM_DICOMDIR_SERIE ...)
 * @param   header gdcmHeader of the current file
 */
void gdcmDicomDir::SetElement(std::string &path,gdcmDicomDirType type,gdcmHeader *header)
{
   std::list<gdcmElement> elemList;
   std::list<gdcmElement>::iterator it;
   guint16 tmpGr, tmpEl;
   gdcmDictEntry *dictEntry;
   gdcmHeaderEntry *entry;
   std::string val;

   switch(type)
   {
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

   for(it=elemList.begin();it!=elemList.end();++it)
   {
      tmpGr=it->group;
      tmpEl=it->elem;
      dictEntry=GetPubDict()->GetDictEntryByNumber(tmpGr,tmpEl);
      entry=new gdcmHeaderEntry(dictEntry);
      entry->SetOffset(0); // just to avoid missprinting

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
               dbg.Verbose(0, "gdcmDicomDir::SetElement : the base path of file name is incorrect");
               val=header->GetFileName();
            }
            else {
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
      }
      //AddHeaderEntry(entry); // both in H Table and in chained list
      tagHT.insert( PairHT( entry->GetKey(),entry) );
      listEntries.push_back(entry);  
      //wasUpdated = 1;	// is private
   }     
}
/**
 * \ingroup gdcmDicomDir
 * \brief   compares two dgcmHeaders
 */
bool gdcmDicomDir::HeaderLessThan(gdcmHeader *header1,gdcmHeader *header2)
{
   return(*header1<*header2);
}

/**
 * \ingroup gdcmDicomDir
 * \brief   Sets the accurate value for the (0x0004,0x1220) element of a DICOMDIR
 */

void gdcmDicomDir::UpdateDirectoryRecordSequenceLength() {
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
}

//-----------------------------------------------------------------------------
