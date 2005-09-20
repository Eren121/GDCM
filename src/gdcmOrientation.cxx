/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmOrientation.cxx,v $
  Language:  C++
  Date:      $Date: 2005/09/20 15:11:19 $
  Version:   $Revision: 1.10 $
                                                                                
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
 *   #   0 : Not Applicable (neither 0020,0037 Image Orientation Patient 
 *   #                       nor     0020,0032 Image Position           found)
 *   #   1 : Axial
 *   #  -1 : Axial invert
 *   #   2 : Coronal
 *   #  -2 : Coronal invert
 *   #   3 : Sagital
 *   #  -3 : Sagital invert
 *   #   4 : Heart Axial
 *   #  -4 : Heart Axial invert
 *   #   5 : Heart Coronal
 *   #  -5 : Heart Coronal invert
 *   #   6 : Heart Sagital
 *   #  -6 : Heart Sagital invert
 */
double Orientation::TypeOrientation( File *f )
{
   float iop[6];
   bool succ = f->GetImageOrientationPatient( iop );
   if ( !succ )
   {
      gdcmErrorMacro( "No Image Orientation (0020,0037) found in the file, cannot proceed." )
      return 0;
   }
/*
std::cout << " iop : ";
for(int i=0;i<6;i++)
   std::cout << iop[i] << "  ";
std::cout << std::endl;
*/
   vector3D ori1;
   vector3D ori2;

   ori1.x = iop[0]; ori1.y = iop[1]; ori1.z = iop[2]; 
   ori2.x = iop[3]; ori2.y = iop[4]; ori2.z = iop[5];

   // two perpendicular vectors describe one plane
   double dicPlane[6][2][3] =
   { {  { 1,   0,    0   },{ 0,      1,     0     }  }, // Axial
     {  { 1,   0,    0   },{ 0,      0,    -1     }  }, // Coronal
     {  { 0,   1,    0   },{ 0,      0,    -1     }  }, // Sagittal
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

 std::cout << "-------------- res : " << res.first << "|" << res.second 
           << std::endl;

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
 std::cout << "-------------- res : " << res.first << "|" << res.second 
           << std::endl;
       res=VerfCriterion( -i, CalculLikelyhood2Vec(refB,refA,ori1,ori2), res );
 std::cout << "-------------- res : " << res.first << "|" << res.second 
           << std::endl;
   }
   return res.first;
/*
//  i=0
//  res=[0,99999]  ## [ <result> , <memory of the last succes calculus> ]
//  for plane in dicPlane:
//      i=i+1
//      refA=plane[0]
//      refB=plane[1]
//      res=self.VerfCriterion(  i , self.CalculLikelyhood2Vec(refA,refB,ori1,ori2) , res )
//      res=self.VerfCriterion( -i , self.CalculLikelyhood2Vec(refB,refA,ori1,ori2) , res )
//  return res[0]
*/
}

Res 
Orientation::VerfCriterion(int typeCriterion, double criterionNew, Res const &in)
{
   Res res;
//   double type = in.first;
   double criterion = in.second;
   if (/*criterionNew < 0.1 && */criterionNew < criterion)
   {
      res.first  = typeCriterion;
      res.second = criterionNew;
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
Orientation::ProductVectorial(vector3D const &vec1, vector3D const &vec2)
{
   vector3D vec3;
   vec3.x =    vec1.y*vec2.z - vec1.z*vec2.y;
   vec3.y = -( vec1.x*vec2.z - vec1.z*vec2.x);
   vec3.z =    vec1.x*vec2.y - vec1.y*vec2.x;

   return vec3;
}

} // end namespace gdcm




// ---------------------------------------------------------------------------
// Here is the original Python code, kindly supplied by THERALYS
//
// C++ code doesn't give good results
// --> FIXME

/*

def TypeOrientation(self,file0):
"""
# ------------------------- Purpose : -----------------------------------
# - This function compare the orientation of the given image and the
#   basics orientations (Axial, Cornal, Sagital)
# ------------------------- Parameters : --------------------------------
# - <file0> : - type : string
#             - The name of the first image file of the serie
# ------------------------- Return : ------------------------------------
#   1 :   Axial
#  -1 :   Axial invert
#   2 :   Coronal
#  -2 :   Coronal invert
#   3 :   Sagital
#  -3 :   Sagital invert
#   4 :   Heart Axial
#  -4 :   Heart Axial invert
#   5 :   Heart Coronal
#  -5 :   Heart Coronal invert
#   6 :   Heart Sagital
#  -6 :   Heart Sagital invert
#
   # ------------------------- Other : -------------------------------------
# This method finds the most similar basic orientation.
"""
try:
   toRead = gdcm.File(file0)
   ValDict = GetValuesDict(toRead)
   try:
      imageOrientation=ValDict["Image Orientation (Patient)"]
   except KeyError:
      imageOrientation=ValDict["Image Orientation"]

   ori1=[float(split(imageOrientation,"\\")[0]),\
      float(split(imageOrientation,"\\")[1]),\
      float(split(imageOrientation,"\\")[2])]
   ori2=[float(split(imageOrientation,"\\")[3]),\
      float(split(imageOrientation,"\\")[4]),\
      float(split(imageOrientation,"\\")[5])]

## two vectors perpendicular describe one plane
   dicPlane=[ [  [1,0,0],[0,1,0]   ],  ## Axial
            [  [1,0,0],[0,0,-1]  ],  ## Coronal
            [  [0,1,0],[0,0,-1]  ],  ## Sagittal
            [  [ 0.8 , 0.5 ,  0.0 ],[-0.1 , 0.1 , -0.95]        ],## Axial - HEART
            [  [ 0.8 , 0.5 ,  0.0 ],[-0.6674 , 0.687 , 0.1794]  ],## Coronal - HEART
            [  [-0.1 , 0.1 , -0.95],[-0.6674 , 0.687 , 0.1794]  ] ] ## Sagittal - HEART

   i=0
   res=[0,99999]  ## [ <result> , <memory of the last succes calcule> ]
   for plane in dicPlane:
      i=i+1
      refA=plane[0]
      refB=plane[1]
      res=self.VerfCriterion(  i , self.CalculLikelyhood2Vec(refA,refB,ori1,ori2) , res )
      res=self.VerfCriterion( -i , self.CalculLikelyhood2Vec(refB,refA,ori1,ori2) , res )
   return res[0]

   except KeyError:
   return 0


   def VerfCriterion(self,typeCriterion,criterionNew,res):
      type = res[0]
      criterion = res[1]
#     if criterionNew<0.1 and criterionNew<criterion:
      if criterionNew<criterion:
         criterion=criterionNew
         type=typeCriterion
      return [ type , criterion ]


   def CalculLikelyhood2Vec(self,refA,refB,ori1,ori2):
"""
   # ------------------------- Purpose : -----------------------------------
   # - This function determine the orientation similarity of two planes.
   #   Each plane is described by two vector.
   # ------------------------- Parameters : --------------------------------
   # - <refA>  : - type : vector 3D (float)
   # - <refB>  : - type : vector 3D (float)
   #             - Description of the first plane
   # - <ori1>  : - type : vector 3D (float)
   # - <ori2>  : - type : vector 3D (float)
   #             - Description of the second plane
   # ------------------------- Return : ------------------------------------
   #  float :   0 if the planes are perpendicular. 
   # While the difference of the orientation between the planes 
   # are big more enlarge is
   # the criterion.
   # ------------------------- Other : -------------------------------------
   #  The calculus is based with vectors normalice
   """

      ori3=self.ProductVectorial(ori1,ori2)
      refC=self.ProductVectorial(refA,refB)
      res=math.pow(refC[0]-ori3[0],2) + math.pow(refC[1]-ori3[1],2) + math.pow(refC[2]-ori3[2],2)
      return math.sqrt(res)

   def ProductVectorial(self,vec1,vec2):
      """
      # ------------------------- Purpose : -----------------------------------
      # - Calculus of the poduct vectorial between two vectors 3D
      # ------------------------- Parameters : --------------------------------
      # - <vec1>  : - type : vector 3D (float)
      # - <vec2>  : - type : vector 3D (float)
      # ------------------------- Return : ------------------------------------
      #  (vec) :    - Vector 3D
      # ------------------------- Other : -------------------------------------
      """
      vec3=[0,0,0]
      vec3[0]=vec1[1]*vec2[2] - vec1[2]*vec2[1]
      vec3[1]=-( vec1[0]*vec2[2] - vec1[2]*vec2[0])
      vec3[2]=vec1[0]*vec2[1] - vec1[1]*vec2[0]
      return vec3

   def GetValuesDict(image):
      """
      Returns a dictionnary containing values associated with Field Names
      dict["Dicom Field Name"]="Dicom field value"
      """
      val=image.GetFirstEntry()
      dic={}
      while(val):
         if isinstance(val,gdcm.ValEntryPtr):
            dic[val.GetName()]=val.GetValue()
         val=image.GetNextEntry()
      return dic

*/
