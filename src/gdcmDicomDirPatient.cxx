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
 * @param  s SQ Item holding the elements related to this "PATIENT" part
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 */
gdcmDicomDirPatient::gdcmDicomDirPatient(gdcmSQItem *s, TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
{
   docEntries = s->GetDocEntries();
}

gdcmDicomDirPatient::gdcmDicomDirPatient(TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
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
      
   gdcmDicomDirStudy *st = new gdcmDicomDirStudy( ptagHT);
   st->FillObject(elemList);

   studies.push_front(st);
   return st; 

  
}   

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
