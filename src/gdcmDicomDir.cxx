// gdcmDicomDir.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDir.h"
#include "gdcmStudy.h"
#include "gdcmSerie.h"
#include "gdcmImage.h"
#include "gdcmDirList.h"
#include "gdcmUtil.h"

#include <string>

#include <sys/types.h>
#include <errno.h>

//-----------------------------------------------------------------------------
class ELEMENTS 
{
public :
   ELEMENTS(unsigned short int _group,unsigned short int _elem,std::string _value)
      {group=_group;elem=_elem;value=_value;}

	unsigned short int group;
	unsigned short int elem;
	std::string value;
};

ELEMENTS metaElem[]={
       //Meta Group Length : to be computed later
   ELEMENTS(0x0002,0x0000,"12345"),
      //File Meta Information Version
   ELEMENTS(0x0002,0x0001,"\2\0\0\0"),
      //Media Stored SOP Class UID i.e. : 'Media Storage Directory Storage'
   ELEMENTS(0x0002,0x0002,"1.2.840.10008.1.3.10"),
      //Media Stored SOP Instance UID  : may be forged later 
   ELEMENTS(0x0002,0x0003,""), 
      //Transfer Syntax UID i.e. : Explicit VR - Little Endian
   ELEMENTS(0x0002,0x0010,"1.2.840.10008.1.2.1"), 
      //Implementation Class UID : may be forged later 
   ELEMENTS(0x0002,0x0012,""), 
      //Implementation Version Name  
   ELEMENTS(0x0002,0x0013,"gdcmLib"),
      //File-set ID  :        
   ELEMENTS(0x0004,0x1130,""),
      //Offset of the first dir of root dir entity : to be computed later   
   ELEMENTS(0x0004,0x1200,"0"),
      //Offset of the last dir of root dir entity : to be computed later   
   ELEMENTS(0x0004,0x1202,"0"),
      //File-set consistency flag   
   ELEMENTS(0x0004,0x1212,"0"),
       //Directory record sequence : *length* to be set later 
   ELEMENTS(0x0004,0x1220,"0"),           
   ELEMENTS(0xffff,0xffff,"") 
} ;

ELEMENTS patientElem[]={
   ELEMENTS(0xfffe,0xe000,"0"),
      // Offset of next directory record : to be computed later
   ELEMENTS(0x0004,0x1400,"0"),
      // Record in use flag : 65535(?)
   ELEMENTS(0x0004,0x1410,"65535"), 
      // Offset of referenced lower-level dir entity : to be computed later   
   ELEMENTS(0x0004,0x1420,"0"),
      // Directory Record Type      
   ELEMENTS(0x0004,0x1430,"PATIENT "), // don't remove trailing space !

      // Specific Character Set
   ELEMENTS(0x0008,0x0005,"ISO_IR 100"),
      // Patient's Name 
   ELEMENTS(0x0010,0x0010,""),
      // Patient ID 
   ELEMENTS(0x0010,0x0020,""),
      // Patient's Birthdate
   ELEMENTS(0x0010,0x0030,""), 
      // Patient's Sex
   ELEMENTS(0x0010,0x0040,""),    
   ELEMENTS(0xffff,0xffff,"") 
 }; 

ELEMENTS studyElem[]={  
   ELEMENTS(0xfffe,0xe000,"0"),
      // Offset of next directory record : to be computed later
   ELEMENTS(0x0004,0x1400,"0"),
      // Record in use flag : 65535(?)
   ELEMENTS(0x0004,0x1410,"65535"), 
      // Offset of referenced lower-level dir entity : to be computed later   
   ELEMENTS(0x0004,0x1420,"0"),   
      // Directory Record Type      
   ELEMENTS(0x0004,0x1430,"STUDY "), // don't remove trailing space !   

      // Specific Character Set
   ELEMENTS(0x0008,0x0005,"ISO_IR 100"), 
      // Study Date 
   ELEMENTS(0x0008,0x0020,""),
      // Study Time
   ELEMENTS(0x0008,0x0030,""),
      // Accession Number
   ELEMENTS(0x0008,0x0050,""), 
      // Study Description
   ELEMENTS(0x0008,0x1030,""), 
      // Study Instance UID : may be forged later
   ELEMENTS(0x0020,0x000d,""), 
      // Study ID : may be forged later
   ELEMENTS(0x0020,0x0010,""),                   
   ELEMENTS(0xffff,0xffff,"") 
}; 


ELEMENTS serieElem[]={  
   ELEMENTS(0xfffe,0xe000,"0"),
      // Offset of next directory record : to be computed later
   ELEMENTS(0x0004,0x1400,"0"),
      // Record in use flag : 65535(?)
   ELEMENTS(0x0004,0x1410,"65535"), 
      // Offset of referenced lower-level dir entity : to be computed later   
   ELEMENTS(0x0004,0x1420,"0"),   
      // Directory Record Type      
   ELEMENTS(0x0004,0x1430,"SERIES"), // don't add trailing space !   

      // Specific Character Set
   ELEMENTS(0x0008,0x0005,"ISO_IR 100"), 
      // Series Date
   ELEMENTS(0x0008,0x0021,""),
      // Series Time
   ELEMENTS(0x0008,0x0031,""),
      // Modality
   ELEMENTS(0x0008,0x0060,""), 
      // Institution Name  : may be forged later
   ELEMENTS(0x0008,0x0080,""), 
      // Institution Address : may be forged later
   ELEMENTS(0x0008,0x0081,""), 
      // Series Description :  may be forged later
   ELEMENTS(0x0008,0x103e,""),
      // Series Instance UID : may be forged later
   ELEMENTS(0x0020,0x000e,""),   
      // Series Number : may be forged later
   ELEMENTS(0x0020,0x0011,"0"),                         
   ELEMENTS(0xffff,0xffff,"") 
}; 

ELEMENTS imageElem[]={  
   ELEMENTS(0xfffe,0xe000,"0"),
      // Offset of next directory record : to be computed later
   ELEMENTS(0x0004,0x1400,"0"),
      // Record in use flag : 65535(?)
   ELEMENTS(0x0004,0x1410,"65535"), 
      // Offset of referenced lower-level dir entity : to be computed later   
   ELEMENTS(0x0004,0x1420,"0"),   
      // Directory Record Type      
   ELEMENTS(0x0004,0x1430,"IMAGE "), // don't remove trailing space ! 

      // Referenced File ID : to be set later(relative File Name)
   ELEMENTS(0x0004,0x1500,""),
      // Referenced SOP Class UID in File : may be forged later
   ELEMENTS(0x0004,0x1510,""),
      // Referenced SOP Class UID in File :  may be forged later
   ELEMENTS(0x0004,0x1511,""),
      // Referenced Transfer Syntax in File
   ELEMENTS(0x0004,0x1512,""),      
      // Specific Character Set
   ELEMENTS(0x0008,0x0005,"ISO_IR 100"), 
      // Image Type
   ELEMENTS(0x0008,0x0008,""), 
      // SOP Class UID : to be set/forged later
   ELEMENTS(0x0008,0x0016,""),
      // SOP Instance UID : to be set/forged later
   ELEMENTS(0x0008,0x0018,""),    
      // Content Date
   ELEMENTS(0x0008,0x0023,""),
      // Content Time
   ELEMENTS(0x0008,0x0033,""),      
      // Referenced Image Sequence : to be set/forged later
   ELEMENTS(0x0008,0x1040,""), 
   ELEMENTS(0xfffe,0xe000,"0"),
      // Referenced SOP Class UID : to be set/forged later
   ELEMENTS(0x0008,0x1150,""), 
      // Referenced SOP Instance UID : to be set/forged later
   ELEMENTS(0x0008,0x1155,""),      
      // Image Number 
   ELEMENTS(0x0020,0x0013,"0"),
      // Image Position Patient 
   ELEMENTS(0x0020,0x0032,"0"),   
      // Image Orientation(Patient)
   ELEMENTS(0x0020,0x0037,"0"),   
      // Frame of Reference UID
   ELEMENTS(0x0020,0x0052,"0"), 
      // Rows
   ELEMENTS(0x0028,0x0010,"0"),   
      // Columns
   ELEMENTS(0x0028,0x0011,"0"),
      // Pixel Spacing
   ELEMENTS(0x0028,0x0030,"0"),   
      // Calibration Image
   ELEMENTS(0x0050,0x0004,"0"),                                    
   ELEMENTS(0xffff,0xffff,"") 
}; 

//-----------------------------------------------------------------------------
// Constructor / Destructor
/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   Filename
 * @param   exception_on_error
 */
gdcmDicomDir::gdcmDicomDir(const char *FileName,
                           bool exception_on_error):
   gdcmParser(FileName,exception_on_error,true)
{
   if( GetListEntry().begin()==GetListEntry().end() ) 
   {
      dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : entry list empty");
      dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir : Parse directory and create the DicomDir");

      std::string path=FileName;
      std::string file;

      int pos1=path.rfind("/");
      int pos2=path.rfind("\\");
      if(pos1>pos2)
         path.resize(pos1);
      else
         path.resize(pos2);
      NewDicomDir(path);
   }

   CreateDicomDir();
}

/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   exception_on_error
 */
gdcmDicomDir::gdcmDicomDir(ListTag *l,
                           bool exception_on_error):                           
   gdcmParser(exception_on_error )  
{    
   listEntries=*l;
   CreateDicomDir();
}

/*
 * \ingroup gdcmDicomDir
 * \brief  Canonical destructor 
 */
gdcmDicomDir::~gdcmDicomDir() 
{
   for(ListPatient::iterator cc=patients.begin();cc!=patients.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
void gdcmDicomDir::Print(std::ostream &os)
{
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
 * \         it's up to the user doing it !
 * @param  fileName file to be written to 
 * @return
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

//-----------------------------------------------------------------------------
// Protected
/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   
 */
void gdcmDicomDir::NewDicomDir(std::string path)
{
   gdcmDirList fileList(path,1);
   ListHeader list;
   gdcmHeader *header;

   listEntries.clear();

   for(gdcmDirList::iterator it=fileList.begin(); 
       it!=fileList.end(); ++it) 
   {
//      std::cout<<*it<<std::endl;
      header=new gdcmHeader(it->c_str());
      if(header->IsReadable())
         list.push_back(header);
      else
         delete header;
   }

   SetElements(path,list);
}

//-----------------------------------------------------------------------------
// Private
/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   
 */
void gdcmDicomDir::CreateDicomDir()
{
   // The list is parsed. When a tag is found :
   //  1 - we save the beginning iterator
   //  2 - we continue to parse
   //  3 - we find an other tag
   //       + we create the object for the precedent tag
   //       + loop to 1 -

   gdcmDicomDirType type=gdcmDicomDir::GDCM_NONE;
   ListTag::iterator begin;
   ListTag::iterator end;

   begin=listEntries.begin();
   end=begin;
   for(ListTag::iterator i=listEntries.begin();i !=listEntries.end();++i) 
   {
      // std::cout << std::hex <<(*i)->GetGroup() << 
      //                  " " <<(*i)->GetElement() << endl;

      std::string v=(*i)->GetValue();
      if(v=="PATIENT ") 
      {
       //  std::cout<<"PATIENT"<<std::endl;
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_PATIENT;
         begin=end;
      }	

      if(v=="STUDY ")
      {
       //  std::cout<<"STUDY"<<std::endl;
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_STUDY;
         begin=end;
      }

      if(v=="SERIES") 
      {
       //  std::cout<<"SERIES"<<std::endl;
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_SERIE;
         begin=end;
      }

      if(v=="IMAGE ") 
      {
       //  std::cout<<"IMAGE"<<std::endl;
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_IMAGE;
         begin=end;
      }
   }

   end=GetListEntry().end();
   AddObjectToEnd(type,begin,end);
}
/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   
 */
void gdcmDicomDir::AddObjectToEnd(gdcmDicomDirType type,ListTag::iterator begin,ListTag::iterator end)
{
   if(begin==end)
      return;

   switch(type)
   {
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
 * \brief   
 * @param   
 */
void gdcmDicomDir::AddPatientToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   patients.push_back(new gdcmPatient(begin,end));
}

/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   
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
 * @param   
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
 * \brief   
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
 * @param   
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
      {
         SetElement(path,GDCM_SERIE,*it);
      } 
      
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
 * @param   
 */
void gdcmDicomDir::SetElement(std::string &path,gdcmDicomDirType type,gdcmHeader *header)
{
   ELEMENTS *elemList;
   guint16 tmpGr, tmpEl;
   gdcmDictEntry *dictEntry;
   gdcmHeaderEntry *entry;
   std::string val;

   switch(type)
   {
      case GDCM_PATIENT:
         elemList=patientElem;
         break;
      case GDCM_STUDY:
         elemList=studyElem;
         break;
      case GDCM_SERIE:
         elemList=serieElem;
         break;
      case GDCM_IMAGE:
         elemList=imageElem;
         break;
      case GDCM_NONE:
         elemList=metaElem;
         break;
      default:
         return;
   }

   for(int i=0;;i++)
   {
      tmpGr=elemList[i].group;
      tmpEl=elemList[i].elem;
      if(tmpGr==0xffff) 
         break;

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
            val=elemList[i].value;
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

//-----------------------------------------------------------------------------
