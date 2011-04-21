/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkGdcm4DSplitter.cxx,v $
  Language:  C++
  Date:      $Date: 2011/04/21 09:14:31 $
  Version:   $Revision: 1.14 $
                                                                                
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
Sometimes, an image within a serie is so artefacted than user decides to replace it
           by an other image.

User needs to be aware, *only him* knows want he wants to do.
vtkGdcm4DSplitter class does the job for him
(despite its name, it works on 3D or 2D+T images too)

==> To (try to) understand how the file store is organised, user is deeply encourage to use something like:

PrintFile dirin=Root_Directory_holding_the_images rec > 1.txt

open it with any test editor, and/or (Linux users) 
grep a_supposed_to_be_string_of_interest 1.txt

 
Aware user will have to specify some points :

. Choose input data
------------------- 

- a single directory
       bool setDirName(std::string &dirName);
- a list of directories
       bool setVectDirName(std::vector<std::string> &vectDirName);
- a list of files       
       bool setVectFileName(std::vector<std::string> &vectFileName);
- a list of gdcm::File*      
       bool setVectGdcmFile(std::vector<GDCM_NAME_SPACE::File *> &vectGdcmFile);

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
   - if you want to get a vector of 2D vtkImageData, use SplitOnPosition  -i.e. one slice in each 'XCoherent fileset'-

 for 'true' 4D multi-orientation image sets (i.e. a stack of n axial + m sagital + p coronal images, at different instants ...)
   --> this is 5D, right?
   (almost) nothing done, yet :
   . use setSplitOnly()
   . Use a first time vtkGdcm4DSplitter with setSplitOnOrientation();
   . Get the VectGdcmFileLists (a std::vector of 'XCoherent fileset')
   . use vtkGdcm4DSplitter, with as many setVectGdcmFile(std::vector<GDCM_NAME_SPACE::File *> &vectGdcmFile) you need 
        one per element of std::vector<GDCM_NAME_SPACE::File *>
        think on 'spliting' and 'sorting' it, according to your needs. 
 
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

- Choose 'flip' option
----------------------
VTK reader 'flop upside down' the images to display them correctly ?!?
When a 'math guy' gets the image, the first thing he does is flipping it again.
Avoid loosing CPU time using :
      void SetFlipY(bool); // Wrong = no flip

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

#include "vtkGdcmReader.h"
#include "vtkGdcm4DSplitter.h"
#include <algorithm>
#include "gdcmSerieHelper.h" // for ImagePositionPatientOrdering()
#include <stdlib.h> // for atof

// Constructor / Destructor
/**
 * \brief   Constructor from a given vtkGdcm4DSplitter
 */
 vtkGdcm4DSplitter::vtkGdcm4DSplitter() :
                 SplitOnPosition(false), SplitOnOrientation(false), SplitOnTag(false),
                 SplitGroup(0), SplitElem(0), SplitConvertToFloat(false), SplitOnly(false),

                 SortOnPosition(false),  SortOnOrientation(false), /* SortOnTag(false),*/ SortOnFileName(false), SortOnUserFunction(false),
                 /*SortGroup(0),  SortElem(0), SortConvertToFloat(false),*/

                 Recursive(false),FlipY(true), TypeDir(0),
                 verbose(false) 
 {
 
 }

/**
 * \brief   Canonical destructor.
 */ 
 vtkGdcm4DSplitter::~vtkGdcm4DSplitter()
 {
    /// \TODO : delete everything that must be! 
 }

       // Locate Data to process
       // ======================
/**
 * \brief sets the directories exploration mode
 * @param recursive whether we want explore recursively the root Directory
 */       
void  vtkGdcm4DSplitter::setRecursive(bool recursive) 
{ 
   Recursive=recursive;
}
      
/**
 * \brief Sets the root Directory to get the images from
 * @param   dirName name of the directory to deal with
 */       
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

/**
 * \brief Sets a list of Directories to get the images from
 * @param   vectDirName vector of directory names to deal with
 */   
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

/**
 * \brief Sets a list of files names to read
 * @param   vectFileName vector of file names to deal with
 */ 
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

/**
 * \brief Sets an already filled std::vector of gdcm::File *
 * @param   vectGdcmFileName std::vector of gdcm::File *
 */
 
 bool vtkGdcm4DSplitter::setVectGdcmFile(GDCM_NAME_SPACE::FileList *vectGdcmFile)
 {
    if ( vectGdcmFile->size() == 0)
    {
          std::cout << "[ vectGdcmFile ] : empty list" << std::endl;
          return false;
    }
    TypeDir=4;
    VectGdcmFile = vectGdcmFile; 
 } 


       // Split criterion
       // ===============
       //

/**
 * \brief   asks for splitting  Filesets according to the Position
 */      
 void  vtkGdcm4DSplitter::setSplitOnPosition()
 {
    SplitOnPosition=true;
    SplitOnOrientation=false;
    SplitOnTag=false;
 }
 
/**
 * \brief   asks for splitting  Filesets according to the Orientation
 */
 void  vtkGdcm4DSplitter::setSplitOnOrientation()
 {
    SplitOnPosition=false;
    SplitOnOrientation=true; 
    SplitOnTag=false;
 }
 
/**
 * \brief   asks for splitting  Filesets according to the value of a given Tag
 * @param   group  group number of the target Element
 * @param   element element number of the target Element 
 */ 
 void  vtkGdcm4DSplitter::setSplitOnTag(unsigned short int splitGroup, unsigned short int splitElem)
 {
    SplitOnPosition=false;
    SplitOnOrientation=false;
    SplitOnTag=true;
    SplitGroup=splitGroup;
    SplitElem=splitElem;
 }
 
/**
 * \brief   asks for converting to 'float' the tag values used as a splitting criteria (lexicographic order may not be suitable)
 */   
 void  vtkGdcm4DSplitter::setSplitConvertToFloat(bool conv) {SplitConvertToFloat=conv;}
 
/**
 * \brief   asks for splitting Filesets according to what was asked for (no sorting, no reading data)
 */  
 void  vtkGdcm4DSplitter::setSplitOnly(bool s)
 {
    SplitOnly = s;
 }
 
       // Sort criterion
       // ==============
       //
 
/**
 * \brief   asks for IPP sorting each XCoherent gdcm::FILE set
 */  
 void  vtkGdcm4DSplitter::setSortOnPosition() 
 {
    SortOnPosition=true;
    SortOnOrientation=false;
    //SortOnTag=false;
    SortOnFileName=false;
    SortOnUserFunction=false;
    SortOnPosition=true;
 }
 
 // use setSortOnUserFunction, instead!
 // void setSortOnTag(unsigned short int sortGroup, unsigned short int sortElem)
 // {
 //    SortOnPosition=false;
 //    SortOnOrientation=false;
 //    SortOnTag=true;
 //    SortOnFileName=false;
 //    SortOnUserFunction=false;
 //    SortGroup=sortGroup;  SortElem=sortElem;
 // }


/**
 * \brief sets a user supplied function (comparison)
 * @param f comparison function
 */
 void  vtkGdcm4DSplitter::setSortOnUserFunction (FoncComp f)
 {
    UserCompareFunction=f;
    SortOnPosition=false;
    SortOnOrientation=false;
    //SortOnTag=false;
    SortOnFileName=false;
    SortOnUserFunction=true;
  }

 //  void setSortConvertToFloat(bool conv)
 //  {
 //     SortConvertToFloat=conv;
 //  }

/**
 * \brief asks for sorting each XCoherent gdcm::FILE set, according to the File names
 */
 void  vtkGdcm4DSplitter::setSortOnFileName()
 {
    SortOnPosition=false;
    SortOnOrientation=false;
    //SortOnTag=false;
    SortOnFileName=true;
    SortOnUserFunction=false;
 }

/**
 * \brief returns a std::vector of gdcm::FileList* (gdcm::FileList : actually, a std::vector of gdcm::File*)
 */ 
 std::vector<GDCM_NAME_SPACE::FileList *> *vtkGdcm4DSplitter::GetVectGdcmFileLists()
 {
    if (SplitOnly)
        return &VectGdcmFileLists;

    GDCM_NAME_SPACE::XCoherentFileSetmap::iterator it;
    for ( it = xcm.begin();
          it != xcm.end();
        ++it)
    {
       VectGdcmFileLists.push_back((*it).second); 
    } 
    return  &VectGdcmFileLists;         
 }

/**
 * \brief returns a std::vector of [2D/3D, depending on what was passed] vtkImageData*
 */
 std::vector<vtkImageData*> * vtkGdcm4DSplitter::GetImageDataVector() 
 { 
    if (SplitOnly)
       return NULL;
     return ImageDataVector;
 }

/**
 * \brief when user _knows_ only _one_ vtkImageData* is returned he may be interested in not getting a vector...
 */
 vtkImageData *vtkGdcm4DSplitter::GetImageData() 
 {
   if (SplitOnly)
      return NULL;
   return (*ImageDataVector)[0];
 }      

 
// bool vtkGdcm4DSplitter::CompareOnSortTagConvertToFloat(GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2)
// {
//   return atof(file1->GetEntryString(vtkGdcm4DSplitter::SortGroup,vtkGdcm4DSplitter::SortElem).c_str()) < atof(file2->GetEntryString(vtkGdcm4DSplitter::SortGroup,vtkGdcm4DSplitter::SortElem).c_str()); 
//   return atof(file1->GetEntryString(SortGroup,SortElem).c_str()) < atof(file2->GetEntryString(SortGroup,SortElem).c_str()); 
// } 

// bool vtkGdcm4DSplitter::CompareOnSortTag(GDCM_NAME_SPACE::File *file1, GDCM_NAME_SPACE::File *file2)
// {
//   return file1->GetEntryString(vtkGdcm4DSplitter::SortGroup,vtkGdcm4DSplitter::SortElem) < file2->GetEntryString(vtkGdcm4DSplitter::SortGroup,vtkGdcm4DSplitter::SortElem);  
// }
 
 
 bool vtkGdcm4DSplitter::Go()
 {
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

   if (!SplitOnPosition && !SplitOnOrientation && !SplitOnTag) 
   {
       ///\TODO (?) Throw an exception "Choose Splitting mode before!"
       std::cout << "Choose Splitting mode before!" << std::endl;
       return false;
   }

   GDCM_NAME_SPACE::FileList *l;
   
   GDCM_NAME_SPACE::SerieHelper *s;  
   s = GDCM_NAME_SPACE::SerieHelper::New();
   
//
// Load the gdcm::File* set, according to user's requierements
// ------------------------------------------------------------
//   
   l = getGdcmFileList();
std::cout << l->size() << " gdcm::File read" << std::endl;
//
// Split the gdcm::File* set, according to user's requierements
// ------------------------------------------------------------
//
   if (SplitOnOrientation) 
   {
      s->SetDropDuplicatePositions(false);
      xcm = s->SplitOnOrientation(l);
   }
   else if (SplitOnPosition)
   {
      s->SetDropDuplicatePositions(true);
      xcm = s->SplitOnPosition(l);

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
      if(verbose)
         std::cout << "Empty XCoherent File Set after 'split' ?!?" << std::endl;
      return false;
   }
   else
   {
      std::cout << xcm.size() << " XCoherent entries found" << std::endl;
   }

// put here, to avoid segfault when unaware user sets SplitOnly to true, and the asks for ImageDataVector
ImageDataVector = new std::vector<vtkImageData*>;

std::cout <<"SplitOnly " << SplitOnly << std::endl;
   if(SplitOnly)
      return true;   
//
//
// ------------------------------------------------------------
//
//   ImageDataVector = new std::vector<vtkImageData*>;

   /// \TODO move inside the loop, or be clever using vtk!
  // vtkGdcmReader *reader = vtkGdcmReader::New(); // move inside the loop, or be clever using vtk!
   
 // XCoherentFileSetmap map < critère de split, FileList (= std::vector de gdcm::File*) >

   for (GDCM_NAME_SPACE::XCoherentFileSetmap::iterator i = xcm.begin(); 
                                                  i != xcm.end();
                                                ++i)
   {
      vtkGdcmReader *reader = vtkGdcmReader::New(); /// \TODO FIXME : unable to delete!
      
      reader->SetFlipY(FlipY);
      // better user SetFileLowerLeft()
      /// \TODO : modify vtkGdcmReader ! 
      if (verbose)
               std::cout << " --- xCoherentName = [" << (*i).first << "]" << std::endl;

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
            /// \TODO (?) SortOnOrientation()
      
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

//      else if (SortOnTag)
//      {  
//         if (verbose) std::cout << "SortOnTag" << std::endl;   
//         printf ("--> %04x %04x\n", SortGroup,SortElem);
//         std::cout << "Sorry, troubles not solved yet; use SortOnUserFunction, right now!" << std::endl;
 
        /*        ==> WARNING : This one has troubles; do NOT use it, right now!
        // a pointer to fonction cannot be casted as a pointer to member function!
        // Use SortOnUserFunction, instead!

        //  if ( SortConvertToFloat )
        //    s->SetUserLessThanFunction( reinterpret_cast<bool (*)(gdcm13::File*, gdcm13::File*)> 
        //                                                         ( &vtkGdcm4DSplitter::CompareOnSortTagConvertToFloat));     
        //  else
        //     s->SetUserLessThanFunction( reinterpret_cast<bool (*)(gdcm13::File*, gdcm13::File*)>
                                                                    ( &vtkGdcm4DSplitter::CompareOnSortTag)); 
       
         // Anything like this, in GDCM2? 
        //  s->UserOrdering((*i).second);
        */

//         //if (verbose) std::cout << "Out of SortOnTag" << std::endl;
//         std::cout << "NO ordering performed  :-( " << std::endl;
//      }
      
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
         // std::cout << "reader->GetOutput() :" << std::endl;
         // reader->GetOutput()->PrintSelf(std::cout, vtkIndent(2));
       }
       
       ImageDataVector->push_back(reader->GetOutput() );
       
       std::vector<vtkImageData*>::iterator it; 
       //if (verbose) // JPR
   
       for(it=ImageDataVector->begin(); it!=ImageDataVector->end(); ++it) {
        // std::cout << "-in vtkGdcm4DSplitter --------------------------" << std::endl;
        // (*it)->PrintSelf(std::cout, vtkIndent(2));
       }
       //std::cout << std::endl;
   }

   //reader->Delete();  // \TODO : fix
   s->Delete(); 
  // f->Delete();
   delete l;
   
   return true;
 }


/**
 * \brief Load the gdcm::File* set, according to user's requierements
 * returns a std::vector of gdcm::File* (gdcm::FileList : actually, a std::vector of gdcm::File*)
 */
 

GDCM_NAME_SPACE::FileList *vtkGdcm4DSplitter::getGdcmFileList()
{

  GDCM_NAME_SPACE::File *f;
  GDCM_NAME_SPACE::DirListType fileNames;

 //
 // Fill fileNames with the user supplied file names (in any)
 // ------------------------------------------------
 //
   if (TypeDir == 0 )  // Nothing was set as input...
   {
      ///\TODO (?) Throw an exception "Set input Directory name(s) / file names  before!"
      std::cout << "Set input Directory name(s) / file names  before!" << std::endl;
      return false;
   }
   else if (TypeDir == 1) // A root directory name was set as input
   {
      GDCM_NAME_SPACE::DirList dirlist(DirName, Recursive); // NO recursive exploration
      fileNames = dirlist.GetFilenames(); // all the file names
   }
   
   else if (TypeDir == 2) // a std::vector of directory names was set as input
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
   else if (TypeDir == 3) // a list of files names was set as input
   {
      fileNames=VectFileName;
   }  

 //
 // Fill l with the gdcm::File* corresponding to the files
 // --------------------------------------
 //

   GDCM_NAME_SPACE::FileList *l = new GDCM_NAME_SPACE::FileList; // (set of gdcm::File*)
   
   if (TypeDir == 4) // an already existing std::vector of gdcm::File* was set as input
   {
      l = VectGdcmFile;
   }
   else
   { 
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
   }
   return l; 
}



 void vtkGdcm4DSplitter::reorgXCoherentFileSetmap(GDCM_NAME_SPACE::XCoherentFileSetmap &xcm)
 {
 /*
 the key of the 'XCoherentFileSetmap', is a std::string, used as if it was found in the Dicom header
 Normaly(?), it's suitable for almost everything ...
 ... but the 'Image Position Patient'.
 We need to order the 'XCoherentFileSetmap' (NOT the content of each XCoherentFileSet!) according to the IPP,
 using Jolinda Smith's algorithm.
 (we use a subset of the one defined in gdcm::SerieHelper)
 
*/

   ELEM e;   
   std::vector<ELEM> vectElem;

/* just to remember :
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
//
// NOTE : if you need to use Jolinda Smith's algorithm, get the one inside gdcm::SerieHelper
// this one is a light version.

//Tags always use the same coordinate system, where "x" is left
//to right, "y" is posterior to anterior, and "z" is foot to head (RAH).

   //iop is calculated based on the file file
   float cosines[6];
   double normal[3];
   double ipp[3];
   double dist;
   double min = 0, max = 0;
   bool first = true;
   
   //double ZSpacing; // useless here! // JPR
   bool DirectOrder = true; // remove it!
   
  // ZSpacing = -1.0;  // will be updated if process doesn't fail
    
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

    //std::multimap<double, ELEM>::iterator it5 = distmultimap.begin();
    //double d1 = (*it5).first;
    //it5++;
    //double d2 = (*it5).first;
    //ZSpacing = d1-d2;
    //if (ZSpacing < 0.0)
    //   ZSpacing = - ZSpacing;

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
