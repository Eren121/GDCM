/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirSerie.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/18 14:28:32 $
  Version:   $Revision: 1.30 $
                                                                                
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
#include "gdcmDebug.h"

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
 * @param   indent indent
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

/**
 * \brief   Get the first entry while visiting the DicomDirImage
 * \return  The first DicomDirImage if found, otherwhise NULL
 */
DicomDirImage *DicomDirSerie::GetFirstEntry()
{
   ItDicomDirImage = Images.begin();
   if (ItDicomDirImage != Images.end())
      return *ItDicomDirImage;
   return NULL;
}

/**
 * \brief   Get the next entry while visiting the DicomDirImages
 * \note : meaningfull only if GetFirstEntry already called
 * \return  The next DicomDirImages if found, otherwhise NULL
 */
DicomDirImage *DicomDirSerie::GetNextEntry()
{
   gdcmAssertMacro (ItDicomDirImage != Images.end());
   {
      ++ItDicomDirImage;
      if (ItDicomDirImage != Images.end())      
         return *ItDicomDirImage;
   }
   return NULL;
}
 
//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
} // end namespace gdcm


