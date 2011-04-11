/*=========================================================================

  Program:   gdcm
  Module:    $RCSfile: test4DSplitter.cxx,v $
  Language:  C++
  Date:      $Date: 2011/04/11 11:28:31 $
  Version:   $Revision: 1.6 $
                                                                                
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

int main(int argc, char *argv[])
{
  
  
   std::cout << "... inside " << argv[0] << std::endl;

// 3D     
std::string strDirName("/home/jpr/Desktop/Patients_Emilie/Patient.3T/AUB Jos/AUBERTIN JOSEPH/PROSTATE - 305629373/dSSh_DWISENSE_602");

// 4D
//std::string strDirName("/home/jpr/Desktop/Patients_Emilie/Patient.3T/AUB Jos/AUBERTIN JOSEPH/PROSTATE - 305629373/DYN7INJDYN6_901");
   

   // ----- Begin Processing -----  
   
   unsigned short int grSplit;
   unsigned short int elSplit;

   unsigned short int grSort;
   unsigned short int elSort;

// Pour un directory '4D'
// en sortie, chaque vtkImageData contiendra une coupe au cours du temps
// n * 2D+T

std::vector<vtkImageData*> *output;

 if (1) {
   vtkGdcm4DSplitter *spl = new vtkGdcm4DSplitter();
   spl->setDirName(strDirName);
   spl->setRecursive(false);
   spl->setSplitOnPosition();
   //spl->setSplitOnOrientation();
   // Time triger : 0018|1060
   grSort=0x0018;
   elSort=0x1060;
   
   // ==> Big troubles with SortOnTag
   //spl->setSortOnTag(grSort, elSort);
   //spl->setSortConvertToFloat(true); 
      
   // ==> use SortOnUserFunction !
   spl->setSortOnUserFunction(myCompareFunction);   

   std::cout << "Everything set" << std::endl;  
   bool res=spl->Go();
   
    std::cout << "GO() done, status " << res << std::endl;
    if(!res)
    {
       std::cout << "plantage!" << std::endl;
    } 

   output = spl->GetImageDataVector();
}

std::cout << "--------------------------------" << std::endl;
std::cout << "Vector size " << output->size()   << std::endl;
std::cout << "--------------------------------" << std::endl;

// Print the first one (why not?)
//(*output)[0]->PrintSelf(std::cout, vtkIndent(2));

 std::vector<vtkImageData*>::iterator it;
 for(it=output->begin(); it!=output->end(); ++it) {
   std::cout << "========================================" << std::endl;
   (*it)->PrintSelf(std::cout, vtkIndent(2));
 }

// Pour un directory '4D'
// en sortie, chaque  vtkImageData contiendra un volume au cours du temps.
// 3D + T

}
  


