// $Header: /cvs/public/gdcm/vtk/vtkGdcmReader.h,v 1.14 2003/11/18 10:19:41 malaterre Exp $

#ifndef __vtkGdcmReader_h
#define __vtkGdcmReader_h

#include <vtkImageReader.h>
#include <list>
#include <string>

class vtkLookupTable;

class VTK_EXPORT vtkGdcmReader : public vtkImageReader
{
public:
  static vtkGdcmReader *New() {return new vtkGdcmReader;};
  vtkTypeMacro(vtkGdcmReader, vtkImageReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  void RemoveAllFileName(void);
  void AddFileName(const char* name);
  void SetFileName(const char *name);
  vtkGetObjectMacro(LookupTable,vtkLookupTable);

protected:
  vtkGdcmReader();
  ~vtkGdcmReader();
  virtual void ExecuteInformation();
  void ExecuteData(vtkDataObject *output);
  void BuildFileListFromPattern();
  int CheckFileCoherence();

private:
  void RemoveAllInternalFileName(void);
  void AddInternalFileName(const char* name);
  vtkLookupTable *LookupTable;

  //BTX
  // Number of columns of the image/volume to be loaded
  int NumColumns;
  // Number of lines of the image/volume to be loaded
  int NumLines;
  // Total number of planes (or images) of the stack to be build.
  int TotalNumberOfPlanes;
  // Number of scalar components of the image to be loaded (1=monochrome 3=rgb)
  int NumComponents;
  // Type of the image[s]: 8/16/32 bits, signed/unsigned:
  std::string ImageType;
  // Pixel size (in number of bytes):
  size_t PixelSize;
  // List of filenames to be read in order to build a stack of images
  // or volume. The order in the list shall be the order of the images.
  std::list<std::string> FileNameList;

  // List of filenames created in ExecuteInformation and used in
  // ExecuteData.
  // If FileNameList isn't empty, InternalFileNameList is a copy of
  //    FileNameList
  // Otherwise, InternalFileNameList correspond to the list of 
  //    files patterned
  std::list<std::string> InternalFileNameList;

  size_t LoadImageInMemory(std::string FileName, unsigned char * Dest,
                           const unsigned long UpdateProgressTarget,
                           unsigned long & UpdateProgressCount);
  //ETX
};
#endif

