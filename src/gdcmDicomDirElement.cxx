// gdcmDicomDirElement.cxx
//-----------------------------------------------------------------------------
#include <fstream>
#include <stdio.h>    // For sprintf

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

/**
 * \ingroup gdcmDicomDirElement
 * \brief   constructor 
 */
 gdcmDicomDirElement::gdcmDicomDirElement(void) {
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

      if( (type=="metaElem")  || (type=="patientElem") || 
          (type=="studyElem") || (type=="serieElem")   || 
          (type=="imageElem") )
      {
         from >> std::hex >> elem.group >> elem.elem;

         eatwhite(from);
         from.getline(buff, 1024, '"');
         eatwhite(from);
         from.getline(buff, 1024, '"');
         elem.value = buff;

         if(type=="metaElem")
             DicomDirMetaList.push_back(elem);
         else if(type=="patientElem")
            DicomDirPatientList.push_back(elem);
         else if(type=="studyElem")
            DicomDirStudyList.push_back(elem);
         else if(type=="serieElem")
            DicomDirSerieList.push_back(elem);
         else if(type=="imageElem")
            DicomDirImageList.push_back(elem);
      }
      from.getline(buff, 1024, '\n');
   }
   from.close();
}

/**
 * \ingroup gdcmDicomDirElement
 * \brief   destructor 
 */
 gdcmDicomDirElement::~gdcmDicomDirElement() {
   DicomDirMetaList.clear();
   DicomDirPatientList.clear();
   DicomDirStudyList.clear();
   DicomDirSerieList.clear();
   DicomDirImageList.clear();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDicomDirElement
 * \brief   Print all
 * \todo add a 'Print Level' check 
 * @param   os The output stream to be written to.
 */
void gdcmDicomDirElement::Print(std::ostream &os) {
   std::ostringstream s;
   std::list<gdcmElement>::iterator it;
   char greltag[10];  //group element tag

   s << "Meta Elements :"<<std::endl;
   for (it = DicomDirMetaList.begin();it!=DicomDirMetaList.end();++it)
   {
      sprintf(greltag,"%04x|%04x ",it->group,it->elem);
      s << "   ("<<greltag<<") = "<< it->value<<std::endl;
   }

   s << "Patient Elements :"<<std::endl;
   for (it = DicomDirPatientList.begin();it!=DicomDirPatientList.end();++it)
   {
      sprintf(greltag,"%04x|%04x ",it->group,it->elem);
      s << "   ("<<greltag<<") = "<< it->value<<std::endl;
   }

   s << "Study Elements :"<<std::endl;
   for (it = DicomDirStudyList.begin();it!=DicomDirStudyList.end();++it)
   {
      sprintf(greltag,"%04x|%04x ",it->group,it->elem);
      s << "   ("<<greltag<<") = "<< it->value<<std::endl;
   }

   s << "Serie Elements :"<<std::endl;
   for (it = DicomDirSerieList.begin();it!=DicomDirSerieList.end();++it)
   {
      sprintf(greltag,"%04x|%04x ",it->group,it->elem);
      s << "   ("<<greltag<<") = "<< it->value<<std::endl;
   }

   s << "Image Elements :"<<std::endl;
   for (it = DicomDirImageList.begin();it!=DicomDirImageList.end();++it)
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
