/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmOrientation.h,v $
  Language:  C++
  Date:      $Date: 2005/09/21 16:39:53 $
  Version:   $Revision: 1.7 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMORIENTATION_H
#define GDCMORIENTATION_H

#include "gdcmBase.h"
#include <map>

namespace gdcm 
{
typedef struct
{
   double x;
   double y;
   double z;
} vector3D;

typedef std::pair<double, double> Res;
class File;

//-----------------------------------------------------------------------------
/**
 * \brief Orientation class for dealing with DICOM image orientation
 */
class GDCM_EXPORT Orientation : public Base
{
public:
  Orientation() {}
  ~Orientation() {}

  double TypeOrientation( File *file );
  std::string GetOrientation ( File *file );  
  
private:
   Res VerfCriterion(int typeCriterion, double criterionNew, Res const &res);
   double CalculLikelyhood2Vec(vector3D const &refA, vector3D const &refB, 
                               vector3D const &ori1, vector3D const &ori2);
   vector3D ProductVectorial(vector3D const &vec1, vector3D const &vec2);
   std::string GetSingleOrientation ( float *iop);
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
