/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirStudy.h,v $
  Language:  C++
  Date:      $Date: 2005/01/16 04:50:41 $
  Version:   $Revision: 1.17 $
                                                                                
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

namespace gdcm 
{
class DicomDirSerie;
//-----------------------------------------------------------------------------
typedef std::list<DicomDirSerie *> ListDicomDirSerie;

//-----------------------------------------------------------------------------
/**
 * \ingroup DicomDirStudy
 * \brief   describes a STUDY within a within a PATIENT
 * (DicomDirPatient) of a given DICOMDIR (DicomDir)
 */
class GDCM_EXPORT DicomDirStudy : public DicomDirObject
{
public:
   DicomDirStudy(); 
   ~DicomDirStudy();

   void Print(std::ostream &os = std::cout, std::string const & indent = "" );
   void WriteContent(std::ofstream *fp, FileType t);

   /**
    * \ingroup DicomDirStudy
    * \brief   returns the SERIE chained List for this STUDY.
    */
   ListDicomDirSerie const &GetDicomDirSeries() const { return Series; };

   /**
    * \ingroup DicomDirStudy
    * \brief   adds the passed SERIE to the SERIE chained List for this STUDY.
    */ 
   void AddDicomDirSerie(DicomDirSerie *obj) { Series.push_back(obj); };

   /**
    * \ingroup DicomDirStudy
    * \brief   TODO
    */ 
   DicomDirSerie* NewSerie();
    
private:
/**
* \brief chained list of DicomDirSeries (to be exploited recursively)
*/ 
   ListDicomDirSerie Series;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
