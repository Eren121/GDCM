/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.h,v $
  Language:  C++
  Date:      $Date: 2007/09/17 12:16:02 $
  Version:   $Revision: 1.134 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef _GDCMFILE_H_
#define _GDCMFILE_H_

#include "gdcmDebug.h"
#include "gdcmDocument.h"
#include "gdcmTagKey.h"


namespace GDCM_NAME_SPACE 
{

class RLEFramesInfo;
class JPEGFragmentsInfo;

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
 * functionality of this class and belongs to gdcm::FileHelper.
 * \note  The various entries of the explicit value representation (VR) shall
 *        be managed within a dictionary which is shared by all File
 *        instances.
 * \note  The File::Set*Tag* family members cannot be defined as
 *        protected due to Swig limitations for as Has_a dependency between
 *        File and FileHelper.
 */
class GDCM_EXPORT File : public Document
{
   gdcmTypeMacro(File);

public:
   static File *New() {return new File();}

   // Loading
  // GDCM_LEGACY(bool Load( std::string const &filename ))
   bool Load();
   // Standard values and informations contained in the header
   bool IsReadable();

   void SetFourthDimensionLocation(uint16_t group, uint16_t elem) {
                   FourthDimensionLocation = TagKey(group, elem); }

   // Some heuristic based accessors, end user intended
   int GetImageNumber();
   ModalityType GetModality();

   int GetXSize();
   int GetYSize();
   int GetZSize();
   int GetTSize(); // unnormalized in DICOM V3

   bool GetSpacing(float &xspacing, float &yspacing, float &zspacing);
   float GetXSpacing();
   float GetYSpacing();
   float GetZSpacing();

   float GetXOrigin();
   float GetYOrigin();
   float GetZOrigin();

   float GetXCosineOnX();
   float GetXCosineOnY();  
   float GetXCosineOnZ();
   float GetYCosineOnX();
   float GetYCosineOnY();  
   float GetYCosineOnZ();   
     
   bool GetImageOrientationPatient( float iop[6] );
   bool GetImagePositionPatient( float ipp[3] );
   
   int GetBitsStored();
   int GetBitsAllocated();
   int GetHighBitPosition();
   int GetSamplesPerPixel();
   int GetPlanarConfiguration();
   int GetPixelSize();
   std::string GetPixelType();
   bool IsSignedPixelData();
   bool IsMonochrome();
   bool IsMonochrome1();
   bool IsPaletteColor();
   bool IsYBRFull();

   bool HasLUT();
   int GetLUTNbits();

   // For rescaling graylevel:
   bool GetRescaleSlopeIntercept(double &slope, double &intercept);   
   double GetRescaleIntercept();
   double GetRescaleSlope();

   int GetNumberOfScalarComponents();
   int GetNumberOfScalarComponentsRaw();

   /// Accessor to  File::GrPixel
   uint16_t GetGrPixel()  { return GrPixel; }
   /// Accessor to  File::NumPixel
   uint16_t GetNumPixel() { return NumPixel; }

   size_t GetPixelOffset();
   size_t GetPixelAreaLength();

   /// returns the RLE info
   RLEFramesInfo *GetRLEInfo() { return RLEInfo; }
   /// Returns the JPEG Fragments info
   JPEGFragmentsInfo *GetJPEGInfo() { return JPEGInfo; }

// Anonymization process
   void AddAnonymizeElement (uint16_t group, uint16_t elem, 
                             std::string const &value);
   /// Clears the list of elements to be anonymized
   void ClearAnonymizeList() { UserAnonymizeList.clear(); }      
   void AnonymizeNoLoad();
   /// Replace patient's own information by info from the Anonymization list
   bool AnonymizeFile();

   bool Write(std::string fileName, FileType filetype);

protected:
   File();
   virtual ~File();
   /// \brief Protect the Writer from writing illegal groups
   bool MayIWrite(uint16_t group)
     { if (group < 8 &&  group !=2 ) return false; else return true; }
      
   /// Store the RLE frames info obtained during parsing of pixels.
   RLEFramesInfo *RLEInfo;
   /// Store the JPEG fragments info obtained during parsing of pixels.
   JPEGFragmentsInfo *JPEGInfo;

   /// \brief In some cases (e.g. for some ACR-NEMA images) the Entry Element
   /// Number of the 'Pixel Element' is *not* found at 0x0010. In order to
   /// make things easier the parser shall store the proper value in
   /// NumPixel to provide a unique access facility. 
   uint16_t NumPixel;
   /// \brief In some cases (e.g. for some ACR-NEMA images) the header entry for
   /// the group of pixels is *not* found at 0x7fe0. 
   /// In order to make things easier the parser shall store the proper value
   /// in GrPixel to provide a unique access facility.
   uint16_t GrPixel;
   /// \brief allows user to tell gdcm in which DataElement is stored 
   ///the -unnormalized- 4th Dimension   
   TagKey FourthDimensionLocation;

private:
   bool DoTheLoadingJob();
   void ComputeRLEInfo();
   void ComputeJPEGFragmentInfo();
   bool     ReadTag(uint16_t, uint16_t);
   uint32_t ReadTagLength(uint16_t, uint16_t);
   void ReadEncapsulatedBasicOffsetTable();
   uint32_t *BasicOffsetTableItemValue;

};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
