/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirSerie.h,v $
  Language:  C++
  Date:      $Date: 2004/09/23 10:47:10 $
  Version:   $Revision: 1.8 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMDIRSERIE_H
#define GDCMDICOMDIRSERIE_H

#include "gdcmDicomDirObject.h"
#include "gdcmDicomDirImage.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmDicomDirImage *> ListDicomDirImage;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirSerie : public gdcmDicomDirObject 
{
public:
   gdcmDicomDirSerie(gdcmSQItem *s, TagDocEntryHT *ptagHT); 
   gdcmDicomDirSerie(TagDocEntryHT *ptagHT); 

   ~gdcmDicomDirSerie();

   virtual void Print(std::ostream &os = std::cout);
   virtual void Write(FILE *fp, FileType t);
/**
 * \ingroup gdcmDicomDirSerie
 * \brief   returns the IMAGE chained List for this SERIE.
 */
   ListDicomDirImage &GetDicomDirImages() { return images; };
/**
 * \ingroup gdcmDicomDirSerie
 * \brief   adds the passed IMAGE to the IMAGE chained List for this SERIE.
 */       
   void AddDicomDirImage(gdcmDicomDirImage *obj) { images.push_back(obj); };

/**
 * \ingroup gdcmDicomDirSerie
 * \brief   TODO
 */ 
   gdcmDicomDirImage* NewImage();
    
private:
/**
* \brief chained list of DicomDirImages
*/ 
   ListDicomDirImage images;
};

//-----------------------------------------------------------------------------
#endif
