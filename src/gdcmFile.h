/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.h,v $
  Language:  C++
  Date:      $Date: 2005/01/23 10:12:34 $
  Version:   $Revision: 1.99 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMFILE_H
#define GDCMFILE_H

#include "gdcmDocument.h"

namespace gdcm 
{

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
/**
 * \brief DICOM elements and their corresponding values (and
 * additionaly the corresponding DICOM dictionary entry) of the header
 * of a DICOM file.
 *
 * The typical usage of instances of class File is to classify a set of
 * dicom files according to header information e.g. to create a file hierarchy
 * reflecting the Patient/Study/Serie informations, or extracting a given
 * SerieId. Accessing the content (image[s] or volume[s]) is beyond the
 * functionality of this class and belongs to gdmcFile.
 * \note  The various entries of the explicit value representation (VR) shall
 *        be managed within a dictionary which is shared by all File
 *        instances.
 * \note  The File::Set*Tag* family members cannot be defined as
 *        protected due to Swig limitations for as Has_a dependency between
 *        File and FileHelper.
 */

//-----------------------------------------------------------------------------

class GDCM_EXPORT File : public Document
{
protected:
   /// \brief In some cases (e.g. for some ACR-NEMA images) the Entry Element
   /// Number of the 'Pixel Element' is *not* found at 0x0010. In order to
   /// make things easier the parser shall store the proper value in
   /// NumPixel to provide a unique access facility. 
   uint16_t NumPixel;
   /// \brief In some cases (e.g. for some ACR-NEMA images) the header entry for
   /// the group of pixels is *not* found at 0x7fe0. In order to
   /// make things easier the parser shall store the proper value in
   /// GrPixel to provide a unique access facility.
   uint16_t GrPixel;

public:
   File();
   File( std::string const &filename );
 
   ~File();

   // Standard values and informations contained in the header
   bool IsReadable();

   // Some heuristic based accessors, end user intended 
   int GetBitsStored();
   int GetBitsAllocated();
   int GetSamplesPerPixel();
   int GetPlanarConfiguration();
   int GetPixelSize();
   int GetHighBitPosition();
   bool IsSignedPixelData();
   bool IsMonochrome();
   bool IsPaletteColor();
   bool IsYBRFull();

   std::string GetPixelType();
   size_t GetPixelOffset();
   size_t GetPixelAreaLength();

   //Some image informations needed for third package imaging library
   int GetXSize();
   int GetYSize();
   int GetZSize();

   float GetXSpacing();
   float GetYSpacing();
   float GetZSpacing();

   // For rescaling graylevel:
   float GetRescaleIntercept();
   float GetRescaleSlope();

   int GetNumberOfScalarComponents();
   int GetNumberOfScalarComponentsRaw();

   // To organize DICOM files based on their x,y,z position 
   void GetImageOrientationPatient( float iop[6] );

   int GetImageNumber();
   ModalityType GetModality();

   float GetXOrigin();
   float GetYOrigin();
   float GetZOrigin();

   bool   HasLUT();
   int    GetLUTNbits();

   /// Accessor to \ref File::GrPixel
   uint16_t GetGrPixel()  { return GrPixel; }
   
   /// Accessor to \ref File::NumPixel
   uint16_t GetNumPixel() { return NumPixel; }

   bool Write(std::string fileName, FileType filetype);

   /// Initialize DICOM File when none
   void InitializeDefaultFile();
 
protected:
   /// Replace patient's specific information by 'anonymous'
   bool AnonymizeFile();

private:

};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
