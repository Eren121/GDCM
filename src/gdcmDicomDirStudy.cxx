/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirStudy.cxx,v $
  Language:  C++
  Date:      $Date: 2004/08/01 02:39:09 $
  Version:   $Revision: 1.9 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirElement.h"
#include "gdcmGlobal.h"
//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \ingroup gdcmDicomDirStudy
 * \brief constructor  
 * @param  s SQ Item holding the elements related to this "STUDY" part
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 */
gdcmDicomDirStudy::gdcmDicomDirStudy(gdcmSQItem *s, TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
{
   docEntries = s->GetDocEntries();
}
/**
 * \ingroup gdcmDicomDirStudy
 * \brief constructor  
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmHeaderEntries)
 */
gdcmDicomDirStudy::gdcmDicomDirStudy(TagDocEntryHT *ptagHT):
   gdcmObject(ptagHT)
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
   os << "STUDY" << std::endl;
   gdcmObject::Print(os);

   for(ListDicomDirSerie::iterator cc = series.begin();
                                   cc != series.end();
                                   ++cc)
   {
      (*cc)->SetPrintLevel(PrintLevel);
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
gdcmDicomDirSerie * gdcmDicomDirStudy::NewSerie()
{
   std::list<gdcmElement> elemList = 
      gdcmGlobal::GetDicomDirElements()->GetDicomDirSerieElements();   

   gdcmDicomDirSerie *st = new gdcmDicomDirSerie(PtagHT);
   FillObject(elemList);
   series.push_front(st);

   return st;  
}   
//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
