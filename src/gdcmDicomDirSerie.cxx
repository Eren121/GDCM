/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirSerie.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/29 12:48:46 $
  Version:   $Revision: 1.40 $
                                                                                
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
 * \note End user must use : DicomDirStudy::NewSerie() 
 */
DicomDirSerie::DicomDirSerie(bool empty):
   DicomDirObject()
{
   if ( !empty )
   {
      ListDicomDirSerieElem const &elemList = 
         Global::GetDicomDirElements()->GetDicomDirSerieElements();   
      FillObject(elemList);
   }
}

/**
 * \brief   Canonical destructor.
 */
DicomDirSerie::~DicomDirSerie() 
{
   ClearImage();
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
 * \brief   adds a new Image (with the basic elements) to a partially created 
 *          DICOMDIR
 */
DicomDirImage *DicomDirSerie::NewImage()
{
   DicomDirImage *dd = DicomDirImage::New();
   Images.push_back(dd);
   return dd;   
}

/**
 * \brief  Remove all images in the serie 
 */
void DicomDirSerie::ClearImage()
{
   for(ListDicomDirImage::iterator cc = Images.begin();
                                   cc != Images.end();
                                   ++cc)
   {
      (*cc)->Delete();
   }
   Images.clear();
}

/**
 * \brief   Get the first entry while visiting the DicomDirImage
 * \return  The first DicomDirImage if DicomDirserie not empty, otherwhise NULL
 */
DicomDirImage *DicomDirSerie::GetFirstImage()
{
   ItImage = Images.begin();
   if (ItImage != Images.end())
      return *ItImage;
   return NULL;
}

/**
 * \brief   Get the next entry while visiting the DicomDirImages
 * \note : meaningfull only if GetFirstEntry already called
 * \return  The next DicomDirImages if found, otherwhise NULL
 */
DicomDirImage *DicomDirSerie::GetNextImage()
{
   gdcmAssertMacro (ItImage != Images.end());

   ++ItImage;
   if (ItImage != Images.end())      
      return *ItImage;
   return NULL;
}

/**
 * \brief Copies all the attributes from an other DocEntrySet 
 * @param set entry to copy from
 * @remarks The contained DocEntries a not copied, only referenced
 */
void DicomDirSerie::Copy(DocEntrySet *set)
{
   // Remove all previous childs
   ClearImage();

   DicomDirObject::Copy(set);

   DicomDirSerie *ddEntry = dynamic_cast<DicomDirSerie *>(set);
   if( ddEntry )
   {
      Images = ddEntry->Images;
      for(ItImage = Images.begin();ItImage != Images.end();++ItImage)
         (*ItImage)->Register();
   }
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints the Object
 * @param os ostream to write to
 * @param indent Indentation string to be prepended during printing
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
} // end namespace gdcm
