// gdcmDicomDirPatient.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirElement.h"
#include "gdcmGlobal.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDicomDirPatient
 * \brief   Constructor
 * @param  begin  iterator (inside the gdcmParser chained list)
 *                on the first Header Entry (i.e Dicom Element)
 *                related to this "PATIENT" part
 * @param  end  iterator  (inside the gdcmParser chained list)
 *              on the last Header Entry (i.e Dicom Element) 
 *              related to this 'PATIENT' part
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 * @param plistEntries pointer to the chained List (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 */
gdcmDicomDirPatient::gdcmDicomDirPatient(ListTag::iterator begin,
                                         ListTag::iterator end,
                                         TagHeaderEntryHT *ptagHT, 
                                         ListTag *plistEntries):
   gdcmObject(begin,end,ptagHT,plistEntries)
{
}

/**
 * \ingroup gdcmDicomDirPatient
 * \brief   Canonical destructor.
 */
gdcmDicomDirPatient::~gdcmDicomDirPatient() 
{
   for(ListDicomDirStudy::iterator cc = studies.begin();cc != studies.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDicomDirPatient
 * \brief   Prints the Object
 * @return
 */ 
void gdcmDicomDirPatient::Print(std::ostream &os)
{
   os<<"PATIENT"<<std::endl;
   gdcmObject::Print(os);

   for(ListDicomDirStudy::iterator cc = studies.begin();cc!=studies.end();++cc)
   {
      (*cc)->SetPrintLevel(printLevel);
      (*cc)->Print(os);
   }
}

//-----------------------------------------------------------------------------
// Public

/**
 * \ingroup gdcmDicomDir
 * \brief   adds a new Patient at the begining of the PatientList
 *          of a partially created DICOMDIR
 */
gdcmDicomDirStudy * gdcmDicomDirPatient::NewStudy(void) {
   std::list<gdcmElement> elemList;   
   elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirStudyElements();
      
   FillObject(elemList);
   gdcmDicomDirStudy *st = new gdcmDicomDirStudy(i, j, ptagHT, plistEntries);
   studies.push_front(st);
   return st; 

  
}   

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
