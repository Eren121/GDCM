/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirSerie.cxx,v $
  Language:  C++
  Date:      $Date: 2004/09/24 11:39:21 $
  Version:   $Revision: 1.15 $
                                                                                
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
 * @param ptagHT pointer to the HTable (gdcmDicomDirObject needs it 
 *               to build the gdcmDocEntries)
 */
gdcmDicomDirSerie::gdcmDicomDirSerie(gdcmSQItem* s, TagDocEntryHT* ptagHT):
   gdcmDicomDirObject(ptagHT)
{
   docEntries = s->GetDocEntries();
}

/**
 * \brief  Constructor 
 * @param ptagHT pointer to the HTable (gdcmDicomDirObject needs it 
 *               to build the gdcmDocEntries)
 */
gdcmDicomDirSerie::gdcmDicomDirSerie(TagDocEntryHT* ptagHT):
   gdcmDicomDirObject(ptagHT)
{
}
/**
 * \brief   Canonical destructor.
 */
gdcmDicomDirSerie::~gdcmDicomDirSerie() 
{
   for(ListDicomDirImage::iterator cc = images.begin();
                                   cc != images.end();
                                   ++cc)
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
void gdcmDicomDirSerie::Print(std::ostream& os)
{
   os << "SERIE" << std::endl;
   gdcmDicomDirObject::Print(os);

   for(ListDicomDirImage::iterator cc = images.begin();
                                   cc != images.end();
                                   ++cc)
   {
      (*cc)->SetPrintLevel(PrintLevel);
      (*cc)->Print(os);
   }
}

//-----------------------------------------------------------------------------
// Public

/**
 * \brief   Writes the Object
 * @return
 */ 
void gdcmDicomDirSerie::Write(FILE* fp, FileType t)
{
   gdcmDicomDirObject::Write(fp, t);

   for(ListDicomDirImage::iterator cc = images.begin();cc!=images.end();++cc)
   {
      (*cc)->Write( fp, t );
   }
}

/**
 * \brief   adds a new Image (with the basic elements) to a partially created DICOMDIR
 */
gdcmDicomDirImage* gdcmDicomDirSerie::NewImage()
{
   std::list<gdcmElement> elemList = 
      gdcmGlobal::GetDicomDirElements()->GetDicomDirImageElements();

   gdcmDicomDirImage* st = new gdcmDicomDirImage(PtagHT);
   FillObject(elemList);
   images.push_front(st);

   return st;   
} 
//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
