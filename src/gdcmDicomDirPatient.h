/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirPatient.h,v $
  Language:  C++
  Date:      $Date: 2004/10/12 04:35:45 $
  Version:   $Revision: 1.10 $
                                                                                
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
#include "gdcmDicomDirStudy.h"
#include "gdcmSQItem.h"

namespace gdcm 
{

//-----------------------------------------------------------------------------
typedef std::list<DicomDirStudy*> ListDicomDirStudy;

//-----------------------------------------------------------------------------
class GDCM_EXPORT DicomDirPatient : public DicomDirObject 
{
public:
   DicomDirPatient(SQItem *s, TagDocEntryHT *ptagHT); 
   DicomDirPatient(TagDocEntryHT *ptagHT); 

   ~DicomDirPatient();

   virtual void Print(std::ostream &os = std::cout);
   virtual void Write(FILE *fp, FileType t);
   /// Returns the STUDY chained List for this PATIENT.
   ListDicomDirStudy &GetDicomDirStudies() { return studies; };

   /// adds the passed STUDY to the STUDY chained List for this PATIENT.
   void AddDicomDirStudy(DicomDirStudy *obj) { studies.push_back(obj); };

   ///  TODO
   DicomDirStudy* NewStudy(); 
         
private:

   /// chained list of DicomDirStudy
   ListDicomDirStudy studies;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
