/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirSerie.h,v $
  Language:  C++
  Date:      $Date: 2005/01/17 10:59:52 $
  Version:   $Revision: 1.20 $
                                                                                
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
   DicomDirSerie(); 
   ~DicomDirSerie();

   void Print( std::ostream &os = std::cout, std::string const & indent = "" );
   void WriteContent( std::ofstream *fp, FileType t );

    // TODO Remove GetDicomDirImages
    // use InitTraversal + GetNextEntry instead.
 
   /// Returns the IMAGE chained List for this SERIE.
   ListDicomDirImage const &GetDicomDirImages() const { return Images; };

   // should avoid exposing internal mechanism
   void InitTraversal();
   DicomDirImage *GetNextEntry();
        
   /// adds the passed IMAGE to the IMAGE chained List for this SERIE.    
   void AddDicomDirImage(DicomDirImage *obj) { Images.push_back(obj); };

   DicomDirImage *NewImage();

private:

   ///chained list of DicomDirImages (to be exploited recursively)
   ListDicomDirImage Images;
   /// iterator on the DicomDirImages of the current DicomDirSerie
   ListDicomDirImage::iterator ItDicomDirImage;
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
