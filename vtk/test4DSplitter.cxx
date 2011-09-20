/*=========================================================================

  Program:   gdcm
  Module:    $RCSfile: test4DSplitter.cxx,v $
  Language:  C++
  Date:      $Date: 2011/09/20 16:09:05 $
  Version:   $Revision: 1.8 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                 
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

/* Raisons ne pas utiliser itkImageSeriesReader:

On Wed, Feb 16, 2011 at 11:51 AM, Roger Bramon Feixas <rogerbramon@gmail.com>
    Hi,
    I'm developing with ITK 3.20 + GDCM 2.0.17 + VTK 5.6 and I've noticed 
    itkImageSeriesReader is ~2x slower than vtkGDCMImageReader (from GDCM2). 
    I compared both codes and I think the difference is the extra copy which 
    itkImageSeriesReader makes from ImageFileReader's output to its own output 
    (ImageSeriesReader::GenerateData() line 393). I've commented it just to test 
    the time needed without making the copy and now both readers take more or less 
    the same time.
*/

#include "gdcmFile.h"
#include "gdcmSerieHelper.h"
#include <vtkImageData.h>
#include <iostream>
#include "vtkGdcmReader.h"

#include <algorithm>
#include "vtkGdcm4DSplitter.h"
    
/**
  * \brief
  */

typedef std::map<std::string, GDCM_NAME_SPACE::File*> SortedFiles;

// Due to a deep C++ trouble in 'SortOnTag', we use 'SortOnUserFunction'
bool myCompareFunction(GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2)
 { 
   return atof(file1->GetEntryString(0x0018,0x1060).c_str()) < atof(file2->GetEntryString(0x0018,0x1060).c_str()); 
 }
 
bool myCompareFunction0008_0032(GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2)
 { 
   return atof(file1->GetEntryString(0x0008,0x0032).c_str()) < atof(file2->GetEntryString(0x0008,0x0032).c_str()); 
 } 
int main(int argc, char *argv[])
{
  
  
   std::cout << "... inside : " << argv[0] << std::endl;

// 3D     
//std::string strDirName("/home/jpr/Desktop/Patients_Emilie/Patient.3T/AUB Jos/AUBERTIN JOSEPH/PROSTATE - 305629373/dSSh_DWISENSE_602");

// 4D
//std::string strDirName("/home/jpr/Desktop/Patients_Emilie/Patient.3T/AUB Jos/AUBERTIN JOSEPH/PROSTATE - 305629373/DYN7INJDYN6_901");

// n directories 
std::string strDirName("/home/jpr/Desktop/patient Andrei Dyn/dyn");
//std::string strDirName("/home/jpr/Desktop/patient Andrei Dyn/dyn2"); // very small sample
   // ----- Begin Processing -----  
   
   unsigned short int grSplit;
   unsigned short int elSplit;

   unsigned short int grSort;
   unsigned short int elSort;

// Pour un directory '4D'
// en sortie, chaque vtkImageData contiendra une coupe au cours du temps
// n * 2D+T

  std::vector<vtkImageData*> *output;
  std::vector<vtkImageData*>::iterator it;

//  ========================================
//  Split on Position (IPP)
//  Sort on UserFunction (use 0008|0032 : Aquisition Time )
//
// Should give a vector of 2D+T vtkImageData*
//  ========================================

//vtkGdcm4DSplitter *spl = new vtkGdcm4DSplitter();

if (1) {

   std::cout << "Try with :  Split on Position (IPP) / Sort on UserFunction (use 0008|0032 : Aquisition Time )" << std::endl;

   vtkGdcm4DSplitter *spl = new vtkGdcm4DSplitter();
   spl->setFlipY(false);
   spl->setDirName(strDirName);
   spl->setRecursive(true);
   
   spl->setSplitOnPosition();
   
   // Time triger : 0018|1060
   //grSort=0x0018;
   //elSort=0x1060;
      
   // ==> use SortOnUserFunction !
   spl->setSortOnUserFunction(myCompareFunction0008_0032);  

   std::cout << "Everything set" << std::endl;  
   bool res=spl->Go();
   
    std::cout << "GO() done, status " << res << std::endl;
    if(!res)
    {
       std::cout << "plantage!" << std::endl;
    } 

   output = spl->GetImageDataVector();

std::cout << "--------------------------------" << std::endl;
std::cout << "Vector size " << output->size()   << std::endl;
std::cout << "--------------------------------" << std::endl;

// Print the first one (why not?)
//(*output)[0]->PrintSelf(std::cout, vtkIndent(2));

   for(it=output->begin(); it!=output->end(); ++it) {
      //std::cout << "========================================" << std::endl;
      //(*it)->PrintSelf(std::cout, vtkIndent(2));
   }

   delete spl;

   // To please valgring  
   std::vector<vtkImageData*>::iterator it2;       
   for ( it2 = output->begin(); // for each vtkImageData*
             it2 != output->end();
           ++it2)
   {
           (*it2)->Delete(); // delete vtkImageData
   }       
   delete output;    
} 


for(int i=0; i<3; i++)
   std::cout << "---------------------------------------------" << std::endl;



//  ========================================
//  Split on Tag 0008|0032 (Aquisition Time)
//  Sort on Position (IPP)
//
// Should give a vector of 'true 3D' vtkImageData*
//  ========================================
 
 if (1) {
 
   std::cout << "Try with :  Split on Tag 0008|0032 (Aquisition Time) / Sort on Position (IPP)" << std::endl; 

   vtkGdcm4DSplitter *spl = new vtkGdcm4DSplitter();
   spl->setFlipY(false);
   spl->setDirName(strDirName);
   spl->setRecursive(true);
   
   // Time triger : 0018|1060
   //grSort=0x0018;
   //elSort=0x1060;
 
   // Aquisition Time  : 0008|0032
   spl->setSplitOnTag(0x0008, 0x0032); 
   
   spl->setSortOnPosition();

   std::cout << "Everything set" << std::endl;  
   bool res=spl->Go();
   
    std::cout << "GO() done, status " << res << std::endl;
    if(!res)
    {
       std::cout << "plantage!" << std::endl;
    } 

   output = spl->GetImageDataVector();


std::cout << "--------------------------------" << std::endl;
std::cout << "Vector size " << output->size()   << std::endl;
std::cout << "--------------------------------" << std::endl;

// Print the first one (why not?)
//(*output)[0]->PrintSelf(std::cout, vtkIndent(2));


   for(it=output->begin(); it!=output->end(); ++it) {
      //std::cout << "========================================" << std::endl;
      //(*it)->PrintSelf(std::cout, vtkIndent(2));
   }
   
   delete spl;

   // To please valgring  
   std::vector<vtkImageData*>::iterator it2;       
   for ( it2 = output->begin(); // for each vtkImageData*
             it2 != output->end();
           ++it2)
   {
           (*it2)->Delete(); // delete vtkImageData
   }       
   delete output;
}

// Pour un directory '4D'
// en sortie, chaque  vtkImageData contiendra un volume au cours du temps.
// 3D + T


}
