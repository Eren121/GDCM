/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirPatient.cxx,v $
  Language:  C++
  Date:      $Date: 2004/08/26 15:29:52 $
  Version:   $Revision: 1.11 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirElement.h"
#include "gdcmGlobal.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor
 * @param  s SQ Item holding the elements related to this "PATIENT" part
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 */
gdcmDicomDirPatient::gdcmDicomDirPatient(gdcmSQItem *s, TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
{
   docEntries = s->GetDocEntries();
}
/**
 * \brief   Constructor
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 */
gdcmDicomDirPatient::gdcmDicomDirPatient(TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
{
}
/**
 * \brief   Canonical destructor.
 */
gdcmDicomDirPatient::~gdcmDicomDirPatient() 
{
   for(ListDicomDirStudy::iterator cc = studies.begin();
                                   cc != studies.end(); ++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints the Object
 * @return
 */ 
void gdcmDicomDirPatient::Print(std::ostream &os)
{
   os << "PATIENT" << std::endl;
   gdcmObject::Print(os);

   for(ListDicomDirStudy::iterator cc = studies.begin();
                                   cc != studies.end(); ++cc)
   {
      (*cc)->SetPrintLevel(PrintLevel);
      (*cc)->Print(os);
   }
}

/**
 * \brief   Writes the Object
 * @return
 */ 
void gdcmDicomDirPatient::Write(FILE *fp, FileType t)
{
   gdcmObject::Write(fp, t);

   for(ListDicomDirStudy::iterator cc = studies.begin();cc!=studies.end();++cc)
   {
      (*cc)->Write( fp, t );
   }
}
//-----------------------------------------------------------------------------
// Public

/**
 * \brief   adds a new Patient at the begining of the PatientList
 *          of a partially created DICOMDIR
 */
gdcmDicomDirStudy * gdcmDicomDirPatient::NewStudy()
{
   std::list<gdcmElement> elemList = 
      gdcmGlobal::GetDicomDirElements()->GetDicomDirStudyElements();
      
   gdcmDicomDirStudy *st = new gdcmDicomDirStudy( PtagHT );
   st->FillObject(elemList);

   studies.push_front(st);
   return st; 
}   

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
