/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirPatient.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/12 04:35:45 $
  Version:   $Revision: 1.15 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirElement.h"
#include "gdcmGlobal.h"
namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor
 * @param  s SQ Item holding the elements related to this "PATIENT" part
 * @param ptagHT pointer to the HTable (DicomDirObject needs it 
 *               to build the HeaderEntries)
 */
DicomDirPatient::DicomDirPatient(SQItem *s, TagDocEntryHT *ptagHT):
   DicomDirObject(ptagHT)
{
   docEntries = s->GetDocEntries();
}
/**
 * \brief   Constructor
 * @param ptagHT pointer to the HTable (DicomDirObject needs it 
 *               to build the HeaderEntries)
 */
DicomDirPatient::DicomDirPatient(TagDocEntryHT* ptagHT):
   DicomDirObject(ptagHT)
{
}
/**
 * \brief   Canonical destructor.
 */
DicomDirPatient::~DicomDirPatient() 
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
void DicomDirPatient::Print(std::ostream& os)
{
   os << "PATIENT" << std::endl;
   DicomDirObject::Print(os);

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
void DicomDirPatient::Write(FILE* fp, FileType t)
{
   DicomDirObject::Write(fp, t);

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
DicomDirStudy* DicomDirPatient::NewStudy()
{
   std::list<Element> elemList = 
      Global::GetDicomDirElements()->GetDicomDirStudyElements();
      
   DicomDirStudy* st = new DicomDirStudy( PtagHT );
   st->FillObject(elemList);

   studies.push_front(st);
   return st; 
}   

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
