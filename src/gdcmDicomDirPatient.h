/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirPatient.h,v $
  Language:  C++
  Date:      $Date: 2004/12/03 20:16:57 $
  Version:   $Revision: 1.15 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMPATIENT_H
#define GDCMPATIENT_H

#include "gdcmDicomDirObject.h"

namespace gdcm 
{
class DicomDirStudy;
//-----------------------------------------------------------------------------
typedef std::list<DicomDirStudy*> ListDicomDirStudy;

//-----------------------------------------------------------------------------
class GDCM_EXPORT DicomDirPatient : public DicomDirObject 
{
public:
   DicomDirPatient(); 
   ~DicomDirPatient();

   void Print(std::ostream &os = std::cout);
   void WriteContent(std::ofstream *fp, FileType t);

   /// Returns the STUDY chained List for this PATIENT.
   ListDicomDirStudy const & GetDicomDirStudies() const { return Studies; };

   /// adds the passed STUDY to the STUDY chained List for this PATIENT.
   void AddDicomDirStudy (DicomDirStudy *obj) { Studies.push_back(obj); };

   ///  TODO
   DicomDirStudy* NewStudy(); 
         
private:

   /// chained list of DicomDirStudy
   ListDicomDirStudy Studies;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
