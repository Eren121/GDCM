// gdcmDicomDirSerie.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirElement.h"
#include "gdcmGlobal.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief  Constructor 
 * @param  begin  iterator (inside the gdcmParser chained list)
 *                on the first Header Entry (i.e Dicom Element)
 *                related to this "SERIE" part
 * @param  end  iterator  (inside the gdcmParser chained list)
 *              on the last Header Entry (i.e Dicom Element) 
 *              related to this 'SERIE' part
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 * @param plistEntries pointer to the chained List (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 */
gdcmDicomDirSerie::gdcmDicomDirSerie(ListTag::iterator begin,
                                     ListTag::iterator end,              
                                     TagHeaderEntryHT *ptagHT, 
                                     ListTag *plistEntries ):
   gdcmObject(begin,end,ptagHT,plistEntries)
{
}

/**
 * \brief   Canonical destructor.
 */
gdcmDicomDirSerie::~gdcmDicomDirSerie() 
{
   for(ListDicomDirImage::iterator cc = images.begin();cc != images.end();++cc)
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
void gdcmDicomDirSerie::Print(std::ostream &os)
{
   os<<"SERIE"<<std::endl;
   gdcmObject::Print(os);

   for(ListDicomDirImage::iterator cc = images.begin();cc != images.end();++cc)
   {
      (*cc)->SetPrintLevel(printLevel);
      (*cc)->Print(os);
   }
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   adds a new Image to a partially created DICOMDIR
 */
gdcmDicomDirImage * gdcmDicomDirSerie::NewImage(void) {
   std::list<gdcmElement> elemList;   
   elemList=gdcmGlobal::GetDicomDirElements()->GetDicomDirImageElements();
      
   FillObject(elemList);
   gdcmDicomDirImage *st = new gdcmDicomDirImage(i, j, ptagHT, plistEntries);
   images.push_front(st);
   return st;   
} 
//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
