// gdcmDicomDirElement.cxx
//-----------------------------------------------------------------------------
#include <fstream>

#include "gdcmDicomDirElement.h"
#include "gdcmUtil.h"

#ifndef PUB_DICT_PATH
#  define PUB_DICT_PATH     "../Dicts/"
#endif
#define DICT_ELEM "DicomDir.dic"

#include <iostream>
#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif

//-----------------------------------------------------------------------------
// Constructor / Destructor
gdcmDicomDirElement::gdcmDicomDirElement(void) 
{
   std::string filename=gdcmDictSet::BuildDictPath() + std::string(DICT_ELEM);
   std::ifstream from(filename.c_str());
   dbg.Error(!from, "gdcmDicomDirElement::gdcmDicomDirElement: can't open dictionary",filename.c_str());

   char buff[1024];
   std::string type;
   gdcmElement elem;

   while (!from.eof()) {
      eatwhite(from);
      from.getline(buff, 1024, ' ');
      type = buff;

      if( (type=="metaElem") || (type=="patientElem") || 
          (type=="studyElem") || (type=="serieElem") || 
          (type=="imageElem") )
      {
         from >> std::hex >> elem.group >> elem.elem;

         eatwhite(from);
         from.getline(buff, 1024, '"');
         eatwhite(from);
         from.getline(buff, 1024, '"');
         elem.value = buff;

         if(type=="metaElem")
            MetaList.push_back(elem);
         else if(type=="patientElem")
            PatientList.push_back(elem);
         else if(type=="studyElem")
            StudyList.push_back(elem);
         else if(type=="serieElem")
            SerieList.push_back(elem);
         else if(type=="imageElem")
            ImageList.push_back(elem);
      }

      from.getline(buff, 1024, '\n');
   }
   from.close();
}

gdcmDicomDirElement::~gdcmDicomDirElement() 
{
   MetaList.clear();
   PatientList.clear();
   StudyList.clear();
   SerieList.clear();
   ImageList.clear();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmVR
 * \brief   Print all 
 * @param   os The output stream to be written to.
 */
void gdcmDicomDirElement::Print(std::ostream &os) 
{
   std::ostringstream s;
   std::list<gdcmElement>::iterator it;
   char greltag[10];  //group element tag

   s << "Meta Elements :"<<std::endl;
   for (it = MetaList.begin();it!=MetaList.end();++it)
   {
      sprintf(greltag,"%04x|%04x ",it->group,it->elem);
      s << "   ("<<greltag<<") = "<< it->value<<std::endl;
   }

   s << "Patient Elements :"<<std::endl;
   for (it = PatientList.begin();it!=PatientList.end();++it)
   {
      sprintf(greltag,"%04x|%04x ",it->group,it->elem);
      s << "   ("<<greltag<<") = "<< it->value<<std::endl;
   }

   s << "Study Elements :"<<std::endl;
   for (it = StudyList.begin();it!=StudyList.end();++it)
   {
      sprintf(greltag,"%04x|%04x ",it->group,it->elem);
      s << "   ("<<greltag<<") = "<< it->value<<std::endl;
   }

   s << "Serie Elements :"<<std::endl;
   for (it = SerieList.begin();it!=SerieList.end();++it)
   {
      sprintf(greltag,"%04x|%04x ",it->group,it->elem);
      s << "   ("<<greltag<<") = "<< it->value<<std::endl;
   }

   s << "Image Elements :"<<std::endl;
   for (it = ImageList.begin();it!=ImageList.end();++it)
   {
      sprintf(greltag,"%04x|%04x ",it->group,it->elem);
      s << "   ("<<greltag<<") = "<< it->value<<std::endl;
   }

   os << s.str();
}

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
