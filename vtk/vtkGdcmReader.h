// $Header: /cvs/public/gdcm/vtk/vtkGdcmReader.h,v 1.1 2003/05/05 14:13:59 frog Exp $

#ifndef __vtkGdcmReader_h
#define __vtkGdcmReader_h

#include "vtkImageReader.h"

class VTK_EXPORT vtkGdcmReader : public vtkImageReader
{
public:
  static vtkGdcmReader *New() {return new vtkGdcmReader;};
  vtkTypeMacro(vtkGdcmReader, vtkImageReader);
  void PrintSelf(ostream& os, vtkIndent indent);
protected:
  vtkGdcmReader();
  ~vtkGdcmReader();
  void ExecuteData(vtkDataObject *output);
  virtual void ExecuteInformation();
};
#endif

