// gdcmDicomDirSerie.cxx
//-----------------------------------------------------------------------------
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirElement.h"
#include "gdcmGlobal.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief  Constructor 
 * @param  s  SQ Item holding the elements related to this "SERIE" part
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmDocEntries)
 */
gdcmDicomDirSerie::gdcmDicomDirSerie(gdcmSQItem *s, TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
{
   docEntries = s->GetDocEntries();
}
gdcmDicomDirSerie::gdcmDicomDirSerie(TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
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
      
   gdcmDicomDirImage *st = new gdcmDicomDirImage(ptagHT);
   FillObject(elemList);
   images.push_front(st);
   return st;   
} 
//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
