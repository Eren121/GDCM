// $Header: /cvs/public/gdcm/vtk/Attic/testvtkGdcmReader.cxx,v 1.7 2003/12/22 12:46:19 regrain Exp $

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkImageMapper.h>
#include <vtkImageData.h>
#include <vtkImageViewer.h>
#include <vtkMatrix4x4.h>
#include <vtkLookupTable.h>
#include <vtkMatrixToLinearTransform.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkTextureMapToPlane.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkImageCast.h>
#include <vtkPNGWriter.h>
#include <vtkTexture.h>

#include "vtkGdcmReader.h"

  
int main( int argc, char *argv[] )
{
   int *taille;
   int x,y;

   // Lecture de l'image
   vtkGdcmReader *reader = vtkGdcmReader::New();
//   reader->DebugOn();
   // Alloc Used High
   // 8 8 7 U : OK
   // reader->SetFileName("../gdcmData/CT-MONO2-8-abdo.dcm");
   // 16 12 11 U : OK but saturated
   // reader->SetFileName("../gdcmData/CT-MONO2-12-lomb-an2.acr2");
   // 16 12 11 U OK
   //OKreader->SetFileName("../gdcmData/MR-MONO2-12-an2.acr2");
   // 16 10 9 U OK
   //reader->SetFileName("../gdcmData/CR-MONO1-10-chest.dcm");
   //reader->Update();
   // 16 16 15 S: OK saturation ?
   // reader->SetFileName("../gdcmData/CT-MONO2-16-ort.dcm");
   // 16 16 15 S:

	if (argc > 1)
     reader->SetFileName( argv[1] );
	else
     reader->SetFileName("../gdcmData/CT-MONO2-16-ankle.dcm");

   reader->UpdateWholeExtent();
   vtkImageData *ima = reader->GetOutput();
   taille=ima->GetDimensions();
   x = taille[0];  y = taille[1];
   cout << "Dimensions of the picture as read with gdcm: "
        << x << " x " << y << endl;

   vtkLookupTable *VTKtable = vtkLookupTable::New();
   VTKtable->SetNumberOfColors(1000);
   VTKtable->SetTableRange(0,1000);
   VTKtable->SetSaturationRange(0,0);
   VTKtable->SetHueRange(0,1);
   VTKtable->SetValueRange(0,1);
   VTKtable->SetAlphaRange(1,1);
   VTKtable->Build();
   // Texture
   vtkTexture * VTKtexture = vtkTexture::New();
   VTKtexture->SetInput(ima);
   VTKtexture->InterpolateOn();
   VTKtexture->SetLookupTable(VTKtable);
   // PlaneSource
   vtkPlaneSource *VTKplane = vtkPlaneSource::New();
   VTKplane->SetOrigin( -0.5, -0.5, 0.0);
   VTKplane->SetPoint1(  0.5, -0.5, 0.0);
   VTKplane->SetPoint2( -0.5,  0.5, 0.0);
   // PolyDataMapper
   vtkPolyDataMapper *VTKplaneMapper = vtkPolyDataMapper::New();
   VTKplaneMapper->SetInput(VTKplane->GetOutput());
   // Actor
   vtkActor* VTKplaneActor = vtkActor::New();
   VTKplaneActor->SetTexture(VTKtexture);
   VTKplaneActor->SetMapper(VTKplaneMapper);
   VTKplaneActor->PickableOn();
   //
   vtkRenderer        *ren = vtkRenderer::New();
   vtkRenderWindow *renwin = vtkRenderWindow::New();
   renwin->AddRenderer(ren);
   vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
   iren->SetRenderWindow(renwin);
   ren->AddActor(VTKplaneActor);
   ren->SetBackground(0,0,0.5);
   renwin->Render();
   iren->Start();

  reader->Delete();
  VTKtable->Delete();
  VTKtexture->Delete();
  VTKplane->Delete();
  VTKplaneMapper->Delete();
  VTKplaneActor->Delete();
  ren->Delete();
  renwin->Delete();
  iren->Delete();

   return(0);
}

