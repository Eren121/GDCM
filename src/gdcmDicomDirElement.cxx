/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirElement.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/24 14:14:10 $
  Version:   $Revision: 1.31 $
                                                                                
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

namespace gdcm 
{
void FillDefaultDIRDict(DicomDirElement *dde);
//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \brief   constructor : populates the chained lists 
 *          from the file 'Dicts/DicomDir.dic'
 */
DicomDirElement::DicomDirElement()
{
   std::string filename = DictSet::BuildDictPath() + DICT_ELEM;
   std::ifstream from(filename.c_str());
   if(!from)
   {
      gdcmVerboseMacro( "Can't open DicomDirElement dictionary" 
                        << filename.c_str());
      FillDefaultDIRDict( this );
   }
   else
   {
      char buff[1024];
      std::string strType;
      Element elem;
      DicomDirType type;

      while (!from.eof())
      {
         from >> std::ws;
         from.getline(buff, 1024, ' ');
         strType = buff;

         if( strType == "metaElem" )
            type = DD_META;
         else if( strType == "patientElem" )
            type = DD_PATIENT;
         else if( strType == "studyElem" )
            type = DD_STUDY;
         else if( strType == "serieElem" )
            type = DD_SERIE;
         else if( strType == "imageElem" )
            type = DD_IMAGE;
         else
         {
            gdcmVerboseMacro("Unknown type found in the file : "
                             <<filename.c_str());
            type = DD_UNKNOWN;
         }

         if( type!=DD_UNKNOWN )
         {
            from >> std::hex >> elem.Group >> elem.Elem;

            from >> std::ws;
            from.getline(buff, 1024, '"');
            from >> std::ws;
            from.getline(buff, 1024, '"');
            elem.Value = buff;

            AddEntry(type, elem);
         }
         from.getline(buff, 1024, '\n');
      }
      from.close();
   }
}

/**
 * \brief   canonical destructor 
 */
DicomDirElement::~DicomDirElement()
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
 * \brief   Print all
 * \todo add a 'Print Level' check 
 * @param   os The output stream to be written to.
 */
void DicomDirElement::Print(std::ostream &os)
{
   std::ostringstream s;
   std::list<Element>::iterator it;
   //char greltag[10];  //group element tag
   std::string greltag;

   s << "Meta Elements :"<<std::endl;
   for (it = DicomDirMetaList.begin(); it != DicomDirMetaList.end(); ++it)
   {
      greltag = Util::Format("%04x|%04x ",it->Group,it->Elem);
      s << "   (" << greltag << ") = " << it->Value << std::endl;
   }

   s << "Patient Elements :"<<std::endl;
   for (it = DicomDirPatientList.begin(); it != DicomDirPatientList.end(); ++it)
   {
      greltag = Util::Format("%04x|%04x ",it->Group,it->Elem);
      s << "   (" << greltag << ") = " << it->Value << std::endl;
   }

   s << "Study Elements :"<<std::endl;
   for (it = DicomDirStudyList.begin(); it != DicomDirStudyList.end(); ++it)
   {
      greltag = Util::Format("%04x|%04x ", it->Group, it->Elem);
      s << "   (" << greltag << ") = " << it->Value << std::endl;
   }

   s << "Serie Elements :"<<std::endl;
   for (it = DicomDirSerieList.begin(); it != DicomDirSerieList.end(); ++it)
   {
      greltag = Util::Format("%04x|%04x ", it->Group, it->Elem);
      s << "   (" << greltag << ") = " << it->Value << std::endl;
   }

   s << "Image Elements :"<<std::endl;
   for (it = DicomDirImageList.begin(); it != DicomDirImageList.end(); ++it)
   {
      greltag = Util::Format("%04x|%04x ", it->Group, it->Elem);
      s << "   (" << greltag << ") = " << it->Value << std::endl;
   }

   os << s.str();
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief Add an entry 
 * @param type type
 * @param elem elem
 */
bool DicomDirElement::AddEntry(DicomDirType type, Element const &elem)
{
   switch( type )
   {
      case DD_META :
         DicomDirMetaList.push_back(elem);
         break;
      case DD_PATIENT :
         DicomDirPatientList.push_back(elem);
         break;
      case DD_STUDY :
         DicomDirStudyList.push_back(elem);
         break;
      case DD_SERIE :
         DicomDirSerieList.push_back(elem);
         break;
      case DD_IMAGE :
         DicomDirImageList.push_back(elem);
         break;
      default :
         return false;
   }
   return true;
}
//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
