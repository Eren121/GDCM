/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkGdcmWriter.h,v $
  Language:  C++
  Date:      $Date: 2005/03/03 11:39:24 $
  Version:   $Revision: 1.6 $
                                                                                
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

#include <vtkImageWriter.h>
#include <vtkLookupTable.h>
#include <string>

//-----------------------------------------------------------------------------
#define VTK_GDCM_WRITE_TYPE_EXPLICIT_VR 1
#define VTK_GDCM_WRITE_TYPE_IMPLICIT_VR 2
#define VTK_GDCM_WRITE_TYPE_ACR         3
#define VTK_GDCM_WRITE_TYPE_ACR_LIBIDO  4

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
   vtkSetMacro(WriteType, int);
   vtkGetMacro(WriteType, int);
   const char *GetWriteTypeAsString();

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
};

//-----------------------------------------------------------------------------
#endif
