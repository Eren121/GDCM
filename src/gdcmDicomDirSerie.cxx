/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirSerie.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/16 04:50:41 $
  Version:   $Revision: 1.26 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirElement.h"
#include "gdcmDicomDirImage.h"
#include "gdcmGlobal.h"

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief  Constructor 
 */
DicomDirSerie::DicomDirSerie():
   DicomDirObject()
{
}
/**
 * \brief   Canonical destructor.
 */
DicomDirSerie::~DicomDirSerie() 
{
   for(ListDicomDirImage::iterator cc = Images.begin();
                                   cc != Images.end();
                                   ++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints the Object
 * @param os ostream to write to
 */ 
void DicomDirSerie::Print(std::ostream &os, std::string const &)
{
   os << "SERIE" << std::endl;
   DicomDirObject::Print(os);

   for(ListDicomDirImage::iterator cc = Images.begin();
                                   cc != Images.end();
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
 */ 
void DicomDirSerie::WriteContent(std::ofstream *fp, FileType t)
{
   DicomDirObject::WriteContent(fp, t);

   for(ListDicomDirImage::iterator cc = Images.begin();
                                   cc!= Images.end();
                                 ++cc )
   {
      (*cc)->WriteContent( fp, t );
   }
}

/**
 * \brief   adds a new Image (with the basic elements) to a partially created DICOMDIR
 */
DicomDirImage *DicomDirSerie::NewImage()
{
   ListDicomDirImageElem const &elemList = 
      Global::GetDicomDirElements()->GetDicomDirImageElements();

   DicomDirImage *st = new DicomDirImage();
   FillObject(elemList);
   Images.push_front(st);

   return st;   
} 
//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
} // end namespace gdcm


