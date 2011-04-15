/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkGdcm4DSplitter.h,v $
  Language:  C++
  Date:      $Date: 2011/04/15 15:23:14 $
  Version:   $Revision: 1.11 $
                                                                                
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
#include "gdcmFile.h"
#include "gdcmSerieHelper.h"

typedef  bool (*FoncComp)(GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2);

#define CALL_MEMBER_FONC(object, ptrToFoncMember)  ((object).*(ptrToFoncMember))

   typedef struct 
   {
      std::string strIPP;
      double dist;
      GDCM_NAME_SPACE::File *file;
   } ELEM;
   
   
//namespace GDCM_NAME_SPACE
//{
  class vtkGdcm4DSplitter {
    public:
//-----------------------------------------------------------------------------

       vtkGdcm4DSplitter();
       ~vtkGdcm4DSplitter();

       // Locate Data to process
       // ======================
       void setRecursive(bool recursive);
       bool setDirName     (std::string &dirName);
       bool setVectDirName (std::vector<std::string> &vectDirName);
       bool setVectFileName(std::vector<std::string> &vectFileName);
       
       bool setVectGdcmFile(std::vector<GDCM_NAME_SPACE::File *> *vectGdcmFile);
       
       void SetFlipY(bool f) { FlipY = f; }

       // Split
       // =====
       void setSplitOnPosition();
       void setSplitOnOrientation();
       void setSplitOnTag(unsigned short int splitGroup, unsigned short int splitElem);

       void setSplitConvertToFloat(bool conv);

       void setSplitOnly(bool s);
       
       // Sort
       // ====
       void setSortOnPosition();
      // use setSortOnUserFunction, instead!
      // void setSortOnTag(unsigned short int sortGroup, unsigned short int sortElem)
      //                                   {SortOnPosition=false; SortOnOrientation=false; SortOnTag=true; SortOnFileName=false; SortOnUserFunction=false;
      //                                    SortGroup=sortGroup;  SortElem=sortElem;}

       void setSortOnUserFunction (FoncComp f);

       //void setSortConvertToFloat(bool conv);

       void setSortOnFileName();

       // SortOnOrientation : not yet made; IOP sorter missing!
       //inline void setSortOnOrientation(){SortOnPosition=false; SortOnOrientation=true;  SortOnTag=false; SortOnFileName=false; SortOnUserFunction=false;}

        std::vector<vtkImageData*> *GetImageDataVector();
        vtkImageData *GetImageData();

        std::vector< GDCM_NAME_SPACE::FileList *> *GetVectGdcmFileLists();
        
        bool Go();

    protected:
    private:
       bool CompareOnSortTag              (GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2);
       bool CompareOnSortTagConvertToFloat(GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2);
       void reorgXCoherentFileSetmap      (GDCM_NAME_SPACE::XCoherentFileSetmap &xcm);
       bool sortVectElem(std::vector<ELEM> *le);

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
       bool SplitOnly;
 
       bool SortOnPosition;
       bool SortOnOrientation;
       bool SortOnTag;
       bool SortOnFileName;
       bool SortOnUserFunction;

       FoncComp UserCompareFunction;

       unsigned short int SortGroup;
       unsigned short int SortElem;
       bool SortConvertToFloat;

       bool Recursive;
       int TypeDir;
       //int TypeResult;
       bool verbose;
       std::vector<vtkImageData*> *ImageDataVector;
       vtkImageData *ImageData;
       //std::vector<GDCM_NAME_SPACE::File *> VectGdcmFile;
       GDCM_NAME_SPACE::FileList *VectGdcmFile; 
       GDCM_NAME_SPACE::XCoherentFileSetmap xcm;
       //std::vector<std::vector<GDCM_NAME_SPACE::File *> > VectGdcmFileLists;
       std::vector<GDCM_NAME_SPACE::FileList *> VectGdcmFileLists;
       bool FlipY;
  };

//} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
