#include <iostream>

#include <vtkRenderWindowInteractor.h>
#include <vtkImageViewer2.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>

#include "vtkGdcmReader.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

//----------------------------------------------------------------------------
// Callback for the interaction
class vtkgdcmObserver : public vtkCommand
{
  public:
  virtual char const *GetClassName() const { return "vtkgdcmObserver";}
  static vtkgdcmObserver *New() 
    { return new vtkgdcmObserver; }
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

  reader->DebugOn();
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

  iren->Initialize();
  iren->Start();

  //if you wish you can export dicom to a vtk file  
  vtkStructuredPointsWriter *writer = vtkStructuredPointsWriter::New();
  writer->SetInput( reader->GetOutput());
  writer->SetFileName( "foo.vtk" );
  writer->SetFileTypeToBinary();
  writer->Write();

  reader->Delete();
  iren->Delete();
  viewer->Delete();
  writer->Delete();
  
  return 0;
}
