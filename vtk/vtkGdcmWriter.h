/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: vtkGdcmWriter.h,v $
  Language:  C++
  Date:      $Date: 2004/12/09 10:59:59 $
  Version:   $Revision: 1.3 $
                                                                                
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

  virtual void RecursiveWrite(int axis, vtkImageData *image, ofstream *file);
  virtual void RecursiveWrite(int axis, vtkImageData *image, 
                              vtkImageData *cache, ofstream *file);
  void WriteDcmFile(char *fileName,vtkImageData *image);

private:
// Variables
   vtkLookupTable *LookupTable;
};

//-----------------------------------------------------------------------------
#endif

