// gdcmDicomDir.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDir.h"
#include "gdcmStudy.h"
#include "gdcmSerie.h"
#include "gdcmImage.h"
#include "gdcmDirList.h"
#include "gdcmUtil.h"

#include <string>
#include <algorithm>

#include <sys/types.h>
#include <errno.h>
//-----------------------------------------------------------------------------
//  For full DICOMDIR description, see:
//  PS 3.3-2003, pages 731-750
//-----------------------------------------------------------------------------
// Constructor / Destructor
/*
 * \ingroup gdcmDicomDir
 * \brief   Constructor
 * @param   Filename
 * @param   exception_on_error
 */
gdcmDicomDir::gdcmDicomDir(const char *FileName, bool parseDir,
                           bool exception_on_error):
   gdcmParser(FileName,exception_on_error,true)
{
   metaElems=NULL;

   if( GetListEntry().begin()==GetListEntry().end() ) 
   {
      dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : entry list empty");

      if(parseDir)
      {
         dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : Parse directory and create the DicomDir");
         ParseDirectory();
      }
   }
   else
      CreateDicomDir();
}

/*
 * \ingroup gdcmDicomDir
 * \brief  Canonical destructor 
 */
gdcmDicomDir::~gdcmDicomDir() 
{
   if(metaElems)
      delete metaElems;
   
   for(ListPatient::iterator cc=patients.begin();cc!=patients.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/*
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
   
   for(ListPatient::iterator cc=patients.begin();cc!=patients.end();++cc)
   {
     (*cc)->SetPrintLevel(printLevel);
     (*cc)->Print(os);
   }
}

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup gdcmDicomDir
 * \brief   writes on disc a DICOMDIR
 * \ warning does NOT add the missing elements in the header :
 *           it's up to the user doing it !
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

   char * filePreamble;
   filePreamble=(char*)calloc(128,1);
   fwrite(filePreamble,128,1,fp1);
   fwrite("DICM",4,1,fp1);
   free(filePreamble);

   WriteEntries(fp1,DICOMDIR);

   fclose(fp1);

   return true;
}

/*
 * \ingroup gdcmDicomDir
 * \brief  fills whole the structure
 */
void gdcmDicomDir::ParseDirectory(void)
{
   NewDicomDir(GetPath());
   CreateDicomDir();
}

//-----------------------------------------------------------------------------
// Protected
/*
 * \ingroup gdcmDicomDir
 * \brief create a gdcmDicomDir from a root Directory 
 * @param path entry point of the stree-like structure
 */
void gdcmDicomDir::NewDicomDir(std::string path)
{
   gdcmDirList fileList(path,1);
   ListHeader list;
   gdcmHeader *header;

   listEntries.clear();
   patients.clear();

   for(gdcmDirList::iterator it=fileList.begin(); 
       it!=fileList.end(); ++it) 
   {
      header=new gdcmHeader(it->c_str());
      if(header->IsReadable())
         list.push_back(header);
      else
         delete header;
   }

   std::sort(list.begin(),list.end(),gdcmDicomDir::HeaderLessThan);

   std::string tmp=fileList.GetDirName();
   SetElements(tmp,list);
}

/*
 * \ingroup gdcmDicomDir
 * \brief   Get the DicomDir path
 * @param   
 */
std::string gdcmDicomDir::GetPath(void)
{
   std::string path=GetFileName();

   int pos1=path.rfind("/");
   int pos2=path.rfind("\\");
   if(pos1>pos2)
      path.resize(pos1);
   else
      path.resize(pos2);

   return(path);
}

//-----------------------------------------------------------------------------
// Private
/*
 * \ingroup gdcmDicomDir
 * \brief create a 'gdcmDicomDir' from a DICOMDIR gdcmHeader 
 */
void gdcmDicomDir::CreateDicomDir()
{
   // The list is parsed. When a tag is found :
   //  1 - we save the beginning iterator
   //  2 - we continue to parse
   //  3 - we find an other tag
   //       + we create the object for the precedent tag
   //       + loop to 1 -

   gdcmDicomDirType type=gdcmDicomDir::GDCM_META;
   ListTag::iterator begin;
   ListTag::iterator end;

   begin=listEntries.begin();
   end=begin;
   for(ListTag::iterator i=end;i !=listEntries.end();++i) 
   {
      std::string v=(*i)->GetValue();
      if(v=="PATIENT ") 
      {
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_PATIENT;
         begin=end;
      }	

      if(v=="STUDY ")
      {
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_STUDY;
         begin=end;
      }

      if(v=="SERIES") 
      {
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_SERIE;
         begin=end;
      }

      if(v=="IMAGE ") 
      {
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_IMAGE;
         begin=end;
      }
   }

   if(begin!=end)
   {
      end=GetListEntry().end();
      AddObjectToEnd(type,begin,end);
   }
}
/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   type
 * @param   begin
 * @param   end
 */
void gdcmDicomDir::AddObjectToEnd(gdcmDicomDirType type,ListTag::iterator begin,ListTag::iterator end)
{
   if(begin==end)
      return;

   switch(type)
   {
      case gdcmDicomDir::GDCM_META:
         AddMetaToEnd(begin,end);
         break;      
      case gdcmDicomDir::GDCM_PATIENT:
         AddPatientToEnd(begin,end);
         break;
      case gdcmDicomDir::GDCM_STUDY:
         AddStudyToEnd(begin,end);
         break;
      case gdcmDicomDir::GDCM_SERIE:
         AddSerieToEnd(begin,end);
         break;
      case gdcmDicomDir::GDCM_IMAGE:
         AddImageToEnd(begin,end);
         break;
   }
}

/*
 * \ingroup gdcmDicomDir
 * \brief Well ... Not realy to end, there is only one occurence  
 * @param   begin
 * @param   end
*/
void gdcmDicomDir::AddMetaToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   if(metaElems)
      delete metaElems;
   metaElems = new gdcmMeta(begin,end);
}

/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   begin
 * @param   end
*/
void gdcmDicomDir::AddPatientToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   patients.push_back(new gdcmPatient(begin,end));
}

/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   begin
 * @param   end
 */
 void gdcmDicomDir::AddStudyToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   if(patients.size()>0)
   {
      ListPatient::iterator itp=patients.end();
      itp--;
     (*itp)->AddStudy(new gdcmStudy(begin,end));
   }
}
/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   begin
 * @param   end
 */
void gdcmDicomDir::AddSerieToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   if(patients.size()>0)
   {
      ListPatient::iterator itp=patients.end();
      itp--;

      if((*itp)->GetStudies().size()>0)
      {
         ListStudy::iterator itst=(*itp)->GetStudies().end();
         itst--;
        (*itst)->AddSerie(new gdcmSerie(begin,end));
      }
   }
}

/*
 * \ingroup gdcmDicomDir
 * @param   begin
 * @param   end
 * @param   
 */
 void gdcmDicomDir::AddImageToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   if(patients.size()>0)
   {
      ListPatient::iterator itp=patients.end();
      itp--;

      if((*itp)->GetStudies().size()>0)
      {
         ListStudy::iterator itst=(*itp)->GetStudies().end();
         itst--;

         if((*itst)->GetSeries().size()>0)
         {
            ListSerie::iterator its=(*itst)->GetSeries().end();
            its--;
           (*its)->AddImage(new gdcmImage(begin,end));
         }
      }
   }
}

/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   path
 * @param   list
 */
void gdcmDicomDir::SetElements(std::string &path,ListHeader &list)
{
   std::string patPrevName="", patPrevID="";
   std::string studPrevInstanceUID="", studPrevID="";
   std::string serPrevInstanceUID="", serPrevID="";

   std::string patCurName, patCurID;
   std::string studCurInstanceUID, studCurID;
   std::string serCurInstanceUID, serCurID;

   SetElement(path,GDCM_NONE,NULL);

   ListTag::iterator debPat=listEntries.begin();
   for(ListHeader::iterator it=list.begin();it!=list.end();++it) 
   {
      // get the current file characteristics
      patCurName=(*it)->GetEntryByNumber(0x0010,0x0010); 
      patCurID=(*it)->GetEntryByNumber(0x0010,0x0011); 
      studCurInstanceUID=(*it)->GetEntryByNumber(0x0020,0x000d);            
      studCurID=(*it)->GetEntryByNumber(0x0020,0x0010);            
      serCurInstanceUID=(*it)->GetEntryByNumber(0x0020,0x000e);            
      serCurID=(*it)->GetEntryByNumber(0x0020,0x0011);

      if(patCurName!=patPrevName || patCurID!=patPrevID) 
         SetElement(path,GDCM_PATIENT,*it);

      // if new Study Deal with 'STUDY' Elements   
      if(studCurInstanceUID!=studPrevInstanceUID || studCurID!=studPrevID) 
         SetElement(path,GDCM_STUDY,*it);

      // if new Serie Deal with 'SERIE' Elements   
      if(serCurInstanceUID!=serPrevInstanceUID || serCurID!=serPrevID) 
         SetElement(path,GDCM_SERIE,*it);
      
      // Always Deal with 'IMAGE' Elements  
      SetElement(path,GDCM_IMAGE,*it);

      patPrevName=patCurName;
      patPrevID=patCurID;
      studPrevInstanceUID=studCurInstanceUID;
      studPrevID=studCurID;
      serPrevInstanceUID=serCurInstanceUID;
      serPrevID=serCurID;
   }
}

/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   path
 * @param   type
 * @param   header
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
      case GDCM_PATIENT:
         elemList=gdcmGlobal::GetDicomDirElements()->GetPatientElements();
         break;
      case GDCM_STUDY:
         elemList=gdcmGlobal::GetDicomDirElements()->GetStudyElements();
         break;
      case GDCM_SERIE:
         elemList=gdcmGlobal::GetDicomDirElements()->GetSerieElements();
         break;
      case GDCM_IMAGE:
         elemList=gdcmGlobal::GetDicomDirElements()->GetImageElements();
         break;
      case GDCM_NONE:
         elemList=gdcmGlobal::GetDicomDirElements()->GetMetaElements();
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
         if((tmpGr==0x0004) &&(tmpEl==0x1130) )
         {
            // TODO force the *end* File Name(remove path)
            val=path;
         }
         else if( (tmpGr==0x0004) && (tmpEl==0x1500) ) // Only used for image
         {
            if(header->GetFileName().substr(0,path.length())!=path)
            {
               dbg.Verbose(0, "gdcmDicomDir::SetElement : the base path of file name is incorrect");
               val=header->GetFileName();
            }
            else
               val=&(header->GetFileName()[path.length()]);
         }
         else
         {
            val=it->value;
         }
      }
      entry->SetValue(val);

      if(dictEntry)
      {
         if( (dictEntry->GetVR()=="UL") || (dictEntry->GetVR()=="SL") ) 
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

      listEntries.push_back(entry);
   }     
}

bool gdcmDicomDir::HeaderLessThan(gdcmHeader *header1,gdcmHeader *header2)
{
   return(*header1<*header2);
}

//-----------------------------------------------------------------------------
