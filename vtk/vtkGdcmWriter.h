// vtkGdcmWriter.h
//-----------------------------------------------------------------------------
#ifndef __vtkGdcmWriter_h
#define __vtkGdcmWriter_h

#include "gdcmCommon.h" // To avoid warnings concerning the std

#include <vtkImageWriter.h>
#include <vtkLookupTable.h>
#include <list>
#include <string>

//-----------------------------------------------------------------------------
class VTK_EXPORT vtkGdcmWriter : public vtkImageWriter
{
public:
   static vtkGdcmWriter *New();
   vtkTypeRevisionMacro(vtkGdcmWriter, vtkImageWriter);

   void PrintSelf(ostream& os, vtkIndent indent);

   vtkSetObjectMacro(LookupTable,vtkLookupTable);
   vtkGetObjectMacro(LookupTable,vtkLookupTable);

protected:
   vtkGdcmWriter();
   ~vtkGdcmWriter();

  virtual void RecursiveWrite(int dim, vtkImageData *region, ofstream *file);

private:
// Variables
   vtkLookupTable *LookupTable;
};

//-----------------------------------------------------------------------------
#endif

