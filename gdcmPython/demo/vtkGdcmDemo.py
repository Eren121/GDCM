## A small demo that displays with VTK a dicom image parsed with gdcm.
## Warning: the parsing of header of the dicom file is done with gdcm
##          but the process of in-memory loading of the image is performed
##          by vtkImageReader (classical vtk operator). Since vtkImageReader
##          has no special knowledge of Dicom file format, this demo
##          will only work for a restrained sub-set of Dicom files (basically
##          non compressed and with "HighBit + 1 != BitsStored").
##          When those conditions are not met try using vtkgdcmReader.py...
import sys
import vtk
from gdcmPython import gdcmHeader

class vtkHistogram:
   '''Some medical images might have a poor dynamic. This is because
      some additional visual tags-like the patient name- are sometimes
      written/drawn on top of the image : to avoid interference with
      the image itself, the level used for this additional text is
      usually way above the maximum level of the image. Hence we use
      some cumulative histograme die-hard technique to remove some
      arbitrary small portion of the dynamic.'''
   def __init__(self, imagereader):
      self.vtkReader  = imagereader
      self.__Histo = None
      self.__CumulHisto = None
      self.__LevelMin = self.vtkReader.GetOutput().GetScalarRange()[0]
      self.__LevelMax = self.vtkReader.GetOutput().GetScalarRange()[1]
      self.__NumberOfBins = 255   # See VTKImageAccumulate
      self.__Spacing = (self.__LevelMax - self.__LevelMin)/self.__NumberOfBins

   def ComputeHisto(self):
      self.__Histo = vtk.vtkImageAccumulate()
      self.__Histo.SetComponentExtent(0, self.__NumberOfBins, 0, 0, 0, 0)
      self.__Histo.SetInput(self.vtkReader.GetOutput())
      self.__Histo.SetComponentSpacing(self.__Spacing, 0.0, 0.0)
      self.__Histo.Update()

   def ComputeCumulativeHisto(self):
      ''' Compyte the Cumulative histogram of the image.'''
      if not self.__Histo:
         self.ComputeHisto()
      self.__CumulHisto = []
      histo = self.__Histo.GetOutput()
      self.__CumulHisto.append(int(histo.GetScalarComponentAsFloat(0,0,0,0)))
      for i in range(1, self.__NumberOfBins):
         value = int(histo.GetScalarComponentAsFloat(i,0,0,0))
         self.__CumulHisto.append( self.__CumulHisto[i-1] + value)

   def GetTruncateLevels(self, LostPercentage):
      ''' Compute the level below which (respectively above which)
          the LostPercentage percentage of the pixels shall be disregarded.
      '''
      if LostPercentage < 0.0 or LostPercentage >1.0:
         print "   vtkHistogram::GetTruncateLevels: defaulting to 5%"
         LostPercentage = 0.05
      if not self.__CumulHisto:
         self.ComputeCumulativeHisto()
      NumPixels     = self.__CumulHisto[self.__NumberOfBins - 1]
      NumLostPixels = NumPixels * LostPercentage
      AboveLevel = None
      BelowLevel = None
      # FIXME : not really clean loop...
      for i in range(0, self.__NumberOfBins-1):
         if not BelowLevel:
            if self.__CumulHisto[i] > NumLostPixels:
               BelowLevel = self.__LevelMin + self.__Spacing * i
         if not AboveLevel:
            if self.__CumulHisto[i] > NumPixels - NumLostPixels:
               AboveLevel = self.__LevelMin + self.__Spacing * i
      if not AboveLevel or not BelowLevel:
         print "   vtkHistogram::GetTruncateLevels: Mistakes were made..."
      return BelowLevel, AboveLevel

class vtkGdcmImage(gdcmHeader):
   ''' All informations required for VTK display
       * VTK pipeline
       * Lut (or associated Window/Level reductive control parameters)'''
   def __init__(self, filename):
      gdcmHeader.__init__(self, filename) 
      self.filename = filename
                               # LUT reductive parameters
      self.__LevelMin  = 0     # Minimum value of pixel intensity
      self.__LevelMax  = 0     # Maximun value of pixel intensity
      self.__Level     = 0     # Current Level value (treshold below
                               # which the image is represented as black)
      self.__Window    = 0     # Width of displayed pixels (linearly).
                               # Above Level + Window pixel are represented
                               # as white.
      self.__VTKplaneActor = None
      self.vtkSetup()

   def VTKreaderSetup(self):
      self.__VTKReader = vtk.vtkImageReader()
      self.__VTKReader.SetFileName(self.filename)
      type = self.GetPixelType()
      if type == "8U":
          self.__VTKReader.SetDataScalarTypeToUnsignedChar()
      elif type == "8S":
          self.__VTKReader.SetDataScalarTypeTodChar()
      elif type == "16U":
          self.__VTKReader.SetDataScalarTypeToUnsignedShort()
      elif type == "16S":
          self.__VTKReader.SetDataScalarTypeToShort()
          self.__VTKReader.SetDataByteOrderToLittleEndian()
          #self.__VTKReader.SetDataByteOrderToBigEndian()
      else:
          print "vtkGdcmImage::VTKreaderSetup: unkown encoding:", type
          sys.exit()
      self.__VTKReader.SetHeaderSize(self.GetPixelOffset())
      self.__VTKReader.SetDataExtent (0, self.GetXSize()-1,
                                      0, self.GetYSize()-1,
                                      1, 1)
      self.__VTKReader.UpdateWholeExtent()

   def vtkSetup(self, orgx = -0.5, orgy = -0.5,
                      ix = 0.5, iy = -0.5, jx = -0.5, jy = 0.5):
      # ImageReader ---> Texture        \
      #                                  | ==> PlaneActor
      # PlaneSource ---> PolyDataMapper /
      self.VTKreaderSetup()
      ### LookupTable
      self.__VTKtable = vtk.vtkLookupTable()
      self.__VTKtable.SetNumberOfColors(1000)
      self.__VTKtable.SetTableRange(0,1000)
      self.CallibrateWindowLevel()    # calls the SetTableRange
      self.__VTKtable.SetSaturationRange(0,0)
      self.__VTKtable.SetHueRange(0,1)
      self.__VTKtable.SetValueRange(0,1)
      self.__VTKtable.SetAlphaRange(1,1)
      self.__VTKtable.Build()
      ### Texture
      self.__VTKtexture = vtk.vtkTexture()
      self.__VTKtexture.SetInput(self.__VTKReader.GetOutput())
      self.__VTKtexture.InterpolateOn()
      self.__VTKtexture.SetLookupTable(self.__VTKtable)
      ### PlaneSource
      self.__VTKplane = vtk.vtkPlaneSource()
      self.__VTKplane.SetOrigin( orgx, orgy, 0.0)
      self.__VTKplane.SetPoint1( ix,   iy,   0.0)
      self.__VTKplane.SetPoint2( jx,   jy,   0.0)
      ### PolyDataMapper
      self.__VTKplaneMapper = vtk.vtkPolyDataMapper()
      self.__VTKplaneMapper.SetInput(self.__VTKplane.GetOutput())
      ### Actor
      self.__VTKplaneActor = vtk.vtkActor()
      self.__VTKplaneActor.SetTexture(self.__VTKtexture)
      self.__VTKplaneActor.SetMapper(self.__VTKplaneMapper)
      self.__VTKplaneActor.PickableOn()

   def CallibrateWindowLevel(self):
      vtkreader = self.__VTKReader
      self.__LevelMin  = vtkreader.GetOutput().GetScalarRange()[0]
      self.__LevelMax  = vtkreader.GetOutput().GetScalarRange()[1]
      histo = vtkHistogram(vtkreader)
      self.__Level, above = histo.GetTruncateLevels(0.05)
      self.__Window = above - self.__Level
      self.__VTKtable.SetTableRange(self.__Level,
                                    self.__Level + self.__Window)

   def GetVTKActor(self):
      return self.__VTKplaneActor

######################################################################
import os
from gdcmPython import GDCM_DATA_PATH

### Get filename from command line or default it
try:
   FileName = sys.argv[1]
except IndexError:
   FileName = os.path.join(GDCM_DATA_PATH, "test.acr")

if not os.path.isfile(FileName):
   print "Cannot open file ", FileName
   sys.exit()

### The default vtkImageReader is unable to read some type of images:
check = gdcmHeader(FileName)
if not check.IsReadable():
   print "The ", FileName, " file is not "
   print "   readable with gdcm. Sorry."
   sys.exit()
check = check.GetPubElVal()
try:
   HighBit = check["High Bit"]
   if len(HighBit) == 0 or HighBit == "gdcm::Unfound":
      raise KeyError
except KeyError:
   print "Gdcm couldn't find the Bits Allocated Dicom tag in file ", FileName
   sys.exit()
try:
   BitsStored = check["Bits Stored"]
   if len(BitsStored) == 0 or BitsStored == "gdcm::Unfound":
      raise KeyError
except KeyError:
   print "Gdcm couldn't find the Bits Stored Dicom tag in file ", FileName
   sys.exit()
if int(HighBit) + 1 != int(BitsStored):
   print "vtkImageReader cannot read the file ", FileName
   print "  because the High Bit is offseted from the BitsStored."
   print "  You should consider using vtkGdcmReader as opposed to the"
   print "  vtkImageReader python class present in this demo. Please"
   print "  see gdcmPython/demo/vtkGdcmReader.py for a demo."
   sys.exit()

### Display in a vtk RenderWindow
image = vtkGdcmImage(FileName)
ren = vtk.vtkRenderer()
renwin = vtk.vtkRenderWindow()
renwin.AddRenderer(ren)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renwin)
ren.AddActor(image.GetVTKActor())
ren.SetBackground(0,0,0.5)
renwin.Render()
iren.Start()
