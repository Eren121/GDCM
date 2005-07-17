/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkgdcmSerieViewer.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/17 04:36:14 $
  Version:   $Revision: 1.1 $
                                                                                
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
//  * the filenames of the Dicom images constituting the stack should be
//    given as command line arguments,
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
#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

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
   if( argc < 2 )
      return 0;

  if( argc > 2 )
    gdcm::Debug::DebugOn();

  
   vtkGdcmReader *reader = vtkGdcmReader::New();
   reader->AllowLookupTableOff();

   // ------------ to check Coherent File List as a parameter

   gdcm::SerieHelper *sh = new gdcm::SerieHelper();
   sh->SetLoadMode(NO_SHADOWSEQ);
   sh->SetDirectory( argv[1], true);
    
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
      if ( l->size() > 0 ) // Why not ? Just an example, for testing
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

   reader->SetCoherentFileList(l);

/*
   if( argc == 2 )
      reader->SetFileName( argv[1] );
   else
      for(int i=1; i< argc; i++)
         reader->AddFileName( argv[i] );
*/

// TODO : allow user to choose Load Mode

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
