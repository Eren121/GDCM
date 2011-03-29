/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkGdcm4DSplitter.h,v $
  Language:  C++
  Date:      $Date: 2011/03/29 12:49:27 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef _VTKGDCM4DSPLITTER_H_
#define _VTKGDCM4DSPLITTER_H_

#include <vector>
#include <vtkImageData.h>
#include "gdcmDirList.h" 

//namespace GDCM_NAME_SPACE 
//{ 
  class vtkGdcm4DSplitter {
    public:
    
       vtkGdcm4DSplitter();
       ~vtkGdcm4DSplitter(){};       
       bool setDirName(std::string &dirName);
       bool setVectDirName(std::vector<std::string> &vectDirName);
       bool setVectFileName(std::vector<std::string> &vectFileName);       
       
       inline void setSplitOnPosition()   {SplitOnPosition=true;  SplitOnOrientation=false; SplitOnTag=false;}
       inline void setSplitOnOrientation(){SplitOnPosition=false; SplitOnOrientation=true;  SplitOnTag=false;}
       inline void setSplitOnTag(unsigned short int splitGroup, unsigned short int splitElem)       
                                          {SplitOnPosition=false; SplitOnOrientation=false; SplitOnTag=true; 
                                           SplitGroup=splitGroup;    SplitElem=splitElem;}
       inline void setSplitConvertToFloat(bool conv) {SplitConvertToFloat=conv;}
       
       inline void setSortOnPosition()   {SortOnPosition=true;  SortOnOrientation=false; SortOnTag=false; SortOnFileName=false;}
       // SortOnOrientation : not yet made
       //inline void setSortOnOrientation(){SortOnPosition=false; SortOnOrientation=true;  SortOnTag=false; SortOnFileName=false;}     
       inline void setSortOnTag(unsigned short int sortGroup, unsigned short int sortElem)       
                                         {SortOnPosition=false; SortOnOrientation=false; SortOnTag=true; SortOnFileName=false;
                                          SortGroup=sortGroup;     SortElem=sortElem;}
       inline void setSortConvertToFloat(bool conv) {SortConvertToFloat=conv;}
                                    
       inline void setSortOnFileName()   {SortOnPosition=false; SortOnOrientation=false; SortOnTag=false; SortOnFileName=true;}
       inline void setRecursive(bool recursive) { Recursive=recursive;}

        std::vector<vtkImageData*> *GetImageDataVector();
        vtkImageData *GetImageData();

        bool Go();  
                
    protected:
    private:
       bool CompareOnSortTag              (GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2);
       bool CompareOnSortTagConvertToFloat(GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2); 
               
    // Data
    // ----
    public:  
    protected:
    private:
       std::string DirName;
       std::vector<std::string> VectDirName;
       std::vector<std::string> VectFileName;
       
       bool SplitOnPosition;
       bool SplitOnOrientation;
       bool SplitOnTag;
 
       unsigned short int SplitGroup;
       unsigned short int SplitElem;
       bool SplitConvertToFloat;
                    
       bool SortOnPosition;
       bool SortOnOrientation;
       bool SortOnTag;    
       bool SortOnFileName;

       unsigned short int SortGroup;
       unsigned short int SortElem;
       bool SortConvertToFloat;
              
       bool Recursive;
       int TypeDir;
       int TypeResult;
       bool verbose;
       std::vector<vtkImageData*> *ImageDataVector;
       vtkImageData *ImageData;               
  };
 
//} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
