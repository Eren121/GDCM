/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirStudy.h,v $
  Language:  C++
  Date:      $Date: 2005/02/04 16:51:36 $
  Version:   $Revision: 1.24 $
                                                                                
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
 * \brief   describes a STUDY within a within a PATIENT
 * (DicomDirPatient) of a given DICOMDIR (DicomDir)
 */
class GDCM_EXPORT DicomDirStudy : public DicomDirObject
{
public:
   DicomDirStudy(bool empty=false); 
   ~DicomDirStudy();

   void Print(std::ostream &os = std::cout, std::string const &indent = "" );
   void WriteContent(std::ofstream *fp, FileType t);

   // Serie methods
   DicomDirSerie* NewSerie();
   /// Adds a gdcm::DicomDirSerie to a Study
   void AddSerie(DicomDirSerie *obj) { Series.push_back(obj); };
   void ClearSerie();

   DicomDirSerie *GetFirstSerie();
   DicomDirSerie *GetNextSerie();
   DicomDirSerie *GetLastSerie();
    
private:

   /// chained list of DicomDirSeries (to be exploited recursively)
   ListDicomDirSerie Series;
   /// iterator on the DicomDirSeries of the current DicomDirStudy
   ListDicomDirSerie::iterator ItSerie;

};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
