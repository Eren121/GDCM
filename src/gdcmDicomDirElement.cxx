// gdcmDicomDirElement.cxx
//-----------------------------------------------------------------------------
#include <fstream>
#include <stdio.h>    // For sprintf
#include <iostream>

#include "gdcmDicomDirElement.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"
#include "gdcmDictSet.h"


/**
 * \ingroup gdcmDicomDirElement
 * \brief   Class for the chained lists from the file 'Dicts/DicomDir.dic'
 */
 
//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup gdcmDicomDirElement
 * \brief   constructor : populates the chained lists 
 *          from the file 'Dicts/DicomDir.dic'
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
 * \brief   canonical destructor 
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
