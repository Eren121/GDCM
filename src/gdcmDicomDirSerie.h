/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirSerie.h,v $
  Language:  C++
  Date:      $Date: 2004/10/13 14:15:29 $
  Version:   $Revision: 1.11 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMDIRSERIE_H
#define GDCMDICOMDIRSERIE_H

#include "gdcmDicomDirObject.h"
#include "gdcmDicomDirImage.h"
namespace gdcm 
{

//-----------------------------------------------------------------------------
typedef std::list<DicomDirImage *> ListDicomDirImage;

//-----------------------------------------------------------------------------
class GDCM_EXPORT DicomDirSerie : public DicomDirObject 
{
public:
   DicomDirSerie( SQItem* s, TagDocEntryHT* ptagHT ); 
   DicomDirSerie( TagDocEntryHT* ptagHT ); 

   ~DicomDirSerie();

   virtual void Print( std::ostream& os = std::cout );
   virtual void Write( FILE* fp, FileType t );
/**
 * \ingroup DicomDirSerie
 * \brief   returns the IMAGE chained List for this SERIE.
 */
   ListDicomDirImage& GetDicomDirImages() { return images; };
/**
 * \ingroup DicomDirSerie
 * \brief   adds the passed IMAGE to the IMAGE chained List for this SERIE.
 */       
   void AddDicomDirImage(DicomDirImage *obj) { images.push_back(obj); };

/**
 * \ingroup DicomDirSerie
 * \brief   TODO
 */ 
   DicomDirImage* NewImage();
    
private:
/**
* \brief chained list of DicomDirImages
*/ 
   ListDicomDirImage images;
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
