/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkGdcmWriter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/17 14:46:18 $
  Version:   $Revision: 1.7 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
                                                                                
#ifndef __vtkGdcmWriter_h
#define __vtkGdcmWriter_h

#include "gdcmCommon.h" // To avoid warnings concerning the std
#include "gdcmFile.h"   // for gdcm::File

#include <vtkImageWriter.h>
#include <vtkLookupTable.h>
#include <string>

//-----------------------------------------------------------------------------

#define VTK_GDCM_WRITE_TYPE_EXPLICIT_VR 1
#define VTK_GDCM_WRITE_TYPE_IMPLICIT_VR 2
#define VTK_GDCM_WRITE_TYPE_ACR         3
#define VTK_GDCM_WRITE_TYPE_ACR_LIBIDO  4

#define VTK_GDCM_WRITE_TYPE_USER_OWN_IMAGE          1
#define VTK_GDCM_WRITE_TYPE_FILTERED_IMAGE          2
#define VTK_GDCM_WRITE_TYPE_CREATED_IMAGE           3
#define VTK_GDCM_WRITE_TYPE_UNMODIFIED_PIXELS_IMAGE 4

//-----------------------------------------------------------------------------
class VTK_EXPORT vtkGdcmWriter : public vtkImageWriter
{
public:
   static vtkGdcmWriter *New();
   vtkTypeRevisionMacro(vtkGdcmWriter, vtkImageWriter);

   void PrintSelf(ostream &os, vtkIndent indent);

   vtkSetObjectMacro(LookupTable, vtkLookupTable);
   vtkGetObjectMacro(LookupTable, vtkLookupTable);

   void SetWriteTypeToDcmImplVR(){SetWriteType(VTK_GDCM_WRITE_TYPE_EXPLICIT_VR);};
   void SetWriteTypeToDcmExplVR(){SetWriteType(VTK_GDCM_WRITE_TYPE_IMPLICIT_VR);};
   void SetWriteTypeToAcr()      {SetWriteType(VTK_GDCM_WRITE_TYPE_ACR);        };
   void SetWriteTypeToAcrLibido(){SetWriteType(VTK_GDCM_WRITE_TYPE_ACR_LIBIDO); };
   
   void SetContentTypeToUserOwnImage()         {SetContentType(VTK_GDCM_WRITE_TYPE_USER_OWN_IMAGE);};   
   void SetContentTypeToFilteredImage()        {SetContentType(VTK_GDCM_WRITE_TYPE_FILTERED_IMAGE);};   
   void SetContentTypeToUserCreatedImage()     {SetContentType(VTK_GDCM_WRITE_TYPE_CREATED_IMAGE);};   
   void SetContentTypeToUnmodifiedPixelsImage(){SetContentType(VTK_GDCM_WRITE_TYPE_UNMODIFIED_PIXELS_IMAGE);};   
   
   vtkSetMacro(WriteType, int);
   vtkGetMacro(WriteType, int);
   const char *GetWriteTypeAsString();

//BTX
   vtkSetMacro(GdcmFile, gdcm::File *);
   vtkGetMacro(GdcmFile, gdcm::File *);
//ETX

   vtkSetMacro(ContentType, int);
   vtkGetMacro(ContentType, int);

     
protected:
   vtkGdcmWriter();
   ~vtkGdcmWriter();

  virtual void RecursiveWrite(int axis, vtkImageData *image, ofstream *file);
  virtual void RecursiveWrite(int axis, vtkImageData *image, 
                              vtkImageData *cache, ofstream *file);
  void WriteDcmFile(char *fileName, vtkImageData *image);

private:
// Variables
   vtkLookupTable *LookupTable;
   int WriteType;
//BTX
   gdcm::File *GdcmFile;
//ETX
   int ContentType;
   
};

//-----------------------------------------------------------------------------
#endif
