/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirSerie.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:47 $
  Version:   $Revision: 1.10 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

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

/**
 * \brief  Constructor 
 * @param ptagHT pointer to the HTable (gdcmObject needs it 
 *               to build the gdcmDocEntries)
 */
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
