// vtkGdcmReader.h
//-----------------------------------------------------------------------------
#ifndef __vtkGdcmReader_h
#define __vtkGdcmReader_h

#include <vtkImageReader.h>
#include <list>
#include <string>

//-----------------------------------------------------------------------------
class vtkLookupTable;

//-----------------------------------------------------------------------------
class VTK_EXPORT vtkGdcmReader : public vtkImageReader
{
public:
   static vtkGdcmReader *New() {return new vtkGdcmReader;};
   vtkTypeMacro(vtkGdcmReader, vtkImageReader);
   void PrintSelf(ostream& os, vtkIndent indent);

   virtual void RemoveAllFileName(void);
   virtual void AddFileName(const char* name);
   virtual void SetFileName(const char *name);
   
   // Description:
   // If this flag is set and the DICOM reader encounters a dicom file with 
   // lookup table the data will be kept as unsigned chars and a lookuptable 
   // will be exported and accessible through GetLookupTable() 
   vtkSetMacro(AllowLookupTable,int);
   vtkGetMacro(AllowLookupTable,int);
   vtkBooleanMacro(AllowLookupTable,int);

   vtkGetObjectMacro(LookupTable,vtkLookupTable);

protected:
   vtkGdcmReader();
   ~vtkGdcmReader();

   virtual void ExecuteInformation();
   virtual void ExecuteData(vtkDataObject *output);
   virtual void BuildFileListFromPattern();
   virtual int CheckFileCoherence();

private:
   void RemoveAllInternalFileName(void);
   void AddInternalFileName(const char* name);

   //BTX
   size_t LoadImageInMemory(std::string FileName, unsigned char * Dest,
                           const unsigned long UpdateProgressTarget,
                           unsigned long & UpdateProgressCount);
   //ETX

// Variables
   vtkLookupTable *LookupTable;
   vtkTimeStamp fileTime;
   int AllowLookupTable;

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
   //ETX
};

//-----------------------------------------------------------------------------
#endif

