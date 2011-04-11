/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkGdcm4DSplitter.cxx,v $
  Language:  C++
  Date:      $Date: 2011/04/11 11:28:31 $
  Version:   $Revision: 1.9 $
                                                                                
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


/* ====================================================================
vtkGdcm4DSplitter

3D, 2D+T, 3D+T, n*2D+T, 4D images are not always stored the same way :
        a single 'Dicom Serie', 
        several 'Dicom series' within a single directory
        several 'Dicom series' within several directories
A 'Dicom Serie' doesn't mean always the same thing :
        a given Slice along the time
        a given Volume at a given time
Sometimes, an image within a serie is so artefacted than user decides to replace
it by an other image.

User needs to be aware, *only him* knows want he wants to do.
vtkGdcm4DSplitter class does the job for hom
(despite its name, it works on 3D or 2D+T images too)

User will have to specify some points

. Choose input data
------------------- 

- a single directory
       bool setDirName(std::string &dirName);
- a list of directories
       bool setVectDirName(std::vector<std::string> &vectDirName);
- a list of files       
       bool setVectFileName(std::vector<std::string> &vectFileName);

- Recursive directory exploration
       void setRecursive(bool recursive);
 
. Choose 'split' criterion :
---------------------------

 - ImagePositionPatient
        void setSplitOnPosition();
 - ImageOrientationPatient
        void setSplitOnOrientation();
 - User choosen tag
        void setSplitOnTag(unsigned short splitGroup, unsigned short splitElem);
        void setSplitConvertToFloat(bool conv);
 - UserDefined Function
        void setSortOnUserFunction (FoncComp f);
 
 for 'true' 3D image sets :
   - if you want to get a single 3D vtkImageData, use SplitOnOrientation -i.e. no split-
   - if you want to get a vector of 2D vtkImageData, use SplitOnPosition  -i.e. one slice in each 'XCoherent filesite'-

 for 'true' 4D multi-orientation image sets (i.e. a stack of axial + sagital + coronal images, at different instants ...)
   --> this is 5D, right?
   Nothing done, yet.

 
. Choose 'sort' criterion :
--------------------------

 - ImagePositionPatient
        void setSortOnPosition(); 
 - ImageOrientationPatient
       ==> Only in your dreams!
       ==> or, please, write a IOP sorter ...
 - UserDefined Function
        void setSortOnUserFunction (FoncComp f);
 - File name
        void setSortOnFileName()
  
. Execute :
-----------
        bool Go();

. Get the result
----------------

// -a single vtkImageData:
//        vtkImageData *GetImageData();
- a vector of vtkImageData
        std::vector<vtkImageData*> *GetImageDataVector();

  ===================================================================== */

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
                 verbose(false) 
 {
 
 }

 std::vector<vtkImageData*> * vtkGdcm4DSplitter::GetImageDataVector() 
 { 
/*
 if (verbose) std::cout << "GetImageDataVector : TypeResult " << TypeResult << std::endl;
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
*/
     return ImageDataVector;
 }
 
 vtkImageData *vtkGdcm4DSplitter::GetImageData() 
 {
 /*
  if (verbose) std::cout << "GetImageData : TypeResult " << TypeResult << std::endl;
    if (TypeResult == 1)
       return ImageData;
    else
      if (TypeResult == 2)
      {
         return (*ImageDataVector)[0];      
      }
      else
         return (vtkImageData*) NULL;
*/
   return (*ImageDataVector)[0]; 
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
  /* if (verbose) printf ("%04x %04x\n", this->SortGroup,this->SortElem);
     if (verbose) std :: cout << file1->GetEntryString(SortGroup,SortElem).c_str() << " : " 
                            << atof(file1->GetEntryString(SortGroup,SortElem).c_str())
                            << std::endl;
*/
//   return atof(file1->GetEntryString(vtkGdcm4DSplitter::SortGroup,vtkGdcm4DSplitter::SortElem).c_str()) < atof(file2->GetEntryString(vtkGdcm4DSplitter::SortGroup,vtkGdcm4DSplitter::SortElem).c_str()); 
   return atof(file1->GetEntryString(SortGroup,SortElem).c_str()) < atof(file2->GetEntryString(SortGroup,SortElem).c_str()); 
 } 

 bool vtkGdcm4DSplitter::CompareOnSortTag(GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2)
 {
   return file1->GetEntryString(vtkGdcm4DSplitter::SortGroup,vtkGdcm4DSplitter::SortElem) < file2->GetEntryString(vtkGdcm4DSplitter::SortGroup,vtkGdcm4DSplitter::SortElem);  
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
      // reorg the std::map xcm according to position // JPR
      // the key of xcm follows lexicographical order
      // (that may be different than the 'distance' order)
      // we have to reorganize it!
      reorgXCoherentFileSetmap(xcm);
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
  // vtkGdcmReader *reader = vtkGdcmReader::New(); // move inside the loop, or be clever using vtk!
   
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
   
      vtkGdcmReader *reader = vtkGdcmReader::New(); /// \FIXME : unable to delete!
       
      if (verbose)
               std::cout << "==========================================xCoherentName = [" << (*i).first << "]" << std::endl;

      if (SortOnPosition)
      {
              if (verbose) std::cout << "SortOnPosition" << std::endl;
              // (will be IPPSorter, in GDCM2)
              s->ImagePositionPatientOrdering((*i).second);
              if (verbose) std::cout << "out of SortOnPosition" << std::endl;     
      }

      else if (SortOnOrientation)
      {
              if (verbose) std::cout << "SortOnOrientation" << std::endl;
            /// \TODO SortOnOrientation()
      
            // we still miss an algo to sort an Orientation, given by 6 cosines!
            //  Anything like this, in GDCM2? 
            std::cout << "SortOnOrientation : not so easy - I(mage)O(rientation)P(atient)Sorter still missing! -" << std::endl;
            // have a look at SerieHelper::SplitOnOrientation() to have an idea of the mess!

            //Better sort on the file name, right now...
             s->FileNameOrdering((*i).second);   
      }

      else if (SortOnFileName)
      {
         if (verbose) std::cout << "SortOnFileName" << std::endl;
         if (verbose) std::cout << "taille " << ((*i).second)->size() << std::endl;

         s->FileNameOrdering((*i).second);
         if (verbose) std::cout << "Out of SortOnFileName" << std::endl;
      }

      else if (SortOnTag)
      {  
         if (verbose) std::cout << "SortOnTag" << std::endl;   
         printf ("--> %04x %04x\n", SortGroup,SortElem);
         std::cout << "Sorry, troubles not solved yet; use SortOnUserFunction, right now!" << std::endl;
 
        /*        ==> WARNING : This one has troubles; do NOT use it, right now!
        // a pointer to fonction cannot be casted as a pointer to member function!
        // Use SortOnUserFunction, instead!

         if ( SortConvertToFloat )
            s->SetUserLessThanFunction( reinterpret_cast<bool (*)(gdcm13::File*, gdcm13::File*)> 
                                                                 ( &vtkGdcm4DSplitter::CompareOnSortTagConvertToFloat));     
         else
            s->SetUserLessThanFunction( reinterpret_cast<bool (*)(gdcm13::File*, gdcm13::File*)>
                                                                 ( &vtkGdcm4DSplitter::CompareOnSortTag)); 
       
         // Anything like this, in GDCM2? 
         s->UserOrdering((*i).second);
        */

         //if (verbose) std::cout << "Out of SortOnTag" << std::endl;
         std::cout << "NO ordering performed  :-( " << std::endl;
      }
      
      else if (SortOnUserFunction)
      {   
          if (verbose) std::cout << "SortOnUserFunction" << std::endl;
          s->SetUserLessThanFunction( UserCompareFunction );
         // Anything like this, in GDCM2? 
         s->UserOrdering((*i).second);
         if (verbose) std::cout << "Out of SortOnUserFunction" << std::endl;   
      }

       reader->SetCoherentFileList((*i).second);
       reader->Update();
       
       /// \TODO : remove the following
       if (verbose) {
          std::cout << "reader->GetOutput() :" << std::endl;
          reader->GetOutput()->PrintSelf(std::cout, vtkIndent(2));
       }
       
       ImageDataVector->push_back(reader->GetOutput() );
       
       std::vector<vtkImageData*>::iterator it; 
       if (verbose)      
       for(it=ImageDataVector->begin(); it!=ImageDataVector->end(); ++it) {
         std::cout << "-in vtkGdcm4DSplitter--------------------------" << std::endl;
         (*it)->PrintSelf(std::cout, vtkIndent(2));
       }
       std::cout << std::endl;
   }

   //reader->Delete();  // \TODO : fix
   s->Delete(); 
   f->Delete();
   delete l;
   
   return true;
 }


 void vtkGdcm4DSplitter::reorgXCoherentFileSetmap(GDCM_NAME_SPACE::XCoherentFileSetmap &xcm)
 {
   ELEM e;   
   std::vector<ELEM> vectElem;
/*
   typedef struct 
   {
      std::string strIPP;
      double dist;
      GDCM_NAME_SPACE::File *file;
   } ELEM;   
*/

   bool Debug=true;
   
   for (GDCM_NAME_SPACE::XCoherentFileSetmap::iterator i = xcm.begin(); 
                                                  i != xcm.end();
                                                ++i)
   {
      if (verbose)
               std::cout << "--- xCoherentName = [" << (*i).first << "]" << std::endl;

      e.strIPP  = (*i).first;
      e.file = *(((*i).second)->begin()); // all the gdcm::File of a given xcm item *have* the same IPP; first one is enough
      e.dist=0.0;
      vectElem.push_back(e);   
   }  
   sortVectElem(&vectElem);
   
   // now, create the final std::map !
   // final_xcm<to_str(e.dist , xcm[e.strIPP]>
   // xcm = final_xcm;
   // check what we need to free !

   int dist;
   char c_dist[100];
   std::string str_dist;
   int lgr=vectElem.size();
   GDCM_NAME_SPACE::XCoherentFileSetmap final_xcm;
   for (int i2=0; i2<lgr; i2++)
   { 
      dist =  (vectElem[i2].dist*1000);
      sprintf(c_dist,"%010d",dist);
      str_dist = c_dist;
/*
      std::cout << "dist " << vectElem[i2].dist 
                << " str_dist " << str_dist
                << " IPP " << vectElem[i2].strIPP
                << std::endl;

*/     
      final_xcm[str_dist] = xcm[vectElem[i2].strIPP];
   } 
   
   /// \TODO : check what needs to be cleared // JPR

   xcm = final_xcm;
   
 }


bool vtkGdcm4DSplitter::sortVectElem(std::vector<ELEM> *fileList)
{
//based on Jolinda Smith's algorithm

//Tags always use the same coordinate system, where "x" is left
//to right, "y" is posterior to anterior, and "z" is foot to head (RAH).

   //iop is calculated based on the file file
   float cosines[6];
   double normal[3];
   double ipp[3];
   double dist;
   double min = 0, max = 0;
   bool first = true;
   
   double ZSpacing; // useless here! // JPR
   bool DirectOrder = true; // remove it!
   
   ZSpacing = -1.0;  // will be updated if process doesn't fail
    
   //std::multimap<double,File *> distmultimap; // JPR
   std::multimap<double,ELEM> distmultimap; // JPR
   
   // Use a multimap to sort the distances from 0,0,0
   //for ( FileList::const_iterator // JPR
   for ( std::vector<ELEM>::iterator   // JPR
         it = fileList->begin();
         it != fileList->end(); ++it )
   {
      //gdcmDebugMacro("deal with " << (*it)->file->GetFileName() );
      if ( first ) 
      {
         (*it).file->GetImageOrientationPatient( cosines );

   // The "Image Orientation Patient" tag gives the direction cosines 
   // for the rows and columns for the three axes defined above. 
   // Typical axial slices will have a value 1/0/0/0/1/0: 
   // rows increase from left to right, 
   // columns increase from posterior to anterior. This is your everyday
   // "looking up from the bottom of the head with the eyeballs up" image. 
   
   // The "Image Position Patient" tag gives the coordinates of the first
   // voxel in the image in the "RAH" coordinate system, relative to some
   // origin.   

   // First, calculate the slice normal from IOP : 
          
         // You only have to do this once for all slices in the volume. Next, 
         // for each slice, calculate the distance along the slice normal 
         // using the IPP ("Image Position Patient") tag.
         // ("dist" is initialized to zero before reading the first slice) :

         normal[0] = cosines[1]*cosines[5] - cosines[2]*cosines[4];
         normal[1] = cosines[2]*cosines[3] - cosines[0]*cosines[5];
         normal[2] = cosines[0]*cosines[4] - cosines[1]*cosines[3];

   // For each slice (here : the first), calculate the distance along 
   // the slice normal using the IPP tag 
    
         ipp[0] = (*it).file->GetXOrigin();
         ipp[1] = (*it).file->GetYOrigin();
         ipp[2] = (*it).file->GetZOrigin();

         dist = 0;
         for ( int i = 0; i < 3; ++i )
         {
            dist += normal[i]*ipp[i];
         }
    
         //gdcmDebugMacro("dist : " << dist);
         distmultimap.insert(std::pair<const double,ELEM>(dist, *it));

         max = min = dist;
         first = false;
      }
      else 
      {
   // Next, for each slice, calculate the distance along the slice normal
   // using the IPP tag 
         ipp[0] = (*it).file->GetXOrigin();
         ipp[1] = (*it).file->GetYOrigin();
         ipp[2] = (*it).file->GetZOrigin();

         dist = 0;
         for ( int i = 0; i < 3; ++i )
         {
            dist += normal[i]*ipp[i];
         }

         (*it).dist = dist; // JPR

         distmultimap.insert(std::pair<const double,ELEM>(dist, *it));
         //gdcmDebugMacro("dist : " << dist);
         min = (min < dist) ? min : dist;
         max = (max > dist) ? max : dist;
      }
   }

  // gdcmDebugMacro("After parsing vector, nb of elements : " << fileList->size() );

/*Useless here. // JPR

   // Find out if min/max are coherent
   if ( min == max )
   {
     gdcmWarningMacro("Looks like all images have the exact same image position. "
                      << "No PositionPatientOrdering sort performed. "
                      << "No 'ZSpacing' calculated! ");
     return false;
   }
*/

/* Useless here, 'split' already done. // JPR

   // Check to see if image shares a common position
   bool ok = true;
   for (std::multimap<double, File *>::iterator it2 = distmultimap.begin(); 
        it2 != distmultimap.end();
        ++it2)
   {
   
      gdcmDebugMacro("Check if image shares a common position : " << ((*it2).second).file->GetFileName() );   
   
      if (distmultimap.count((*it2).first) != 1)
      {
         gdcmWarningMacro("File: ["
              << ((*it2).second->GetFileName())
              << "] : more than ONE file at distance: '"
              << (*it2).first
              << " (position is not unique!) "
              << "No PositionPatientOrdering sort performed. "
              << "No 'ZSpacing' calculated! ");      

         ok = false;
      }
   }
   if (!ok)
   {
      if (! DropDuplicatePositions)
         return false;
   }

*/
      
// Now, we can calculate Z Spacing as the difference
// between the "dist" values for the first two slices.

// The following (un)-commented out code is let here
// to be re-used by whomsoever is interested...

    std::multimap<double, ELEM>::iterator it5 = distmultimap.begin();
    double d1 = (*it5).first;
    it5++;
    double d2 = (*it5).first;
    ZSpacing = d1-d2;
    if (ZSpacing < 0.0)
       ZSpacing = - ZSpacing;

   fileList->clear();  // doesn't delete list elements, only nodes

// Acording to user requierement, we sort direct order or reverse order.
   if (DirectOrder)
   {  
      for (std::multimap<double, ELEM>::iterator it3 = distmultimap.begin();
           it3 != distmultimap.end();
           ++it3)
      {
         fileList->push_back( (*it3).second );
 
/*       useless here! // JPR

         if (DropDuplicatePositions)
         {
            // ImagePositionPatientOrdering  wrong duplicates are found ???
            // --> fixed. See comment

            it3 =  distmultimap.upper_bound((*it3).first); // skip all duplicates
           // the upper_bound function increments the iterator to the next non-duplicate entry
           // The for loop iteration also increments the iterator, which causes the code to skip every other image
           // --> decrement the iterator after the upper_bound function call
            it3--;
            if (it3 == distmultimap.end() )  // if last image, stop iterate
               break;
         }
*/
      }
   }
   else // user asked for reverse order
   {
      std::multimap<double, ELEM>::const_iterator it4;
      it4 = distmultimap.end();
      do
      {
         it4--;
         fileList->push_back( (*it4).second );

/* useless here // JPR

         if (DropDuplicatePositions)  // skip all duplicates
         {
            // lower_bound finds the next element that is 
            // less than or *equal to* the current value!
            //it4 =  distmultimap.lower_bound((*it4).first);
   
           // David Feng's fix
           std::multimap<double, ELEM>::const_iterator itPrev = it4;
           while (itPrev->first == it4->first)
              --itPrev;
           it4 = itPrev;
    
           if (it4 == distmultimap.begin() ) // if first image, stop iterate
               break;
         }
*/ 
      } while (it4 != distmultimap.begin() );
   }

   distmultimap.clear();

   return true;

}
