// $Header: /cvs/public/gdcm/vtk/vtkGdcmReader.h,v 1.7 2003/07/04 17:12:43 regrain Exp $

#ifndef __vtkGdcmReader_h
#define __vtkGdcmReader_h

#include <list>
#include <string>
#include "vtkImageReader.h"

class VTK_EXPORT vtkGdcmReader : public vtkImageReader
{
public:
  static vtkGdcmReader *New() {return new vtkGdcmReader;};
  vtkTypeMacro(vtkGdcmReader, vtkImageReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  void RemoveAllFileName(void);
  void AddFileName(const char* name);
  void SetFileName(const char *name);

protected:
  vtkGdcmReader();
  ~vtkGdcmReader();
  virtual void ExecuteInformation();
  void ExecuteData(vtkDataObject *output);
  void BuildFileListFromPattern();
  int CheckFileCoherence();

private:
  //BTX
  // Number of columns of the image/volume to be loaded
  int NumColumns;
  // Number of lines of the image/volume to be loaded
  int NumLines;
  // Total number of planes (or images) of the stack to be build.
  int TotalNumberOfPlanes;
  // Type of the image[s]: 8/16/32 bits, signed/unsigned:
  std::string ImageType;
  // Pixel size (in number of bytes):
  size_t PixelSize;
  // List of filenames to be read in order to build a stack of images
  // or volume. The order in the list shall be the order of the images.
  std::list<std::string> FileNameList;

  size_t LoadImageInMemory(std::string FileName, unsigned char * Dest,
                           const unsigned long UpdateProgressTarget,
                           unsigned long & UpdateProgressCount);
  //ETX
};
#endif

