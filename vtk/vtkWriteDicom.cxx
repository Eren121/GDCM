// This example illustrates how the vtkGdcmWriter vtk class can be
// used in order to:
//
// Usage:
// 
//----------------------------------------------------------------------------
#include <iostream>

#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>
#include <vtkImageData.h>

#include "vtkGdcmReader.h"
#include "vtkGdcmWriter.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

//----------------------------------------------------------------------------
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

   reader->Update();

   vtkImageData *output;
   if( reader->GetLookupTable() )
   {
      //convert to color:
      vtkImageMapToColors *map = vtkImageMapToColors::New ();
      map->SetInput (reader->GetOutput());
      map->SetLookupTable (reader->GetLookupTable());
      map->SetOutputFormatToRGB();
      output = map->GetOutput();
      map->Delete();
   }
   else
   {
      output = reader->GetOutput();
   }
  
   //print debug info:
   output->Print( cout );

   //////////////////////////////////////////////////////////
   // WRITE...
   //if you wish you can export dicom to a vtk file 
   // this file will have the add of .tmp.dcm extention
   std::string fileName = argv[1];
   fileName += ".tmp.dcm";

   vtkGdcmWriter *writer = vtkGdcmWriter::New();
   writer->SetFileName(fileName.c_str());
   writer->SetInput(output);
   writer->Write();
   //////////////////////////////////////////////////////////

   // Clean up
   writer->Delete();
   reader->Delete();

   return 0;
}
