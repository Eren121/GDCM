/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirElement.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/10 00:42:54 $
  Version:   $Revision: 1.19 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDicomDirElement.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"
#include "gdcmDictSet.h"

#include <fstream>
#include <iostream>

//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \brief   constructor : populates the chained lists 
 *          from the file 'Dicts/DicomDir.dic'
 */
gdcmDicomDirElement::gdcmDicomDirElement()
{
   std::string filename = gdcmDictSet::BuildDictPath() + std::string(DICT_ELEM);
   std::ifstream from(filename.c_str());
   dbg.Error(!from, "gdcmDicomDirElement::gdcmDicomDirElement: can't open dictionary",
              filename.c_str());

   char buff[1024];
   std::string type;
   gdcmElement elem;

   while (!from.eof())
   {
      from >> std::ws;
      from.getline(buff, 1024, ' ');
      type = buff;

      if( (type=="metaElem")  || (type=="patientElem") || 
          (type=="studyElem") || (type=="serieElem")   || 
          (type=="imageElem") )
      {
         from >> std::hex >> elem.Group >> elem.Elem;

         from >> std::ws;
         from.getline(buff, 1024, '"');
         from >> std::ws;
         from.getline(buff, 1024, '"');
         elem.Value = buff;

         if( type == "metaElem" )
         {
            DicomDirMetaList.push_back(elem);
         }
         else if( type == "patientElem" )
         {
            DicomDirPatientList.push_back(elem);
         }
         else if( type == "studyElem" )
         {
            DicomDirStudyList.push_back(elem);
         }
         else if( type == "serieElem" )
         {
            DicomDirSerieList.push_back(elem);
         }
         else if( type == "imageElem" )
         {
            DicomDirImageList.push_back(elem);
         }
      }
      from.getline(buff, 1024, '\n');
   }
   from.close();
}

/**
 * \ingroup gdcmDicomDirElement
 * \brief   canonical destructor 
 */
gdcmDicomDirElement::~gdcmDicomDirElement()
{
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
void gdcmDicomDirElement::Print(std::ostream &os)
{
   std::ostringstream s;
   std::list<gdcmElement>::iterator it;
   //char greltag[10];  //group element tag
   std::string greltag;

   s << "Meta Elements :"<<std::endl;
   for (it = DicomDirMetaList.begin(); it != DicomDirMetaList.end(); ++it)
   {
      greltag = gdcmUtil::Format("%04x|%04x ",it->Group,it->Elem);
      s << "   (" << greltag << ") = " << it->Value << std::endl;
   }

   s << "Patient Elements :"<<std::endl;
   for (it = DicomDirPatientList.begin(); it != DicomDirPatientList.end(); ++it)
   {
      greltag = gdcmUtil::Format("%04x|%04x ",it->Group,it->Elem);
      s << "   (" << greltag << ") = " << it->Value << std::endl;
   }

   s << "Study Elements :"<<std::endl;
   for (it = DicomDirStudyList.begin(); it != DicomDirStudyList.end(); ++it)
   {
      greltag = gdcmUtil::Format("%04x|%04x ", it->Group, it->Elem);
      s << "   (" << greltag << ") = " << it->Value << std::endl;
   }

   s << "Serie Elements :"<<std::endl;
   for (it = DicomDirSerieList.begin(); it != DicomDirSerieList.end(); ++it)
   {
      greltag = gdcmUtil::Format("%04x|%04x ", it->Group, it->Elem);
      s << "   (" << greltag << ") = " << it->Value << std::endl;
   }

   s << "Image Elements :"<<std::endl;
   for (it = DicomDirImageList.begin(); it != DicomDirImageList.end(); ++it)
   {
      greltag = gdcmUtil::Format("%04x|%04x ", it->Group, it->Elem);
      s << "   (" << greltag << ") = " << it->Value << std::endl;
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
