/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirPatient.h,v $
  Language:  C++
  Date:      $Date: 2004/08/01 02:39:09 $
  Version:   $Revision: 1.6 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMPATIENT_H
#define GDCMPATIENT_H

#include "gdcmObject.h"
#include "gdcmDicomDirStudy.h"
#include "gdcmSQItem.h"
//-----------------------------------------------------------------------------
typedef std::list<gdcmDicomDirStudy*> ListDicomDirStudy;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirPatient : public gdcmObject 
{
public:
   gdcmDicomDirPatient(gdcmSQItem *s, TagDocEntryHT *ptagHT); 
   gdcmDicomDirPatient(TagDocEntryHT *ptagHT); 

   ~gdcmDicomDirPatient();

   virtual void Print(std::ostream &os = std::cout);

   /// Returns the STUDY chained List for this PATIENT.
   ListDicomDirStudy &GetDicomDirStudies() { return studies; };

   /// adds the passed STUDY to the STUDY chained List for this PATIENT.
   void AddDicomDirStudy(gdcmDicomDirStudy *obj) { studies.push_back(obj); };

   ///  TODO
   gdcmDicomDirStudy* NewStudy(); 
         
private:

   /// chained list of DicomDirStudy
   ListDicomDirStudy studies;
};

//-----------------------------------------------------------------------------
#endif
