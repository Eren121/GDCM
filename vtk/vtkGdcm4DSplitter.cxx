/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkGdcm4DSplitter.cxx,v $
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

/* Raisons ne pas utiliser itkImageSeriesReader:

On Wed, Feb 16, 2011 at 11:51 AM, Roger Bramon Feixas <rogerbramon@gmail.com>
    Hi,
    I'm developing with ITK 3.20 + GDCM 2.0.17 + VTK 5.6 and I've noticed 
    itkImageSeriesReader is ~2x slower than vtkGDCMImageReader (from GDCM2). 
    I compared both codes and I think the difference is the extra copy which 
    itkImageSeriesReader makes from ImageFileReader's output to its own output 
    (ImageSeriesReader::GenerateData() line 393).
*/

#include "gdcmSerieHelper.h"

#include "vtkGdcmReader.h"
#include "vtkGdcm4DSplitter.h"
#include <algorithm>
#include "gdcmSerieHelper.h" // for ImagePositionPatientOrdering()
#include <stdlib.h> // for atof

 vtkGdcm4DSplitter::vtkGdcm4DSplitter() :
                 SplitOnPosition(false), SplitOnOrientation(false), SplitOnTag(false),
                 SplitGroup(0), SplitElem(0),

                 SortOnPosition(false),  SortOnOrientation(false),  SortOnTag(false), 
                 SortGroup(0),  SortElem(0), SortConvertToFloat(false),

                 Recursive(false), TypeDir(0), 
                 verbose(true) 
 {
 
 }

 std::vector<vtkImageData*> * vtkGdcm4DSplitter::GetImageDataVector() 
 {
 if (verbose) std::cout << "TypeDir " << TypeDir << std::endl;
    if (TypeResult == 2)
       return ImageDataVector;
    else
      if (TypeResult == 1)
      {
         std::vector<vtkImageData*> *t = new std::vector<vtkImageData*>; 
         t->push_back( ImageData );
         return t;            
      }
      else
         return (std::vector<vtkImageData*>*) NULL;
 }
 
 vtkImageData *vtkGdcm4DSplitter::GetImageData() 
 {
    if (TypeResult == 1)
       return ImageData;
    else
      if (TypeResult == 1)
      {
         return (*ImageDataVector)[0];      
      }
      else
         return (vtkImageData*) NULL;
 }
       
       
 bool vtkGdcm4DSplitter::setDirName(std::string &dirName) 
 {
    if ( ! GDCM_NAME_SPACE::DirList::IsDirectory(dirName) ) 
    {
       std::cout << "[" << dirName << "] is NOT a directory" << std::endl;
       return false;
    }
    DirName = dirName; 
    TypeDir=1;
    return true;
 }
 
 bool vtkGdcm4DSplitter::setVectDirName(std::vector<std::string> &vectDirName) 
 {
    int nbDir = vectDirName.size();
    for (int iDir=0; iDir<nbDir; iDir++)
    {
       if ( ! GDCM_NAME_SPACE::DirList::IsDirectory(vectDirName[iDir]) ) 
       {
          std::cout << "[" << vectDirName[iDir] << "] is NOT a directory" << std::endl;
          return false;
       }
    }   

    VectDirName = vectDirName; 
    TypeDir=2;
    return true;
 }
 
 bool vtkGdcm4DSplitter::setVectFileName(std::vector<std::string> &vectFileName)
 {
    if ( vectFileName.size() == 0)
    {
          std::cout << "[ vectFileName ] : empty list" << std::endl;
          return false;
    }
    VectFileName = vectFileName;
    TypeDir=3;
    return true;
 }      

 bool vtkGdcm4DSplitter::CompareOnSortTagConvertToFloat(GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2)
 { 
   if (verbose) printf ("%04x %04x\n", this->SortGroup,this->SortElem);
   if (verbose) std :: cout << file1->GetEntryString(SortGroup,SortElem).c_str() << " : " 
                            << atof(file1->GetEntryString(SortGroup,SortElem).c_str())
                            << std::endl;
   return atof(file1->GetEntryString(SortGroup,SortElem).c_str()) < atof(file2->GetEntryString(SortGroup,SortElem).c_str());  
 } 

 bool vtkGdcm4DSplitter::CompareOnSortTag(GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2)
 {
   return file1->GetEntryString(SortGroup,SortElem) < file2->GetEntryString(SortGroup,SortElem);  
 }
   
 bool vtkGdcm4DSplitter::Go()
 {
   if (!SplitOnPosition && !SplitOnOrientation && !SplitOnTag ) 
   {
       ///\TODO (?) Throw an exception "Choose Splitting mode before!"
       std::cout << "Choose Splitting mode before!" << std::endl;
       return false;
   }

   /// How To :
   /*
   entree nom de directory / Vecteur de noms?
   recursif oui/non
   recuperer la liste des gdcm::File*
   passer a SerieHelper (pas de check du Serie UID)
   set critere de split
   
   trier chaque Coherent file set
   passer chacun a un vtkGcdmReader
   retourner le (vecteur de) vtkImageData
   */
   
   GDCM_NAME_SPACE::SerieHelper *s;  
   s = GDCM_NAME_SPACE::SerieHelper::New();

   GDCM_NAME_SPACE::File *f;
   GDCM_NAME_SPACE::DirListType fileNames;
   
   if (TypeDir == 0 )
   {
      ///\TODO (?) Throw an exception "Set input Directory name(s) / file names  before!"
      std::cout << "Set input Directory name(s) / file names  before!" << std::endl;
      return false;
   }
   else if (TypeDir == 1)
   {
      GDCM_NAME_SPACE::DirList dirlist(DirName, Recursive); // NO recursive exploration
      fileNames = dirlist.GetFilenames(); // all the file names
   }
   
   else if (TypeDir == 2)
   {
      int nbDir = VectDirName.size();
      GDCM_NAME_SPACE::DirListType tmpFileNames;
      for (int iDir=0; iDir<nbDir; iDir++)
      {
        GDCM_NAME_SPACE::DirList dirlist(VectDirName[iDir], Recursive);
        tmpFileNames = dirlist.GetFilenames();
        // Concat two std::vector
        //vector1.insert( vector1.end(), vector2.begin(), vector2.end() );
       fileNames.insert( fileNames.end(), tmpFileNames.begin(), tmpFileNames.end() );
      }    
   }
   else if (TypeDir == 3)
   {
      fileNames=VectFileName;
   }  

   GDCM_NAME_SPACE::FileList *l = new GDCM_NAME_SPACE::FileList; // (set of gdcm::File)
   double floatTagvalue;  
   // Loop on all the gdcm-readable files
   for (GDCM_NAME_SPACE::DirListType::iterator it = fileNames.begin();
                                    it != fileNames.end();
                                  ++it)
   {
      int maxSize  = 0x7fff;         // load Elements of any length
      f = GDCM_NAME_SPACE::File::New();
      f->SetMaxSizeLoadEntry(maxSize);
      f->SetFileName( *it );
      if (f->Load())
         l->push_back(f);
      else 
         std::cout << " Fail to load [" <<  *it << "]" << std::endl;          
   }   

   GDCM_NAME_SPACE::XCoherentFileSetmap xcm;

   if (SplitOnOrientation) 
   {
            s->SetDropDuplicatePositions(false);
            xcm = s->SplitOnOrientation(l);
   }
   else if (SplitOnPosition)
   {
            s->SetDropDuplicatePositions(true);
            xcm = s->SplitOnPosition(l);
   }
   else if (SplitOnTag) 
   {
         s->SetDropDuplicatePositions(false);

         // Crashes if DataElement not found
         //std:: cout << GDCM_NAME_SPACE::Global::GetDicts()->GetDefaultPubDict()->GetEntry(groupelem[0],groupelem[1])->GetName() << std::endl;
            if ( ! SplitConvertToFloat )
               xcm = s->SplitOnTagValue(l, SplitGroup, SplitElem);
            else 
            {
                xcm = s->SplitOnTagValueConvertToFloat(l, SplitGroup, SplitElem);
            }
   }
   
   if (xcm.size() == 0)
   {
      if(verbose) std::cout << "Empty XCoherent File Set after 'split' ?!?" << std::endl;
      return false;
   }
   else if (xcm.size() == 1)
      TypeResult=1;
   else
      TypeResult=2;

   ImageDataVector = new std::vector<vtkImageData*>;
   vtkGdcmReader *reader = vtkGdcmReader::New();
   for (GDCM_NAME_SPACE::XCoherentFileSetmap::iterator i = xcm.begin(); 
                                                  i != xcm.end();
                                                ++i)
   {
           if (verbose)
               std::cout << "--- xCoherentName = [" << (*i).first << "]" << std::endl;
   }
 // XCoherentFileSetmap map < critère de split, FileList (= std::vector de gdcm::File*) >

   for (GDCM_NAME_SPACE::XCoherentFileSetmap::iterator i = xcm.begin(); 
                                                  i != xcm.end();
                                                ++i)
   {
           if (verbose)
               std::cout << "==========================================xCoherentName = [" << (*i).first << "]" << std::endl;

           if (SortOnPosition)
           {
              if (verbose) std::cout << "SortOnPosition" << std::endl;
              // (will be IPPSorter, in GDCM2)
              s->ImagePositionPatientOrdering((*i).second);
              if (verbose) std::cout << "out of SortOnPosition" << std::endl      
            }

           if (SortOnOrientation)
           {
              if (verbose) std::cout << "SortOnOrientation" << std::endl;
             /// \TODO SortOnOrientation()
           // Within a 'just to see' program, 
           // OrderFileList() causes trouble, since some files
           // (eg:MIP views) don't have 'Position', now considered as mandatory
           // --> Activated on user demand.
   
           // Information is in :      
           // 0020,0032 : Image Position Patient
           // 0020,0030 : Image Position (RET)
      
            // we still miss an algo to sort an Orientation, given by 6 cosines!
    //  Anything like this, in GDCM2? 
      std::cout << "SortOnOrientation : not so easy - I(mage)O(rientation)P(atient)Sorter still missing! -" << std::endl;
    // have a look at SerieHelper::SplitOnPosition() to have an idea of the mess!

      //Better sort on the file name, right now...
      s->FileNameOrdering((*i).second);   
   }

   if (SortOnFileName)
   {
      if (verbose) std::cout << "SortOnFileName" << std::endl;
      if (verbose) std::cout << "taille " << ((*i).second)->size() << std::endl;
      s->FileNameOrdering((*i).second);
      if (verbose) std::cout << "Out of SortOnFileName" << std::endl;
   }

   if (SortOnTag)
   {  
      if (verbose) std::cout << "SortOnTag" << std::endl;   
      printf ("--> %04x %04x\n", SortGroup,SortElem);
              if ( SortConvertToFloat )
         s->SetUserLessThanFunction( reinterpret_cast<bool (*)(gdcm13::File*, gdcm13::File*)> 
                                                              ( &vtkGdcm4DSplitter::CompareOnSortTagConvertToFloat));     
      else
         s->SetUserLessThanFunction( reinterpret_cast<bool (*)(gdcm13::File*, gdcm13::File*)>
                                                              ( &vtkGdcm4DSplitter::CompareOnSortTag)); 
       
      // Anything like this, in GDCM2? 
      s->UserOrdering((*i).second);
      if (verbose) std::cout << "Out of SortOnTag" << std::endl;
   }

    reader->SetCoherentFileList((*i).second);
    reader->Update();
            ImageDataVector->push_back(reader->GetOutput() );

         std::cout << std::endl;
   }

   reader->Delete();
   s->Delete(); 
   f->Delete();
   delete l;
 }

