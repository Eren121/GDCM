/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirStudy.h,v $
  Language:  C++
  Date:      $Date: 2004/10/22 03:05:41 $
  Version:   $Revision: 1.11 $
                                                                                
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
namespace gdcm 
{

//-----------------------------------------------------------------------------
typedef std::list<DicomDirSerie *> ListDicomDirSerie;

//-----------------------------------------------------------------------------
class GDCM_EXPORT DicomDirStudy : public DicomDirObject
{
public:
   DicomDirStudy(SQItem *s, TagDocEntryHT *ptagHT); 
   DicomDirStudy(TagDocEntryHT *ptagHT); 

   ~DicomDirStudy();

   virtual void Print(std::ostream &os = std::cout);
   virtual void Write(std::ofstream *fp, FileType t);
/**
 * \ingroup DicomDirStudy
 * \brief   returns the SERIE chained List for this STUDY.
 */
   ListDicomDirSerie &GetDicomDirSeries() { return series; };
/**
 * \ingroup DicomDirStudy
 * \brief   adds the passed SERIE to the SERIE chained List for this STUDY.
 */ 
   void AddDicomDirSerie(DicomDirSerie *obj) { series.push_back(obj); };

/**
 * \ingroup DicomDirStudy
 * \brief   TODO
 */ 
   DicomDirSerie* NewSerie();
    
private:
/**
* \brief chained list of DicomDirSeries (to be exploited recursively)
*/ 
   ListDicomDirSerie series;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
