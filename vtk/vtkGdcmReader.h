// $Header: /cvs/public/gdcm/vtk/vtkGdcmReader.h,v 1.2 2003/05/29 16:58:24 frog Exp $

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
  void AddFileName(const char* name);
  void SetFileName(const char *name);
protected:
  vtkGdcmReader();
  ~vtkGdcmReader();
  virtual void ExecuteInformation();
  void ExecuteData(vtkDataObject *output);
  void BuilFileListFromPattern();
  bool CheckFileCoherence();
  // List of filenames to be read in order to build a stack of images
  // or volume. The order in the list shall be the order of the images.
  //BTX
  std::list<std::string> FileNameList;
  //ETX
};
#endif

