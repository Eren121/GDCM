/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirStudy.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/17 10:59:52 $
  Version:   $Revision: 1.24 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirElement.h"
#include "gdcmGlobal.h"
#include "gdcmDicomDirSerie.h"

namespace gdcm 
{
//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief  Constructor 
 */
DicomDirStudy::DicomDirStudy():
   DicomDirObject()
{
}
/**
 * \ingroup DicomDirStudy
 * \brief   Canonical destructor.
 */
DicomDirStudy::~DicomDirStudy() 
{
   for(ListDicomDirSerie::iterator cc = Series.begin();
                                   cc != Series.end();
                                 ++cc )
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints the Object
 * @param os ostream to write to 
 * @return
 */ 
void DicomDirStudy::Print(std::ostream &os, std::string const & )
{
   os << "STUDY" << std::endl;
   DicomDirObject::Print(os);

   for(ListDicomDirSerie::iterator cc = Series.begin();
                                   cc != Series.end();
                                   ++cc)
   {
      (*cc)->SetPrintLevel(PrintLevel);
      (*cc)->Print(os);
   }
}

//-----------------------------------------------------------------------------
// Public

/**
 * \brief   Writes the Object
 * @param fp ofstream to write to
 * @param t Type of the File (explicit VR, implicitVR, ...) 
 * @return
 */ 
void DicomDirStudy::WriteContent(std::ofstream *fp, FileType t)
{
   DicomDirObject::WriteContent(fp, t);

   for(ListDicomDirSerie::iterator cc = Series.begin();
                                   cc!= Series.end();
                                 ++cc )
   {
      (*cc)->WriteContent( fp, t );
   }
}

/**
 * \brief   adds a new Serie at the begining of the SerieList
 *          of a partially created DICOMDIR
 */
DicomDirSerie *DicomDirStudy::NewSerie()
{
   ListDicomDirSerieElem const &elemList = 
      Global::GetDicomDirElements()->GetDicomDirSerieElements();   

   DicomDirSerie* st = new DicomDirSerie();
   FillObject(elemList);
   Series.push_front(st);

   return st;  
} 

/**
 * \brief   Initialise the visit of the Series
 */
void DicomDirStudy::InitTraversal()
{
   ItDicomDirSerie = Series.begin();
}

/**
 * \brief   Get the next entry while visiting the DicomDirSeries
 * \return  The next DicomDirSeries if found, otherwhise NULL
 */
DicomDirSerie *DicomDirStudy::GetNextEntry()
{
   if (ItDicomDirSerie != Series.end())
   {
      DicomDirSerie *tmp = *ItDicomDirSerie;
      ++ItDicomDirSerie;
      return tmp;
   }
   else
   {
      return NULL;
   }
}  
//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
} // end namespace gdcm

