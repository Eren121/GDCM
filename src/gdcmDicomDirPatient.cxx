/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirPatient.cxx,v $
  Language:  C++
  Date:      $Date: 2004/12/03 20:16:57 $
  Version:   $Revision: 1.21 $
                                                                                
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
#include "gdcmDicomDirStudy.h"
#include "gdcmSQItem.h"

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor
 * @param ptagHT pointer to the HTable (DicomDirObject needs it 
 *               to build the HeaderEntries)
 */
DicomDirPatient::DicomDirPatient():
   DicomDirObject()
{
}
/**
 * \brief   Canonical destructor.
 */
DicomDirPatient::~DicomDirPatient() 
{
   for(ListDicomDirStudy::const_iterator cc = Studies.begin();
                                         cc != Studies.end(); 
                                       ++cc )
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

   for(ListDicomDirStudy::const_iterator cc = Studies.begin();
                                         cc != Studies.end(); 
                                       ++cc )
   {
      (*cc)->SetPrintLevel(PrintLevel);
      (*cc)->Print(os);
   }
}

/**
 * \brief   Writes the Object
 * @return
 */ 
void DicomDirPatient::WriteContent(std::ofstream* fp, FileType t)
{
   DicomDirObject::WriteContent(fp, t);

   for(ListDicomDirStudy::iterator cc = Studies.begin();
                                   cc!= Studies.end();
                                 ++cc )
   {
      (*cc)->WriteContent( fp, t );
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
   ListDicomDirStudyElem const & elemList = 
      Global::GetDicomDirElements()->GetDicomDirStudyElements();
      
   DicomDirStudy* st = new DicomDirStudy();
   st->FillObject(elemList);

   Studies.push_front(st);
   return st; 
}   

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
