/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirStudy.h,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:06 $
  Version:   $Revision: 1.9 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMDIRSTUDY_H
#define GDCMDICOMDIRSTUDY_H

#include "gdcmDicomDirObject.h"
#include "gdcmDicomDirSerie.h"

//-----------------------------------------------------------------------------
typedef std::list<gdcmDicomDirSerie *> ListDicomDirSerie;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirStudy : public gdcmDicomDirObject
{
public:
   gdcmDicomDirStudy(gdcmSQItem *s, TagDocEntryHT *ptagHT); 
   gdcmDicomDirStudy(TagDocEntryHT *ptagHT); 

   ~gdcmDicomDirStudy();

   virtual void Print(std::ostream &os = std::cout);
   virtual void Write(FILE *fp, FileType t);
/**
 * \ingroup gdcmDicomDirStudy
 * \brief   returns the SERIE chained List for this STUDY.
 */
   ListDicomDirSerie &GetDicomDirSeries() { return series; };
/**
 * \ingroup gdcmDicomDirStudy
 * \brief   adds the passed SERIE to the SERIE chained List for this STUDY.
 */ 
   void AddDicomDirSerie(gdcmDicomDirSerie *obj) { series.push_back(obj); };

/**
 * \ingroup gdcmDicomDirStudy
 * \brief   TODO
 */ 
   gdcmDicomDirSerie* NewSerie();
    
private:
/**
* \brief chained list of DicomDirSeries (to be exploited recursively)
*/ 
   ListDicomDirSerie series;
};

//-----------------------------------------------------------------------------
#endif
