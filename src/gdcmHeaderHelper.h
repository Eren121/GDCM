/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmHeaderHelper.h,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:48 $
  Version:   $Revision: 1.16 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMHEADERHELPER_H
#define GDCMHEADERHELPER_H

#include "gdcmHeader.h"

//-----------------------------------------------------------------------------
// Dicom Part 3.3 Compliant
enum ModalityType {
   Unknow,
   AU,       // Voice Audio
   AS,       // Angioscopy
   BI,       // Biomagnetic Imaging
   CF,       // Cinefluorography
   CP,       // Culposcopy
   CR,       // Computed Radiography
   CS,       // Cystoscopy
   CT,       // Computed Tomography
   DD,       // Duplex Dopler
   DF,       // Digital Fluoroscopy
   DG,       // Diaphanography
   DM,       // Digital Microscopy
   DS,       // Digital Substraction Angiography
   DX,       // Digital Radiography
   ECG,      // Echocardiography
   EPS,      // Basic Cardiac EP
   ES,       // Endoscopy
   FA,       // Fluorescein Angiography
   FS,       // Fundoscopy
   HC,       // Hard Copy
   HD,       // Hemodynamic
   LP,       // Laparoscopy
   LS,       // Laser Surface Scan
   MA,       // Magnetic Resonance Angiography
   MR,       // Magnetic Resonance
   NM,       // Nuclear Medicine
   OT,       // Other
   PT,       // Positron Emission Tomography
   RF,       // Radio Fluoroscopy
   RG,       // Radiographic Imaging
   RTDOSE,   // Radiotherapy Dose
   RTIMAGE,  // Radiotherapy Image
   RTPLAN,   // Radiotherapy Plan
   RTSTRUCT, // Radiotherapy Structure Set
   SM,       // Microscopic Imaging
   ST,       // Single-photon Emission Computed Tomography
   TG,       // Thermography
   US,       // Ultrasound
   VF,       // Videofluorography
   XA,       // X-Ray Angiography
   XC        // Photographic Imaging
};
      
//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmHeaderHelper
 * \brief  
 *
 * - This class is meant to *interpret* data given from gdcmHeader
 * - That is to say :
 *  - it will help other dev to link against there lib
 *  - return value instead of string
 *  - will be able to search for data at some other place
 *  - return *default value* which is not a gdcmHeader goal
 *  - ...
 */
class GDCM_EXPORT gdcmHeaderHelper : public gdcmHeader {
public:
   gdcmHeaderHelper();
   gdcmHeaderHelper(const char *filename, 
              bool  exception_on_error = false, 
              bool  enable_sequences   = false,
	      bool  ignore_shadow      = false);

   int GetPixelSize();
   std::string GetPixelType();
   
   float GetXSpacing();
   float GetYSpacing();
   float GetZSpacing();
   
   // Usefull for rescaling graylevel:
   float GetRescaleIntercept();
   float GetRescaleSlope();

   int GetNumberOfScalarComponents();
   int GetNumberOfScalarComponentsRaw();

   std::string GetStudyUID();
   std::string GetSeriesUID();
   std::string GetClassUID();
   std::string GetInstanceUID();
   
   /**
    * change GetXImagePosition -> GetXOrigin in order not to confuse reader
    * -# GetXOrigin can return default value (=0) if it was not ImagePosition
    * -# Image Position is different in dicomV3 <> ACR NEMA -> better use generic
    * name
   */
   float GetXOrigin();
   float GetYOrigin();
   float GetZOrigin();
   
   int GetImageNumber();
   ModalityType GetModality();
   
   void GetImageOrientationPatient( float* iop );
};

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmSerieHeaderHelper
 * \brief  
 *
 * - This class should be used for a stack of 2D dicom images.
 * - For a multiframe dicom image better use directly gdcmHeaderHelper
*/
class GDCM_EXPORT gdcmSerieHeaderHelper {
public:
    gdcmSerieHeaderHelper() {};
    ~gdcmSerieHeaderHelper();

   void AddFileName(std::string filename); //should return bool or throw error ?
   void AddGdcmFile(gdcmHeaderHelper *file);
   void SetDirectory(std::string dir);
   void OrderGdcmFileList();
   
   inline gdcmHeaderHelper *GetGdcmHeader()
   {
      //Assume all element in the list have the same global infos
      return CoherentGdcmFileList.front();
   }
   
   std::list<gdcmHeaderHelper*>& GetGdcmFileList();

private:
   bool ImagePositionPatientOrdering();
   bool ImageNumberOrdering();
   bool FileNameOrdering();
   
   std::list<gdcmHeaderHelper*> CoherentGdcmFileList;
};

//-----------------------------------------------------------------------------
#endif
