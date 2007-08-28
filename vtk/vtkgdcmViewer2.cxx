/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkgdcmViewer2.cxx,v $
  Language:  C++
  Date:      $Date: 2007/08/28 14:02:13 $
  Version:   $Revision: 1.10 $
                                                                                
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
#include <vtkImageViewer2.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>

#include "vtkGdcmReader.h"
#include "gdcmDocument.h"  // for NO_SHADOWSEQ
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
#if (VTK_MAJOR_VERSION >= 5)
            int max = ImageViewer->GetSliceMax();
            int slice = (ImageViewer->GetSlice() + 1 ) % ++max;
            ImageViewer->SetSlice( slice );
#else
            int max = ImageViewer->GetWholeZMax();
            int slice = (ImageViewer->GetZSlice() + 1 ) % ++max;
            ImageViewer->SetZSlice( slice );
#endif
#if !( (VTK_MAJOR_VERSION >= 5) || ( VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION >= 5 ) )
         // This used to be a bug in version VTK 4.4 and earlier
            ImageViewer->GetRenderer()->ResetCameraClippingRange();
#endif
            ImageViewer->Render();
         }
      }
   }
   vtkImageViewer2 *ImageViewer;
};


int main(int argc, char *argv[])
{
   if( argc < 2 )
      return 0;
  
   vtkGdcmReader *reader = vtkGdcmReader::New();
   reader->AllowLookupTableOff();

   if( argc == 2 )
      reader->SetFileName( argv[1] );
   else
      for(int i=1; i< argc; i++)
         reader->AddFileName( argv[i] );

// TODO : allow user to choose Load Mode
   reader->SetLoadMode(GDCM_NAME_SPACE::LD_NOSHADOWSEQ);  
   reader->Update();

   //print debug info:
   reader->GetOutput()->Print( cout );

   vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();

   vtkImageViewer2 *viewer = vtkImageViewer2::New();

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
   
   // For a single medical image, it would be more efficient to use
   // 0028|1050 [DS] [Window Center]
   // 0028|1051 [DS] [Window Width]
   // but vtkgdcmReader doesn't know about them :-(

   if( reader->GetOutput()->GetNumberOfScalarComponents() == 1 )
     {
     vtkFloatingPointType *range = reader->GetOutput()->GetScalarRange();
     viewer->SetColorLevel (0.5 * (range[1] + range[0]));
     viewer->SetColorWindow (range[1] - range[0]);
     }
      viewer->SetInput ( reader->GetOutput() );
   }
   viewer->SetupInteractor (iren);

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
