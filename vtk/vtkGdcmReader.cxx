// $Header: /cvs/public/gdcm/vtk/vtkGdcmReader.cxx,v 1.3 2003/05/21 08:21:17 frog Exp $
#include "vtkGdcmReader.h"
#include "vtkByteSwap.h"
#include <stdio.h>
#include "vtkObjectFactory.h"
#include "vtkImageFlip.h"
#include "gdcm.h"

vtkGdcmReader::vtkGdcmReader()
{
  // Constructor
}

//----------------------------------------------------------------------------
vtkGdcmReader::~vtkGdcmReader()
{ 
  // FIXME free memory
}

// Configure the output e.g. WholeExtent, spacing, origin, scalar type...
void vtkGdcmReader::ExecuteInformation()
{
  //FIXME free any old memory
      
  // if the user has not set the extent, but has set the VOI
  // set the zaxis extent to the VOI z axis
  if (this->DataExtent[4]==0 && this->DataExtent[5] == 0 &&
     (this->DataVOI[4] || this->DataVOI[5]))
    {
    this->DataExtent[4] = this->DataVOI[4];
    this->DataExtent[5] = this->DataVOI[5];
    }

  this->ComputeInternalFileName(this->DataExtent[4]);
  
  // Check for file existence.
  FILE *fp;
  fp = fopen(this->InternalFileName,"rb");
  if (!fp)
    {
    vtkErrorMacro("Unable to open file " << this->InternalFileName);
    return;
    }
  fclose(fp);

  // Check for Gdcm parsability
  gdcmHeader GdcmHeader(this->InternalFileName);
  if (!GdcmHeader.IsReadable())
    {
    vtkErrorMacro("Gdcm cannot parse file " << this->InternalFileName);
    return;
    }

  int NX = GdcmHeader.GetXSize();
  int NY = GdcmHeader.GetYSize();
  int NZ = GdcmHeader.GetZSize();
  vtkDebugMacro("Image dimension as read from Gdcm:" <<
                NX << " " << NY << " " << NZ);

  if(NZ>1) this->SetFileDimensionality(3);

  // When the user has set the VOI, check it's coherence with the file content.
  if (this->DataVOI[0] || this->DataVOI[1] || 
      this->DataVOI[2] || this->DataVOI[3] ||
      this->DataVOI[4] || this->DataVOI[5])
    { 
    if ((this->DataVOI[0] < 0) ||
        (this->DataVOI[1] >= NX) ||
        (this->DataVOI[2] < 0) ||
        (this->DataVOI[3] >= NY) ||
        (this->DataVOI[4] < 0) ||
        (this->DataVOI[5] >= NZ))
      {
      vtkWarningMacro("The requested VOI is larger than the file's ("
                      << this->InternalFileName << ") extent ");
      this->DataVOI[0] = 0;
      this->DataVOI[1] = NX - 1;
      this->DataVOI[2] = 0;
      this->DataVOI[3] = NY - 1;
      this->DataVOI[4] = 0;
      this->DataVOI[5] = NZ - 1;
      }
    }

  // Positionate the Extent.
  this->DataExtent[0] = 0;
  this->DataExtent[1] = NX - 1;
  this->DataExtent[2] = 0;
  this->DataExtent[3] = NY - 1;
  if(this->GetFileDimensionality()==3)
    {
      this->DataExtent[4] = 0;
      this->DataExtent[5] = NZ - 1;
    }
  
  // We don't need to positionate the Endian related stuff (by using
  // this->SetDataByteOrderToBigEndian() or SetDataByteOrderToLittleEndian()
  // since the // reading of the file is done by gdcm

  // But we need to set up the data type for downstream filters:
  string type = GdcmHeader.GetPixelType();
  if      ( type == "8U" )
    {
    vtkDebugMacro("8 bits unsigned image");
    this->SetDataScalarTypeToUnsignedChar(); 
    }
  else if ( type == "8S" )
    {
    vtkErrorMacro("Cannot handle 8 bit signed files");
    return;
    }
  else if ( type == "16U" )
    {
    vtkDebugMacro("16 bits unsigned image");
    this->SetDataScalarTypeToUnsignedShort();
    }
  else if ( type == "16S" )
    {
    vtkDebugMacro("16 bits signed image");
    this->SetDataScalarTypeToShort();
    //vtkErrorMacro("Cannot handle 16 bit signed files");
    }
  else if ( type == "32U" )
    {
    vtkDebugMacro("32 bits unsigned image");
    vtkDebugMacro("WARNING: forced to signed int !");
    this->SetDataScalarTypeToInt();
    }
  else if ( type == "32S" )
    {
    vtkDebugMacro("32 bits signed image");
    this->SetDataScalarTypeToInt();
    }
  else
    {
    vtkErrorMacro("Bad File Type " << this->InternalFileName
                                   << "Type " << type);
    return;
    }

  vtkImageReader::ExecuteInformation();
}

// Update -> UpdateData -> Execute -> ExecuteData (see vtkSource.cxx for
// last step.
// This function (redefinition of vtkImageReader::ExecuteData, see 
// VTK/IO/vtkImageReader.cxx) reads a data from a file. The datas
// extent/axes are assumed to be the
// same as the file extent/order.
void vtkGdcmReader::ExecuteData(vtkDataObject *output)
{
  if (!this->FileName)
    {
    vtkErrorMacro("A valid FileName must be specified.");
    return;
    }

  vtkImageData *data = this->AllocateOutputData(output);
  data->SetExtent(this->DataExtent);
  data->GetPointData()->GetScalars()->SetName("ImageFile");

  // First check the coherence between the DataExtent and the
  // size of the pixel data as annouced by gdcm (looks a bit paranoid).
  gdcmFile GdcmFile(this->InternalFileName);
  int NumColumns = this->DataExtent[1] - this->DataExtent[0] + 1;
  int NumLines   = this->DataExtent[3] - this->DataExtent[2] + 1;
  int NumPlanes  = this->DataExtent[5] - this->DataExtent[4] + 1;
  int size = NumColumns * NumLines * NumPlanes * GdcmFile.GetPixelSize();
  if ( size != GdcmFile.GetImageDataSize() )
    {
    vtkDebugMacro("Inconsistency with GetImageDataSize");
    vtkDebugMacro("Number of scalar components"
                  << this->NumberOfScalarComponents);
    }
  // Allocate pixel data space itself.
  unsigned char *mem = new unsigned char [size];

  // If the data structure of vtk for image/volume representation
  // were straigthforwards the following would suffice:
  //    GdcmFile.GetImageDataIntoVector((void*)mem, size);
  // But vtk chose to invert the lines of an image, that is the last
  // line comes first (for some axis related reasons?). Hence we need
  // to load the image line by line, starting from the end:
  int LineSize = NumColumns * GdcmFile.GetPixelSize();
  unsigned char * Source      = (unsigned char*)GdcmFile.GetImageData();
  unsigned char * Destination = mem + size - LineSize;
  for (int i = 0; i < NumLines; i++)
    {
    memcpy((void*)Destination, (void*)Source, LineSize);
    Source      += LineSize;
    Destination -= LineSize;
    }
  // The "size" of the vtkScalars data is expressed in number of points,
  // and is not the memory size representing those points:
  size = size / GdcmFile.GetPixelSize();
  data->GetPointData()->GetScalars()->SetVoidArray(mem, size, 0);
  this->Modified();

}

void vtkGdcmReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageReader::PrintSelf(os,indent);
  //CLEANME os << indent << "TypeSize: " << this->TypeSize << "\n";
}
