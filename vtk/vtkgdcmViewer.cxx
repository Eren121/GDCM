#include <vtkRenderWindowInteractor.h>
#include <vtkImageViewer2.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsWriter.h>

#include "vtkGdcmReader.h"

int main(int argc, char *argv[])
{

  vtkGdcmReader *reader = vtkGdcmReader::New();
  reader->SetFileName( argv[1] );
  //reader->DebugOn();
  reader->Update();
  
  //print debug info:
  reader->GetOutput()->Print( std::cout );
        
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();

  vtkImageViewer2 *viewer = vtkImageViewer2::New();
  viewer->SetInput ( reader->GetOutput() );
  viewer->SetupInteractor (iren);
  
//  float *range = reader->GetOutput()->GetScalarRange();
//  viewer->SetColorWindow (range[1] - range[0]);
//  viewer->SetColorLevel (0.5 * (range[1] + range[0]));

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
