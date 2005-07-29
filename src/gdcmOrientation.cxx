/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmOrientation.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/29 15:07:56 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmOrientation.h"
#include "gdcmFile.h"
#include "gdcmDebug.h"
#include <math.h> // for sqrt

namespace gdcm 
{
//--------------------------------------------------------------------
//  THERALYS Algorithm to determine the most similar basic orientation
//
//  Transliterated from original Python code.
//  Kept as close as possible to the original code
//  in order to speed up any further modif of Python code :-(
//-----------------------------------------------------------------------

/**
 * \brief  THERALYS' Algorithm to determine the most similar basic orientation
 *           (Axial, Coronal, Sagital) of the image
 * \note Should be run on the first gdcm::File of a 'coherent' Serie
 * @return orientation code
 *   #   0 :   Not Applicable (neither 0020,0037 Image Orientation Patient 
 *   #                         nor     0020,0032 Image Position    found )
 *   #   1 :   Axial
 *   #  -1 :   Axial invert
 *   #   2 :   Coronal
 *   #  -2 :   Coronal invert
 *   #   3 :   Sagital
 *   #  -3 :   Sagital invert
 *   #   4 :   Heart Axial
 *   #  -4 :   Heart Axial invert
 *   #   5 :   Heart Coronal
 *   #  -5 :   Heart Coronal invert
 *   #   6 :   Heart Sagital
 *   #  -6 :   Heart Sagital invert
 */
double Orientation::TypeOrientation( File *f )
{
   float iop[6];
   bool succ = f->GetImageOrientationPatient( iop );
   if ( !succ )
   {
      gdcmErrorMacro( "No Image Orientation (0020,0037) was found in the file, cannot proceed." )
      return 0;
   }

   vector3D ori1;
   vector3D ori2;

   ori1.x = iop[0]; ori1.y = iop[1]; ori1.z = iop[2]; 
   ori1.x = iop[3]; ori2.y = iop[4]; ori2.z = iop[5];

   // two perpendicular vectors describe one plane
   double dicPlane[6][2][3] =
   { {  {1,    0,    0   },{0,       1,     0     }  }, // Axial
     {  {1,    0,    0   },{0,       0,    -1     }  }, // Coronal
     {  {0,    1,    0   },{0,       0,    -1     }  }, // Sagittal
     {  { 0.8, 0.5,  0.0 },{-0.1,    0.1 , -0.95  }  }, // Axial - HEART
     {  { 0.8, 0.5,  0.0 },{-0.6674, 0.687, 0.1794}  }, // Coronal - HEART
     {  {-0.1, 0.1, -0.95},{-0.6674, 0.687, 0.1794}  }  // Sagittal - HEART
   };

   vector3D refA;
   vector3D refB;
   int i = 0;
   Res res;   // [ <result> , <memory of the last succes calcule> ]
   res.first = 0;
   res.second = 99999;
   for (int numDicPlane=0; numDicPlane<6; numDicPlane++)
   {
       ++i;
       // refA=plane[0]
       refA.x = dicPlane[numDicPlane][0][0]; 
       refA.y = dicPlane[numDicPlane][0][1]; 
       refA.z = dicPlane[numDicPlane][0][2];
       // refB=plane[1]
       refB.x = dicPlane[numDicPlane][1][0]; 
       refB.y = dicPlane[numDicPlane][1][1]; 
       refB.z = dicPlane[numDicPlane][1][2];
       res=VerfCriterion(  i, CalculLikelyhood2Vec(refA,refB,ori1,ori2), res );
       res=VerfCriterion( -i, CalculLikelyhood2Vec(refB,refA,ori1,ori2), res );
   }
   return res.first;
/*
//             i=0
//             res=[0,99999]  ## [ <result> , <memory of the last succes calculus> ]
//             for plane in dicPlane:
//                 i=i+1
//                 refA=plane[0]
//                 refB=plane[1]
//                 res=self.VerfCriterion(  i , self.CalculLikelyhood2Vec(refA,refB,ori1,ori2) , res )
//                 res=self.VerfCriterion( -i , self.CalculLikelyhood2Vec(refB,refA,ori1,ori2) , res )
//             return res[0]
*/
}

Res 
Orientation::VerfCriterion(int typeCriterion, double criterionNew, Res const &in)
{
   Res res;
   double criterion = in.second;
   if (criterionNew < criterion)
   {
      res.first  = criterionNew;
      res.second = typeCriterion;
   }
/*
//   type = res[0]
//   criterion = res[1]
// #     if criterionNew<0.1 and criterionNew<criterion:
//   if criterionNew<criterion:
//      criterion=criterionNew
//      type=typeCriterion
//   return [ type , criterion ]
*/
   return res;
} 

inline double square_dist(vector3D const &v1, vector3D const &v2)
{
  double res;
  res = (v1.x - v2.x)*(v1.x - v2.x) +
        (v1.y - v2.y)*(v1.y - v2.y) +
        (v1.z - v2.z)*(v1.z - v2.z);
  return res;
}

//------------------------- Purpose : -----------------------------------
//- This function determines the orientation similarity of two planes.
//  Each plane is described by two vectors.
//------------------------- Parameters : --------------------------------
//- <refA>  : - type : vector 3D (double)
//- <refB>  : - type : vector 3D (double)
//            - Description of the first plane
//- <ori1>  : - type : vector 3D (double)
//- <ori2>  : - type : vector 3D (double)
//            - Description of the second plane
//------------------------- Return : ------------------------------------
// double :   0 if the planes are perpendicular. While the difference of
//            the orientation between the planes are big more enlarge is
//            the criterion.
//------------------------- Other : -------------------------------------
// The calculus is based with vectors normalice
double
Orientation::CalculLikelyhood2Vec(vector3D const &refA, vector3D const &refB, 
                                  vector3D const &ori1, vector3D const &ori2 )
{

   vector3D ori3 = ProductVectorial(ori1,ori2);
   vector3D refC = ProductVectorial(refA,refB);
   double res = square_dist(refC, ori3);

   return sqrt(res);
}

//------------------------- Purpose : -----------------------------------
//- Calculus of the poduct vectorial between two vectors 3D
//------------------------- Parameters : --------------------------------
//- <vec1>  : - type : vector 3D (double)
//- <vec2>  : - type : vector 3D (double)
//------------------------- Return : ------------------------------------
// (vec) :    - Vector 3D
//------------------------- Other : -------------------------------------
vector3D
Orientation::ProductVectorial(vector3D const & vec1, vector3D const & vec2)
{
   vector3D vec3;
   vec3.x =    vec1.y*vec2.z - vec1.z*vec2.y;
   vec3.y = -( vec1.x*vec2.z - vec1.z*vec2.x);
   vec3.z =    vec1.x*vec2.y - vec1.y*vec2.x;

   return vec3;
}

} // end namespace gdcm

