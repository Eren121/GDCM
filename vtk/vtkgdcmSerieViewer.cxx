/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkgdcmSerieViewer.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/30 18:37:48 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
// This example illustrates how the vtkGdcmReader vtk class can be
// used in order to:
//  * produce a simple (vtk based) Dicom image STACK VIEWER.
//  * dump the stack considered as a volume in a vtkStructuredPoints
//    vtk file: the vtk gdcm wrappers can be seen as a simple way to convert
//    a stack of Dicom images into a native vtk volume.
//
// Usage:
//  * the Directory name that contains the Dicom images constituting the stack 
//    should be given as command line argument,
//  * you can navigate through the stack by hitting any character key,
//  * the produced vtk file is named "foo.vtk" (in the invocation directory).
// 
//----------------------------------------------------------------------------
#include <vtkRenderWindowInteractor.h>
#include <vtkImageViewer.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>

#include "vtkGdcmReader.h"
#include "gdcmDocument.h"  // for NO_SHADOWSEQ
#include "gdcmSerieHelper.h"
#include "gdcmDebug.h"
#include "gdcmArgMgr.h" // for Argument Manager functions
#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

void userSuppliedMirrorFunction (uint8_t *im, gdcm::File *f);
void userSuppliedTopDownFunction(uint8_t *im, gdcm::File *f);

//----------------------------------------------------------------------------
// Callback for the interaction
class vtkgdcmObserver : public vtkCommand
{
public:
   virtual char const *GetClassName() const 
   { 
      return "vtkgdcmObserver";
   }

   static vtkgdcmObserver *New() 
   { 
      return new vtkgdcmObserver; 
   }

   vtkgdcmObserver()
   {
      this->ImageViewer = NULL;
   }

   virtual void Execute(vtkObject *, unsigned long event, void* )
   {
      if ( this->ImageViewer )
      {
         if ( event == vtkCommand::CharEvent )
         {
            int max = ImageViewer->GetWholeZMax();
            int slice = (ImageViewer->GetZSlice() + 1 ) % ++max;
            ImageViewer->SetZSlice( slice );
            ImageViewer->GetRenderer()->ResetCameraClippingRange();
            ImageViewer->Render();
         }
      }
   }
   vtkImageViewer *ImageViewer;
};

int main(int argc, char *argv[])
{
   START_USAGE(usage)
   " \n vtkgdcmSerieViewer : \n",
   " Display a Serie within a Directory                                       ",
   " You can navigate through the stack by hitting any character key.         ",
   " usage: vtkgdcmSerieViewer filein=fileName [noshadowseq][noshadow][noseq] ",
   "                           [reverse] [{[mirror]|[topdown]|[rotate]}]      ",
   "                           [check][debug]                                 ",
   "      noshadowseq: user doesn't want to load Private Sequences            ",
   "      noshadow   : user doesn't want to load Private groups (odd number)  ",
   "      noseq      : user doesn't want to load Sequences                    ",
   "      mirror     : user wants to 'mirror' the images | just some simple   ",
   "      topdown    : user wants to 'topdown' the images| examples of user   ",
   "      rotate     : user wants NOT YET MADE           | supplied functions ",
   "      debug      : user wants to run the program in 'debug mode'          ",
   FINISH_USAGE


   // Initialize Arguments Manager   
   gdcm::ArgMgr *am= new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage") )
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }

   char *dirName = am->ArgMgrWantString("dirname",usage);

   int loadMode = 0x00000000;
   if ( am->ArgMgrDefined("noshadowseq") )
      loadMode |= NO_SHADOWSEQ;
   else 
   {
      if ( am->ArgMgrDefined("noshadow") )
         loadMode |= NO_SHADOW;
      if ( am->ArgMgrDefined("noseq") )
         loadMode |= NO_SEQ;
   }

   bool reverse = am->ArgMgrDefined("reverse");

   bool mirror  = am->ArgMgrDefined("mirror");
   bool topdown = am->ArgMgrDefined("topdown");
   bool rotate  = am->ArgMgrDefined("rotate");

  bool check   = am->ArgMgrDefined("check");

   if ( (int)mirror + (int)topdown + (int)rotate > 1)
   {
      std::cout << "mirror *OR* topDown *OR* rotate !"
                << std::endl;
      delete am;
      return 0;
   }

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();

   /* if unused Param we give up */
   if ( am->ArgMgrPrintUnusedLabels() )
   {
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   } 

   delete am;  // we don't need Argument Manager any longer

   // ----------------------- End Arguments Manager ----------------------
  
   // ------------ to check Coherent File List as a parameter

   gdcm::SerieHelper *sh = new gdcm::SerieHelper();
   sh->SetLoadMode(loadMode);
   if (reverse)
      sh->SetSortOrderToReverse();
   sh->SetDirectory( dirName, true);
    
   // Just to see

   int nbFiles;
   // For all the Coherent Files lists of the gdcm::Serie
   gdcm::FileList *l = sh->GetFirstCoherentFileList();
   if (l == 0 )
   {
      std::cout << "Oops! No CoherentFileList found ?!?" << std::endl;
      return 0;
   }
   while (l)
   { 
      nbFiles = l->size() ;
      if ( l->size() > 1 )
      {
         std::cout << "Sort list : " << nbFiles << " long" << std::endl;
         sh->OrderFileList(l);  // sort the list
         break;  // The first one is OK. user will have to check
      }
      else
      {
         std::cout << "Oops! Empty CoherentFileList found ?!?" << std::endl;
      }
      l = sh->GetNextCoherentFileList();
   }

   if (check)
   {
      if ( !sh->IsCoherent(l) ) // just be sure (?)
      {
         std::cout << "Files are not coherent. Stop everything " << std::endl;
         delete sh;
         return 0;
      }
   }

   vtkGdcmReader *reader = vtkGdcmReader::New();
   reader->AllowLookupTableOff();

   if (mirror)
      reader->SetUserFunction (userSuppliedMirrorFunction);
   else if (topdown)
      reader->SetUserFunction (userSuppliedTopDownFunction);

   // Only the first FileList is dealt with (just an example)
   // (The files will not be parsed twice by the reader)

   //---------------------------------------------------------
   reader->SetCoherentFileList(l);
   //---------------------------------------------------------

   // because we passed a Coherent File List from a SerieHelper,
   // setting LoadMode is useless in this case
   //  reader->SetLoadMode(NO_SHADOWSEQ);  
   reader->Update();

   //print debug info:
   reader->GetOutput()->Print( cout );

   vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();

   vtkImageViewer *viewer = vtkImageViewer::New();

   if( reader->GetLookupTable() )
   {
      //convert to color:
      vtkImageMapToColors *map = vtkImageMapToColors::New ();
      map->SetInput (reader->GetOutput());
      map->SetLookupTable (reader->GetLookupTable());
      map->SetOutputFormatToRGB();
      viewer->SetInput ( map->GetOutput() );
      map->Delete();
   }
   else
   {
      vtkFloatingPointType *range = reader->GetOutput()->GetScalarRange();
      viewer->SetColorLevel (0.5 * (range[1] + range[0]));
      viewer->SetColorWindow (range[1] - range[0]);

      viewer->SetInput ( reader->GetOutput() );
   }
   viewer->SetupInteractor (iren);
  
   //vtkFloatingPointType *range = reader->GetOutput()->GetScalarRange();
   //viewer->SetColorWindow (range[1] - range[0]);
   //viewer->SetColorLevel (0.5 * (range[1] + range[0]));

   // Here is where we setup the observer, 
   vtkgdcmObserver *obs = vtkgdcmObserver::New();
   obs->ImageViewer = viewer;
   iren->AddObserver(vtkCommand::CharEvent,obs);
   obs->Delete();

   //viewer->Render();
   iren->Initialize();
   iren->Start();

   //if you wish you can export dicom to a vtk file  
   vtkStructuredPointsWriter *writer = vtkStructuredPointsWriter::New();
   writer->SetInput( reader->GetOutput());
   writer->SetFileName( "foo.vtk" );
   writer->SetFileTypeToBinary();
   //writer->Write();

   reader->Delete();
   iren->Delete();
   viewer->Delete();
   writer->Delete();

   return 0;
}


// --------------------------------------------------------
// This is just a *very* simple example of user supplied function
//      to mirror (why not ?) the image
// It's *not* part of gdcm.
// --------------------------------------------------------

#define UF(ty)                          \
   int i, j;                            \
   ty *imj;                             \
   ty tamp;                             \
   for (j=0;j<ny;j++)                   \
   {                                    \
      imj = (ty *)im +j*nx;             \
      for (i=0;i<nx/2;i++)              \
      {                                 \
        tamp       =imj[i];             \
        imj[i]     =imj[nx-1-i];        \
        imj[nx-1-i]=tamp;               \
      }                                 \
   }                                    \
   if (nx%2 != 0)                       \
   {                                    \
      for (j=0;j<ny;j++)                \
      {                                 \
        imj = (ty *)im  +j*nx;          \
        tamp       =imj[i];             \
        imj[i]     =imj[nx/2+1];        \
        imj[nx/2+1]=tamp;               \
      }                                 \
   }

void userSuppliedMirrorFunction(uint8_t *im, gdcm::File *f)
{
   if (f->GetZSize() != 1)
   {
      std::cout << "mirror : Multiframe images not yet dealt with" << std::endl;
      return;
   }

   if (f->GetSamplesPerPixel() != 1 || f->GetBitsAllocated() == 24)
   {
      std::cout << "mirror : RGB / YBR not yet dealt with" << std::endl;
      return;
   }
   int nx = f->GetXSize();
   int ny = f->GetYSize();

   std::string pixelType = f->GetPixelType();
   if ( pixelType ==  "8U" || pixelType == "8S" )
   {
      UF(uint8_t)
      return;
   }
   if ( pixelType == "16U" || pixelType == "16S")
   {
      UF(uint16_t)
      return;
   }
   std::cout << "mirror : Pixel Size (!=8, !=16) not yet dealt with" 
             << std::endl;
   return;
}


// --------------------------------------------------------
// This is just a *very* simple example of user supplied function
//      to topdown (why not ?) the image
// It's *not* part of gdcm.
// --------------------------------------------------------

#define UF2(ty)                         \
   int i, j;                            \
   ty *imj, *imJ;                       \
   ty tamp;                             \
   for (j=0;j<ny/2;j++)                 \
   {                                    \
      imj = (ty *)im +j*nx;             \
      imJ = (ty *)im +(ny-1-j)*nx;      \
      for (i=0;i<nx;i++)                \
      {                                 \
        tamp       =imj[i];             \
        imj[i]     =imJ[i];             \
        imJ[i]     =tamp;               \
      }                                 \
   }

void userSuppliedTopDownFunction(uint8_t *im, gdcm::File *f)
{
   if (f->GetZSize() != 1)
   {
      std::cout << "mirror : Multiframe images not yet dealt with" << std::endl;
      return;
   }

   if (f->GetSamplesPerPixel() != 1 || f->GetBitsAllocated() == 24)
   {
      std::cout << "mirror : RGB / YBR not yet dealt with" << std::endl;
      return;
   }
   int nx = f->GetXSize();
   int ny = f->GetYSize();

   std::string pixelType = f->GetPixelType();
   if ( pixelType ==  "8U" || pixelType == "8S" )
   {
      UF2(uint8_t)
      return;
   }
   if ( pixelType == "16U" || pixelType == "16S")
   {
      UF2(uint16_t)
      return;
   }
   std::cout << "topdown : Pixel Size (!=8, !=16) not yet dealt with" 
             << std::endl;
   return;
}




