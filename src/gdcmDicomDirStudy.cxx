// gdcmDicomDirStudy.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirElement.h"
#include "gdcmGlobal.h"
//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup gdcmDicomDirStudy
 * \brief constructor  
 * @param  begin  iterator (inside the gdcmParser chained list)
 *                on the first Header Entry (i.e Dicom Element)
 *                related to this "STUDY" part
 * @param  end  iterator  (inside the gdcmParser chained list)
 *              on the last Header Entry (i.e Dicom Element) 
 *              related to this 'STUDY' part
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 * @param plistEntries pointer to the chained List (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 */
gdcmDicomDirStudy::gdcmDicomDirStudy(ListTag::iterator begin,
                                     ListTag::iterator end,
                                     TagHeaderEntryHT *ptagHT, 
                                     ListTag *plistEntries):
   gdcmObject(begin,end,ptagHT,plistEntries)
{
}

/**
 * \ingroup gdcmDicomDirStudy
 * \brief   Canonical destructor.
 */
gdcmDicomDirStudy::~gdcmDicomDirStudy() 
{
   for(ListDicomDirSerie::iterator cc = series.begin();cc != series.end();++cc)
   {
      delete *cc;
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDicomDirStudy
 * \brief   Prints the Object
 * @return
 */ 
void gdcmDicomDirStudy::Print(std::ostream &os)
{
   os<<"STUDY"<<std::endl;
   gdcmObject::Print(os);

   for(ListDicomDirSerie::iterator cc = series.begin();cc != series.end();++cc)
   {
      (*cc)->SetPrintLevel(printLevel);
      (*cc)->Print(os);
   }
}

//-----------------------------------------------------------------------------
// Public

/**
 * \ingroup gdcmDicomStudy
 * \brief   adds a new Serie at the begining of the SerieList
 *          of a partially created DICOMDIR
 */
gdcmDicomDirSerie * gdcmDicomDirStudy::NewSerie(void) {
  
   std::list<gdcmElement> elemList;
   elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirSerieElements();   

   FillObject(elemList);
   gdcmDicomDirSerie *st = new gdcmDicomDirSerie(i, j, ptagHT, plistEntries);
   series.push_front(st);
   return st;  
}   
//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
