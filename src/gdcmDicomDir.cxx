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
gdcmDicomDir::gdcmDicomDir(bool exception_on_error):                           
   gdcmParser(exception_on_error )  
{    

}



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

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
void gdcmDicomDir::CreateDicomDir(void)
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

   begin=GetListEntry().begin();
   end=begin;
   for(ListTag::iterator i=GetListEntry().begin();i != GetListEntry().end();++i) 
   {
      // std::cout << std::hex << (*i)->GetGroup() << 
      //                  " " << (*i)->GetElement() << endl;

      std::string v = (*i)->GetValue();
      if (v == "PATIENT ") 
      {
//         std::cout<<"PATIENT"<<std::endl;
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_PATIENT;
         begin=end;
      }	

      if (v == "STUDY ")
      {
//         std::cout<<"STUDY"<<std::endl;
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_STUDY;
         begin=end;
      }

      if (v == "SERIES") 
      {
//         std::cout<<"SERIES"<<std::endl;
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_SERIE;
         begin=end;
      }

      if (v == "IMAGE ") 
      {
//         std::cout<<"IMAGE"<<std::endl;
         end=i;
         AddObjectToEnd(type,begin,end);

         type=gdcmDicomDir::GDCM_IMAGE;
         begin=end;
      }
   }

   end=GetListEntry().end();
   AddObjectToEnd(type,begin,end);
}

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

void gdcmDicomDir::AddPatientToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   patients.push_back(new gdcmPatient(begin,end));
}

void gdcmDicomDir::AddStudyToEnd(ListTag::iterator begin,ListTag::iterator end)
{
   if(patients.size()>0)
   {
      ListPatient::iterator itp=patients.end();
      itp--;
      (*itp)->AddStudy(new gdcmStudy(begin,end));
   }
}

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
