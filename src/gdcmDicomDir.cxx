// gdcmDicomDir.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDir.h"
#include "gdcmStudy.h"
#include "gdcmSerie.h"
#include "gdcmImage.h"
#include "gdcmUtil.h"

#include <string>

//-----------------------------------------------------------------------------
// Constructor / Destructor

/*
 * \ingroup gdcmDicomDir
 * \brief   
 * @param   Filename
 * @param   exception_on_error
 */
gdcmDicomDir::gdcmDicomDir(std::string & FileName,
                           bool exception_on_error):
   gdcmParser(FileName.c_str(),exception_on_error, true )  
{
   if ( GetListEntry().begin() == GetListEntry().end() ) 
   {
      dbg.Verbose(0, "gdcmDicomDir::gdcmDicomDir entry list empty");
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
   for(ListPatient::iterator cc = patients.begin();cc!=patients.end();++cc)
   {
      delete *cc;
   }
}



//-----------------------------------------------------------------------------
// Print
void gdcmDicomDir::Print(std::ostream &os)
{
   for(ListPatient::iterator cc = patients.begin();cc!=patients.end();++cc)
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
bool gdcmDicomDir::Write(std::string fileName) {

   FILE * fp1;
   fp1 = fopen(fileName.c_str(),"wb");
   if (fp1 == NULL) {
      printf("Failed to open (write) File [%s] \n",fileName.c_str());
      return (false);
   }
   char * filePreamble;  
   filePreamble=(char*)calloc(128,1);
   fwrite(filePreamble,128,1,fp1);
   fwrite("DICM",4,1,fp1);
   free (filePreamble);
   WriteEntries(DICOMDIR,fp1);
    
   return true;

}

//-----------------------------------------------------------------------------
// Protected

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
   for(ListTag::iterator i=listEntries.begin();i != listEntries.end();++i) 
   {
      // std::cout << std::hex << (*i)->GetGroup() << 
      //                  " " << (*i)->GetElement() << endl;

      std::string v = (*i)->GetValue();
      if (v == "PATIENT ") 
      {
       //  std::cout<<"PATIENT"<<std::endl;
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_PATIENT;
         begin=end;
      }	

      if (v == "STUDY ")
      {
       //  std::cout<<"STUDY"<<std::endl;
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_STUDY;
         begin=end;
      }

      if (v == "SERIES") 
      {
       //  std::cout<<"SERIES"<<std::endl;
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_SERIE;
         begin=end;
      }

      if (v == "IMAGE ") 
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

//-----------------------------------------------------------------------------
