## This demo illustrates the usage of the python-wrappers of vtkGdcmReader
## native C++ vtk class. vtkGdcmReader.cxx is a C++ level wrapper of
## gdcm which offers a vtk class vtkGdcmReader (inheriting from vtkImageReader,
## see gdcm/vtk/vtkGdcmReader.cxx). vtkgdcmPython wraps this class for
## python (by using vtk wrappers).
from vtk import *
from gdcmPython import *
from gdcmPython.vtkgdcmPython import *
import sys

### Get filename from command line or default it
try:
   FileName = sys.argv[1]
except IndexError:
   FileName = os.path.join(GDCM_DATA_PATH, "test.acr")

### Build the header element list
test = gdcmHeader(FileName)
if not test.IsReadable():
   print "The ", FileName, " file is not readable with gdcm. Sorry."
   sys.exit()
del test
toDisplay = vtkGdcmReader()
toDisplay.SetFileName(FileName)
### Uncomment to test for volumes:
###toDisplay.DebugOn()
###toDisplay.AddFileName(FileName)
toDisplay.UpdateWholeExtent()

VTKtable = vtkLookupTable()
VTKtable.SetNumberOfColors(1000)
VTKtable.SetTableRange(0,1000)
VTKtable.SetSaturationRange(0,0)
VTKtable.SetHueRange(0,1)
VTKtable.SetValueRange(0,1)
VTKtable.SetAlphaRange(1,1)
VTKtable.Build()

VTKtexture = vtkTexture()
VTKtexture.SetInput(toDisplay.GetOutput())
VTKtexture.InterpolateOn()
VTKtexture.SetLookupTable(VTKtable)

VTKplane = vtkPlaneSource()
VTKplane.SetOrigin( -0.5, -0.5, 0.0)
VTKplane.SetPoint1(  0.5, -0.5, 0.0)
VTKplane.SetPoint2( -0.5,  0.5, 0.0)

VTKplaneMapper = vtkPolyDataMapper()
VTKplaneMapper.SetInput(VTKplane.GetOutput())

VTKplaneActor = vtkActor()
VTKplaneActor.SetTexture(VTKtexture)
VTKplaneActor.SetMapper(VTKplaneMapper)
VTKplaneActor.PickableOn()

ren = vtkRenderer()
renwin = vtkRenderWindow()
renwin.AddRenderer(ren)
iren = vtkRenderWindowInteractor()
iren.SetRenderWindow(renwin)
ren.AddActor(VTKplaneActor)
ren.SetBackground(0,0,0.5)
renwin.Render()
iren.Start()



