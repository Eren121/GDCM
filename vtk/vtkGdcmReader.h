/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkGdcmReader.h,v $
  Language:  C++
  Date:      $Date: 2005/08/22 12:30:36 $
  Version:   $Revision: 1.25 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
                                                                                
#ifndef __vtkGdcmReader_h
#define __vtkGdcmReader_h

#include "gdcmCommon.h" // To avoid warnings concerning the std
#include "gdcmFile.h"

#include <vtkImageReader.h>
#include <list>
#include <string>
#include <vector>

typedef void (*VOID_FUNCTION_PUINT8_PFILE_POINTER)(uint8_t *, gdcm::File *);

//-----------------------------------------------------------------------------
class vtkLookupTable;

//-----------------------------------------------------------------------------
class VTK_EXPORT vtkGdcmReader : public vtkImageReader
{
public:
   static vtkGdcmReader *New();
   vtkTypeRevisionMacro(vtkGdcmReader, vtkImageReader);
   void PrintSelf(ostream& os, vtkIndent indent);

   virtual void RemoveAllFileName(void);
   virtual void AddFileName(const char *name);
   virtual void SetFileName(const char *name);
   void SetCoherentFileList( std::vector<gdcm::File* > *cfl) {
                                                      CoherentFileList = cfl; }    
   //void SetCheckFileCoherenceLight();
   vtkSetMacro(AllowLightChecking, int);
   vtkGetMacro(AllowLightChecking, int);
   vtkBooleanMacro(AllowLightChecking, int);

   void SetUserFunction (VOID_FUNCTION_PUINT8_PFILE_POINTER userFunc )
                        { UserFunction = userFunc; }   
   // Description:
   // If this flag is set and the DICOM reader encounters a dicom file with 
   // lookup table the data will be kept as unsigned chars and a lookuptable 
   // will be exported and accessible through GetLookupTable() 
   vtkSetMacro(AllowLookupTable, int);
   vtkGetMacro(AllowLookupTable, int);
   vtkBooleanMacro(AllowLookupTable, int);

   vtkGetObjectMacro(LookupTable, vtkLookupTable);

/**
 * \brief Sets the LoadMode as a boolean string. 
 *        NO_SEQ, NO_SHADOW, NO_SHADOWSEQ... (nothing more, right now)
 *        WARNING : before using NO_SHADOW, be sure *all* your files
 *        contain accurate values in the 0x0000 element (if any) 
 *        of *each* Shadow Group. The parser will fail if the size is wrong !
 * @param   mode Load mode to be used    
 */
   void SetLoadMode (int mode) { LoadMode = mode; }

protected:
   vtkGdcmReader();
   ~vtkGdcmReader();

   virtual void ExecuteInformation();
   virtual void ExecuteData(vtkDataObject *output);
   virtual void BuildFileListFromPattern();
   virtual int CheckFileCoherence();
   virtual int CheckFileCoherenceLight();
   virtual int CheckFileCoherenceAlreadyDone();
private:
   void RemoveAllInternalFileName(void);
   void AddInternalFileName(const char* name);

   //BTX
   size_t LoadImageInMemory(std::string fileName, unsigned char *dest,
                           const unsigned long updateProgressTarget,
                           unsigned long &updateProgressCount);

   size_t LoadImageInMemory(gdcm::File *f, unsigned char *dest,
                           const unsigned long updateProgressTarget,
                           unsigned long &updateProgressCount);

   size_t DoTheLoadingJob (gdcm::File *f,
                           unsigned char *dest,
                           const unsigned long updateProgressTarget,
                           unsigned long &updateProgressCount);
   //ETX

// Variables
   vtkLookupTable *LookupTable;
   vtkTimeStamp fileTime;
   int AllowLookupTable;

   int AllowLightChecking;

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
   std::vector<gdcm::File* > *CoherentFileList;
  
   //ETX

   /// \brief Bit string integer (each one considered as a boolean)
   ///        Bit 0 : Skip Sequences,    if possible
   ///        Bit 1 : Skip Shadow Groups if possible
   ///        Bit 2 : Skip Sequences inside a Shadow Group, if possible
   ///        Probabely (?), some more to add
   int LoadMode;

   /// Pointer to a user suplied function to allow modification of pixel order
   VOID_FUNCTION_PUINT8_PFILE_POINTER UserFunction;

};

//-----------------------------------------------------------------------------
#endif

