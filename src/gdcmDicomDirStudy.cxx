/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirStudy.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/25 11:11:58 $
  Version:   $Revision: 1.31 $
                                                                                
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
#include "gdcmDebug.h"

namespace gdcm 
{
//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief  Constructor 
 */
DicomDirStudy::DicomDirStudy(bool empty):
   DicomDirObject()
{
   if( !empty )
   {
      ListDicomDirStudyElem const &elemList = 
         Global::GetDicomDirElements()->GetDicomDirStudyElements();
      FillObject(elemList);
   }
}

/**
 * \brief   Canonical destructor.
 */
DicomDirStudy::~DicomDirStudy() 
{
   ClearSerie();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints the Object
 * @param os ostream to write to 
 * @param indent Indentation string to be prepended during printing
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
   DicomDirSerie* st = new DicomDirSerie();
   Series.push_back(st);
   return st;
} 

/**
 * \brief  Remove all series in the study 
 */
void DicomDirStudy::ClearSerie()
{
   for(ListDicomDirSerie::iterator cc = Series.begin();
                                   cc != Series.end();
                                 ++cc )
   {
      delete *cc;
   }
   Series.clear();
}

/**
 * \brief   Get the first entry while visiting the DicomDirSeries
 * \return  The first DicomDirSerie if found, otherwhise NULL
 */
DicomDirSerie *DicomDirStudy::GetFirstSerie()
{
   ItSerie = Series.begin();
   if (ItSerie != Series.end())
      return *ItSerie;
   return NULL;
}

/**
 * \brief   Get the next entry while visiting the DicomDirSeries
 * \note : meaningfull only if GetFirstEntry already called
 * \return  The next DicomDirSerie if found, otherwhise NULL
 */
DicomDirSerie *DicomDirStudy::GetNextSerie()
{
   gdcmAssertMacro (ItSerie != Series.end());
   {
      ++ItSerie;
      if (ItSerie != Series.end())
         return *ItSerie;
   }
   return NULL;
}  

/**
 * \brief   Get the last entry while visiting the DicomDirSeries
 * \return  The first DicomDirSerie if found, otherwhise NULL
 */
DicomDirSerie *DicomDirStudy::GetLastSerie()
{
   ItSerie = Series.end();
   if (ItSerie != Series.begin())
   {
     --ItSerie;
      return *ItSerie;
   }
   return NULL;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
} // end namespace gdcm

