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
