/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirPatient.h,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:47 $
  Version:   $Revision: 1.4 $
                                                                                
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
typedef std::list<gdcmDicomDirStudy *> ListDicomDirStudy;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmDicomDirPatient : public gdcmObject 
{
public:
   gdcmDicomDirPatient(gdcmSQItem *s, TagDocEntryHT *ptagHT); 
   gdcmDicomDirPatient(TagDocEntryHT *ptagHT); 

   ~gdcmDicomDirPatient(void);

   virtual void Print(std::ostream &os = std::cout);
/**
 * \ingroup gdcmDicomDirPatient
 * \brief   returns the STUDY chained List for this PATIENT.
 */
   inline ListDicomDirStudy &GetDicomDirStudies() 
      {return studies;};
/**
 * \ingroup gdcmDicomDirPatient
 * \brief   adds the passed STUDY to the STUDY chained List for this PATIENT.
 */      
   inline void AddDicomDirStudy(gdcmDicomDirStudy *obj) 
      {studies.push_back(obj);};
/**
 * \ingroup gdcmDicomDirPatient
 * \brief   TODO
 */ 
   gdcmDicomDirStudy* NewStudy(void); 
         
private:

/**
* \brief chained list of DicomDirStudy
*/ 
   ListDicomDirStudy studies;
};

//-----------------------------------------------------------------------------
#endif
