/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirSerie.h,v $
  Language:  C++
  Date:      $Date: 2005/01/25 11:11:58 $
  Version:   $Revision: 1.24 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMDIRSERIE_H
#define GDCMDICOMDIRSERIE_H

#include "gdcmDicomDirObject.h"

namespace gdcm 
{
class DicomDirImage;
//-----------------------------------------------------------------------------
typedef std::list<DicomDirImage *> ListDicomDirImage;

//-----------------------------------------------------------------------------
/**
 * \brief   describes a SERIE  within a within a STUDY
 * (DicomDirStudy) of a given DICOMDIR (DicomDir)
 */
class GDCM_EXPORT DicomDirSerie : public DicomDirObject 
{
public:
   DicomDirSerie(bool empty=false); 
   ~DicomDirSerie();

   void Print( std::ostream &os = std::cout, std::string const & indent = "" );
   void WriteContent( std::ofstream *fp, FileType t );
 
   // should avoid exposing internal mechanism
   DicomDirImage *GetFirstImage();
   DicomDirImage *GetNextImage();
   DicomDirImage *GetLastImage();
        
   /// adds the passed IMAGE to the IMAGE chained List for this SERIE.    
   void AddImage(DicomDirImage *obj) { Images.push_back(obj); };
   DicomDirImage *NewImage();

   void ClearImage();

private:

   ///chained list of DicomDirImages (to be exploited recursively)
   ListDicomDirImage Images;
   /// iterator on the DicomDirImages of the current DicomDirSerie
   ListDicomDirImage::iterator ItImage;
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
