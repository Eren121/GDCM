/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.cxx,v $
  Language:  C++
  Date:      $Date: 2009/05/19 15:08:36 $
  Version:   $Revision: 1.344 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

//
// --------------  Remember ! ----------------------------------
//
// Image Position (Patient)                            (0020,0032):
// If not found (ACR_NEMA) we try Image Position       (0020,0030)
// If not found (ACR-NEMA), we consider Slice Location (0020,1041)
//                                   or Location       (0020,0050) 
//                                   as the Z coordinate, 
// 0. for all the coordinates if nothing is found
//
// Image Position (Patient) (0020,0032) VM=3
// -->
//  The attribute Patient Orientation (0020,0020) from the General Image Module 
// is of type 2C and has the condition Required if image does not require 
// Image Orientation (0020,0037) and Image Position (0020,0032). 
// However, if the image does require the attributes 
// - Image Orientation (Patient) (0020,0037), VM=6
// - Image Position (Patient)    (0020,0032), VM=3
// then attribute Patient Orientation (0020,0020) should not be present
//  in the images.
//
// Remember also :
// Patient Position (0018,5100) values :

//  HFS   = Head First-Supine, where increasing (positive axis direction) :
//     X -> to the direction pointed to by the patient's oustretched left arm
//     Y -> to the anterior-to-posterior direction in the patient's body
//     Z -> to the feet-to-head direction in the patient's body

//  HFP   = Head First-Prone, where increasing (positive axis direction) :
//     X -> to the direction pointed to by the patient's oustretched left arm
//     Y -> to the anterior-to-posterior direction in the patient's body
//     Z -> to the feet-to-head direction in the patient's body

//  FFS  = Feet First-Supine, where increasing (positive axis direction) :
//     X -> to the direction pointed to by the patient's oustretched left arm
//     Y -> to the anterior-to-posterion direction in the patient's body
//     Z -> to the feet-to-head direction in the patient's body

//  FFP  = Feet First-Prone, where increasing (positive axis direction) :
//     X -> to the direction pointed to by the patient's oustretched left arm
//     Y -> to the posterior-to-anterior direction in the patient's body
//     Z -> to the feet-to-head direction in the patient's body

// HFDR = Head First-Decubitus Right
// HFDL = Head First-Decubitus Left
// FFDR = Feet First-Decubitus Right
// FFDL = Feet First-Decubitus Left

//  we can also find (non standard!)     

// SEMIERECT
// SUPINE

// CS 2 Patient Orientation (0020 0020)
//    When the coordinates of the image 
//    are always present, this field is almost never used.
//    Better we don't trust it too much ...
//    Found Values are :
//     L\P
//     L\FP
//     P\F
//     L\F
//     P\FR
//     R\F
//
// (0020|0037) [Image Orientation (Patient)] [1\0\0\0\1\0 ]

               
// ---------------------------------------------------------------
//
#include "gdcmFile.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"
#include "gdcmTS.h"
#include "gdcmSeqEntry.h"
#include "gdcmRLEFramesInfo.h"
#include "gdcmJPEGFragmentsInfo.h"
#include "gdcmDataEntry.h"
#include "gdcmSQItem.h"

#include <vector>
#include <stdio.h>  //sscanf
#include <stdlib.h> // for atoi

namespace GDCM_NAME_SPACE
{

//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \brief Constructor used when we want to generate dicom files from scratch
 */
File::File():
   Document()
{
   RLEInfo  = new RLEFramesInfo;
   JPEGInfo = new JPEGFragmentsInfo;
   GrPixel  = 0x7fe0;  // to avoid further troubles
   NumPixel = 0x0010;
   BasicOffsetTableItemValue = 0;
   FourthDimensionLocation = TagKey(0,0);
}


/**
 * \brief   Canonical destructor.
 */
File::~File()
{
   if ( RLEInfo )
      delete RLEInfo;
   if ( JPEGInfo )
      delete JPEGInfo;
   delete[] BasicOffsetTableItemValue;
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Loader  
 * @return false if file cannot be open or no swap info was found,
 *         or no tag was found.
 */
bool File::Load( ) 
{
   if ( ! this->Document::Load( ) )
      return false;

    return DoTheLoadingJob( );   
}

/**
 * \brief   Does the Loading Job (internal use only)
 * @return false if file cannot be open or no swap info was found,
 *         or no tag was found.
 */
bool File::DoTheLoadingJob( ) 
{
   // for some ACR-NEMA images GrPixel, NumPixel is *not* 7fe0,0010
   // We may encounter the 'RETired' (0x0028, 0x0200) tag
   // (Image Location") . This entry contains the number of
   // the group that contains the pixel data (hence the "Pixel Data"
   // is found by indirection through the "Image Location").
   // Inside the group pointed by "Image Location" the searched element
   // is conventionally the element 0x0010 (when the norm is respected).
   // When the "Image Location" is missing we default to group 0x7fe0.
   // Note: this IS the right place for the code
 
   // Image Location
   const std::string &imgLocation = GetEntryString(0x0028, 0x0200);
   if ( imgLocation == GDCM_UNFOUND )
   {
      // default value
      GrPixel = 0x7fe0;
   }
   else
   {
      GrPixel = (uint16_t) atoi( imgLocation.c_str() );
   }   

   // sometimes Image Location value doesn't follow
   // the supposed processor endianness.
   // see gdcmData/cr172241.dcm
   if ( GrPixel == 0xe07f )
   {
      GrPixel = 0x7fe0;
   }

   if ( GrPixel != 0x7fe0 )
   {
      // This is a kludge for old dirty Philips imager.
      NumPixel = 0x1010;
   }
   else
   {
      NumPixel = 0x0010;
   }

   // Now, we know GrPixel and NumPixel.
   // Let's create a VirtualDictEntry to allow a further VR modification
   // and force VR to match with BitsAllocated.
   DocEntry *entry = GetDocEntry(GrPixel, NumPixel); 
   if ( entry != 0 )
   {
      // Compute the RLE or JPEG info
      OpenFile();
      const std::string &ts = GetTransferSyntax();
      Fp->seekg( entry->GetOffset(), std::ios::beg );
      if ( Global::GetTS()->IsRLELossless(ts) ) 
         ComputeRLEInfo();
      else if ( Global::GetTS()->IsJPEG(ts) )
         ComputeJPEGFragmentInfo();
      CloseFile();

      // Create a new DataEntry to change the DictEntry
      // The changed DictEntry will have 
      // - a correct PixelVR OB or OW)
      // - the name to "Pixel Data"
      
       //==>Take it easy!
       //==> Just change the VR !

/* 
      DataEntry *oldEntry = dynamic_cast<DataEntry *>(entry);
      if (oldEntry)
      {
         VRKey PixelVR;
         // 8 bits allocated is a 'O Bytes' , as well as 24 (old ACR-NEMA RGB)
         // more than 8 (i.e 12, 16) is a 'O Words'
         if ( GetBitsAllocated() == 8 || GetBitsAllocated() == 24 ) 
            PixelVR = "OB";
         else
            PixelVR = "OW";

         // Change only made if usefull
         if ( PixelVR != oldEntry->GetVR() )
         {
            //DictEntry* newDict = DictEntry::New(GrPixel,NumPixel,
            //                                    PixelVR,"1","Pixel Data");
            //DataEntry *newEntry = DataEntry::New(newDict);
            //newDict->Delete();
            //newEntry->Copy(entry);
            //newEntry->SetBinArea(oldEntry->GetBinArea(),oldEntry->IsSelfArea());
            //oldEntry->SetSelfArea(false);

            //RemoveEntry(oldEntry);
            //AddEntry(newEntry);
            //newEntry->Delete();
         }
      }
*/
         VRKey PixelVR;
         // 8 bits allocated is a 'OB(ytes)' , as well as 24 (old ACR-NEMA RGB)
         // more than 8 (i.e 12, 16) is a 'OW(ords)'
         if ( GetBitsAllocated() == 8 || GetBitsAllocated() == 24 ) 
            PixelVR = "OB";
         else
            PixelVR = "OW";
         // Change only made if usefull
         if ( PixelVR != entry->GetVR() )
         { 
            entry->SetVR(PixelVR);  
         }         
   }
   return true;
}

/**
 * \brief  This predicate, based on hopefully reasonable heuristics,
 *         decides whether or not the current File was properly parsed
 *         and contains the mandatory information for being considered as
 *         a well formed and usable Dicom/Acr File.
 * @return true when File is the one of a reasonable Dicom/Acr file,
 *         false otherwise. 
 */
bool File::IsReadable()
{
   if ( !Document::IsReadable() )
   {
      return false;
   }

   const std::string &res = GetEntryString(0x0028, 0x0005);
   if ( res != GDCM_UNFOUND && atoi(res.c_str()) > 4 )
   {
      gdcmWarningMacro("Wrong Image Dimensions" << res);
      return false; // Image Dimensions
   }
   bool b0028_0100 = true;
   if ( !GetDocEntry(0x0028, 0x0100) )
   {
      gdcmWarningMacro("Bits Allocated (0028|0100) not found"); 
      //return false; // "Bits Allocated"
      b0028_0100 = false;
   }
   bool b0028_0101 = true;
   if ( !GetDocEntry(0x0028, 0x0101) )
   {
      gdcmWarningMacro("Bits Stored (0028|0101) not found");
      //return false; // "Bits Stored"
      b0028_0101 = false;
   }
   bool b0028_0102 = true;
   if ( !GetDocEntry(0x0028, 0x0102) )
   {
      gdcmWarningMacro("Hight Bit (0028|0102) not found"); 
      //return false; // "High Bit"
      b0028_0102 = false;
   }
   bool b0028_0103 = true;
   if ( !GetDocEntry(0x0028, 0x0103) )
   {
      gdcmWarningMacro("Pixel Representation (0028|0103) not found");
      //return false; // "Pixel Representation" i.e. 'Sign' ( 0 : unsigned, 1 : signed)
      b0028_0103 = false;
   }

   if ( !b0028_0100 && !b0028_0101 && !b0028_0102 && !b0028_0103)
   {
      gdcmWarningMacro("Too much mandatory Tags missing !");
      return false;
   }

   if ( !GetDocEntry(GrPixel, NumPixel) )
   {
      gdcmWarningMacro("Pixel Dicom Element " << std::hex <<
                        GrPixel << "|" << NumPixel << "not found");
      return false; // Pixel Dicom Element not found :-(
   }
   return true;
}

/**
 * \brief gets the info from 0020,0013 : Image Number else 0.
 * @return image number
 */
int File::GetImageNumber()
{
   //0020 0013 : Image Number
   std::string strImNumber = GetEntryString(0x0020,0x0013);
   if ( strImNumber != GDCM_UNFOUND )
   {
      return atoi( strImNumber.c_str() );
   }
   return 0;   //Hopeless
}

/**
 * \brief gets the info from 0008,0060 : Modality
 * @return Modality Type
 */
ModalityType File::GetModality()
{
   // 0008 0060 : Modality
   std::string strModality = GetEntryString(0x0008,0x0060);
   if ( strModality != GDCM_UNFOUND )
   {
           if ( strModality.find("AU")  < strModality.length()) return AU;
      else if ( strModality.find("AS")  < strModality.length()) return AS;
      else if ( strModality.find("BI")  < strModality.length()) return BI;
      else if ( strModality.find("CF")  < strModality.length()) return CF;
      else if ( strModality.find("CP")  < strModality.length()) return CP;
      else if ( strModality.find("CR")  < strModality.length()) return CR;
      else if ( strModality.find("CT")  < strModality.length()) return CT;
      else if ( strModality.find("CS")  < strModality.length()) return CS;
      else if ( strModality.find("DD")  < strModality.length()) return DD;
      else if ( strModality.find("DF")  < strModality.length()) return DF;
      else if ( strModality.find("DG")  < strModality.length()) return DG;
      else if ( strModality.find("DM")  < strModality.length()) return DM;
      else if ( strModality.find("DS")  < strModality.length()) return DS;
      else if ( strModality.find("DX")  < strModality.length()) return DX;
      else if ( strModality.find("ECG") < strModality.length()) return ECG;
      else if ( strModality.find("EPS") < strModality.length()) return EPS;
      else if ( strModality.find("FA")  < strModality.length()) return FA;
      else if ( strModality.find("FS")  < strModality.length()) return FS;
      else if ( strModality.find("HC")  < strModality.length()) return HC;
      else if ( strModality.find("HD")  < strModality.length()) return HD;
      else if ( strModality.find("LP")  < strModality.length()) return LP;
      else if ( strModality.find("LS")  < strModality.length()) return LS;
      else if ( strModality.find("MA")  < strModality.length()) return MA;
      else if ( strModality.find("MR")  < strModality.length()) return MR;
      else if ( strModality.find("NM")  < strModality.length()) return NM;
      else if ( strModality.find("OT")  < strModality.length()) return OT;
      else if ( strModality.find("PT")  < strModality.length()) return PT;
      else if ( strModality.find("RF")  < strModality.length()) return RF;
      else if ( strModality.find("RG")  < strModality.length()) return RG;
      else if ( strModality.find("RTDOSE")   
                                        < strModality.length()) return RTDOSE;
      else if ( strModality.find("RTIMAGE")  
                                        < strModality.length()) return RTIMAGE;
      else if ( strModality.find("RTPLAN")
                                        < strModality.length()) return RTPLAN;
      else if ( strModality.find("RTSTRUCT") 
                                        < strModality.length()) return RTSTRUCT;
      else if ( strModality.find("SM")  < strModality.length()) return SM;
      else if ( strModality.find("ST")  < strModality.length()) return ST;
      else if ( strModality.find("TG")  < strModality.length()) return TG;
      else if ( strModality.find("US")  < strModality.length()) return US;
      else if ( strModality.find("VF")  < strModality.length()) return VF;
      else if ( strModality.find("XA")  < strModality.length()) return XA;
      else if ( strModality.find("XC")  < strModality.length()) return XC;

      else
      {
         /// \todo throw error return value ???
         /// specified <> unknown in our database
         return Unknow;
      }
   }
   return Unknow;
}

/**
 * \brief   Retrieve the number of columns of image.
 * @return  The encountered size when found, 0 by default.
 *          0 means the file is NOT USABLE. The caller will have to check
 */
int File::GetXSize()
{
   DataEntry *entry = GetDataEntry(0x0028,0x0011);
   if( entry )
      return (int)entry->GetValue(0);
   return 0;
}

/**
 * \brief   Retrieve the number of lines of image.
 * \warning The defaulted value is 1 as opposed to File::GetXSize()
 * @return  The encountered size when found, 1 by default 
 *          (The ACR-NEMA file contains a Signal, not an Image).
 */
int File::GetYSize()
{
   DataEntry *entry = GetDataEntry(0x0028,0x0010);
   if( entry )
      return (int)entry->GetValue(0);

   if ( IsDicomV3() )
   {
      return 0;
   }

   // The Rows (0028,0010) entry was optional for ACR/NEMA.
   // (at least some images didn't have it.)
   // It might hence be a signal (1D image). So we default to 1:
   return 1;
}

/**
 * \brief   Retrieve the number of planes of volume or the number
 *          of frames of a multiframe.
 * \warning When present we consider the "Number of Frames" as the third
 *          dimension. When missing we consider the third dimension as
 *          being the ACR-NEMA "Planes" tag content.
 * @return  The encountered size when found, 1 by default (single image).
 */
int File::GetZSize()
{
   // Both  DicomV3 and ACR/Nema consider the "Number of Frames"
   // as the third dimension.
   DataEntry *entry = GetDataEntry(0x0028,0x0008);
   if( entry )
      return (int)entry->GetValue(0);

   // We then consider the "Planes" entry as the third dimension 
   entry = GetDataEntry(0x0028,0x0012);
   if( entry )
      return (int)entry->GetValue(0);
   return 1;
}

// Special case:
//  ts["1.2.840.10008.5.1.4.1.1.4.1"] = "Enhanced MR Image Storage";
bool File::GetSpacing(float &xspacing, float &yspacing, float &zspacing)
{
      xspacing = yspacing = zspacing = 1.0;
      TS *ts = Global::GetTS();
      std::string sopclassuid_used;
      // D 0002|0002 [UI] [Media Storage SOP Class UID]
 
      //const std::string &mediastoragesopclassuid_str = GetEntryValue(0x0002,0x0002);  
      const std::string &mediastoragesopclassuid_str = GetEntryString(0x0002,0x0002);
      const std::string &mediastoragesopclassuid = ts->GetValue(mediastoragesopclassuid_str);
      //D 0008|0016 [UI] [SOP Class UID]
      const std::string &sopclassuid_str = GetEntryString(0x0008,0x0016);
      const std::string &sopclassuid = ts->GetValue(sopclassuid_str);
      if ( mediastoragesopclassuid == GDCM_UNFOUND && sopclassuid == GDCM_UNFOUND )
        {
        return false;
        }
      else
        {
        if ( mediastoragesopclassuid == sopclassuid )
          {
          sopclassuid_used = mediastoragesopclassuid;
          }
        else
          {
          gdcmWarningMacro( "Inconsistant SOP Class UID: "
            << mediastoragesopclassuid << " and " << sopclassuid );
          return false;
          }
        }
      // ok we have now the correct SOP Class UID
      if( sopclassuid_used == "Enhanced MR Image Storage" )
        {
        SeqEntry *PerframeFunctionalGroupsSequence = GetSeqEntry(0x5200,0x9230);
        unsigned int n = PerframeFunctionalGroupsSequence->GetNumberOfSQItems();
        if( !n ) return false;
        SQItem *item1 = PerframeFunctionalGroupsSequence->GetFirstSQItem();
        DocEntry *p = item1->GetDocEntry(0x0028,0x9110);
        if( !p ) return false;
        SeqEntry *seq = dynamic_cast<SeqEntry*>(p);
        unsigned int n1 = seq->GetNumberOfSQItems();
        if( !n1 ) return false;
        SQItem *item2 = seq->GetFirstSQItem();
        // D 0028|0030 [DS] [Pixel Spacing] [0.83333331346511\0.83333331346511 ]
        DocEntry *p2 = item2->GetDocEntry(0x0028,0x0030);
        if( !p2 ) return false;
        DataEntry *entry = dynamic_cast<DataEntry *>(p2);
        std::string spacing = entry->GetString();
        if ( sscanf( spacing.c_str(), "%f\\%f", &yspacing, &xspacing) != 2 )
          {
          xspacing = yspacing = 1.;
          return false;
          }
        // D 0018|0050 [DS] [Slice Thickness] [1 ]
        DocEntry *p3 = item2->GetDocEntry(0x0018,0x0050);
        if( !p3 ) return false;
        DataEntry *entry2 = dynamic_cast<DataEntry *>(p3);
        std::string thickness = entry2->GetString();
        if ( sscanf( thickness.c_str(), "%f", &zspacing) != 1 )
          {
          zspacing = 1.;
          return false;
          }
        return true;
        }
     return false;
   }

/**
 * \brief   Retrieve the -unnormalized- number of 'times' of '4D image'.
 *          User has to tell gdcm the location of this '4th Dimension component'
 *          using SetFourthDimensionLocation() method before.
 * \warning The defaulted value is 1.
 * @return  The encountered size when found, 1 by default 
 *          (The file doesn't contain a '4D image'.).
 */
int File::GetTSize()
{
   if (FourthDimensionLocation == TagKey(0,0) )// 4D location is not set : not a 4D object
      return 1;
      
   DataEntry *entry = GetDataEntry(FourthDimensionLocation.GetGroup(),
                                   FourthDimensionLocation.GetElement() );
   if( !entry )
   {
      gdcmWarningMacro( " FourthDimensionLocation not found at : " <<
                    std::hex << FourthDimensionLocation.GetGroup()
                  << "|" << FourthDimensionLocation.GetElement());
      return 1;
   }
   else
   {
      return (int)entry->GetValue(0);
   }      
}

/**
  * \brief gets the info from 0018,1164 : ImagerPixelSpacing
  *                      then 0028,0030 : Pixel Spacing
  *             else 1.0
  * @return X dimension of a pixel
  */
float File::GetXSpacing()
{
   float xspacing = 1.0;
   float yspacing = 1.0;
   float zspacing = 1.0;

   uint32_t nbValue;
   DataEntry *entry;
   bool ok = false; 
  if ( GetSpacing(xspacing,yspacing,zspacing) )
  {
    return xspacing;
  }
 // else fallback

/*
From:David Clunie - view profile
Date:Wed, May 24 2006 1:12 pm
Email:David Clunie <dclu...@dclunie.com>
Groups:comp.protocols.dicom

The short answer is that:

- (0018,1164) describes a spacing equivalent to that which
  would be measured off a film in projection radiography

- (0018,7022) does not describe the image pixels themselves,
  since detector elements may have been binned to produce
  pixels

- (0018,7020) may be different from (0018,7022) since there
  may be non-sensitive material separating individual
  detectors (i.e. the size is smaller than the spacing
  between centers)

Only (0018,1164) is relevant when measuring things; the
detector-specific attributes are there to describe the
acquisition.

David

PS. For ultrasound you need to use Region Calibration. 
*/
 
/*   
It *SHOULD* first find the IOD and then deduce which tags to read
Eg: Cross section this is in Pixel Spacing (0028,0030)
CR is in Imager Pixel Spacing (0018,1164)
US is in Pixel Aspect Ratio (0028,0034)
RT is in :
(3002,0011) Image Plane Pixel Spacing
(3002,0012) RT Image Position
and
(3004,000c) for deducing Z spacing 
*/

   std::string SOPClassUID = GetEntryString(0x0008,0x0016);

   /// \todo check the various SOP Class
   ///       to get the Pixel Spacing at the proper location
   
   ///\todo find images to check if it *actually* works    
           
   if (Util::DicomStringEqual( SOPClassUID,"1.2.840.10008.5.1.4.1.1.6")
   // Ultrasound Image Storage (Retired)
    || Util::DicomStringEqual( SOPClassUID,"1.2.840.10008.5.1.4.1.1.6.1")
   // Ultrasound Image Storage
    || Util::DicomStringEqual( SOPClassUID,"1.2.840.10008.5.1.4.1.1.3")
   // Ultrasound Multi-Frame Storage (Retired)
    || Util::DicomStringEqual( SOPClassUID,"1.2.840.10008.5.1.4.1.1.3.1") )
   // Ultrasound Multi-FrameImage Storage
   {
      // - check if  SOPClassUID contains 2 parts (e.g. "4\3")
      // - guess how to deduce the spacing (FOV ?, ??)
      
      entry = GetDataEntry(0x0028,0x0034);
      if ( entry )
      {
         nbValue = entry->GetValueCount();
         if( nbValue !=2 ) {
            gdcmWarningMacro("PixelAspectRatio (0x0028,0x0034) "
            << "has a wrong number of values :" << nbValue);
         }
         xspacing = 1.0; // We get Pixel Aspect Ratio, not Spacing ...
         ok = true;
      }
  
      if (ok)
         return xspacing;
   }
/*      
   if (Util::DicomStringEqual( SOPClassUID,"1.2.840.10008.5.1.4.1.1.1") ) 
   // Computed Radiography Image Storage   

   // CR is in Imager Pixel Spacing (0018,1164)//    

*/
   // go on with old method ...
   // ---------------------
   // To follow David Clunie's advice, we first check ImagerPixelSpacing

   entry = GetDataEntry(0x0018,0x1164);
   if( entry )
   {
      nbValue = entry->GetValueCount();
      // Can't use IsValueCountValid because of the complex heuristic.
      if( nbValue !=2 )
         gdcmWarningMacro("ImagerPixelSpacing (0x0018,0x1164) "
         << "has a wrong number of values :" << nbValue);

      if( nbValue >= 3 )
         xspacing = (float)entry->GetValue(2);
      else if( nbValue >= 2 )
         xspacing = (float)entry->GetValue(1);
      else
         xspacing = (float)entry->GetValue(0);

      if ( xspacing == 0.0 )
         xspacing = 1.0;
      return xspacing;
   }
   else
   {
      gdcmWarningMacro( "Unfound Imager Pixel Spacing (0018,1164)" );
   }

   entry = GetDataEntry(0x0028,0x0030);
   if( entry )
   {
      nbValue = entry->GetValueCount();
      if( nbValue !=2 )
         gdcmWarningMacro("PixelSpacing (0x0018,0x0030) "
          << "has a wrong number of values :" << nbValue);      
  
      if( nbValue >= 3 )
         xspacing = (float)entry->GetValue(2);
      else if( nbValue >= 2 )
         xspacing = (float)entry->GetValue(1);
      else
         xspacing = (float)entry->GetValue(0);

      if ( xspacing == 0.0 )
         xspacing = 1.0;
      return xspacing;
   }
   else
   {
      gdcmWarningMacro( "Unfound Pixel Spacing (0028,0030)" );
   }
   return xspacing;
}

/**
  * \brief gets the info from 0018,1164 : ImagerPixelSpacing
  *               then from   0028,0030 : Pixel Spacing                         
  *             else 1.0
  * @return Y dimension of a pixel
  */
float File::GetYSpacing()
{
   float xspacing = 1., yspacing = 1.0, zspacing = 1.;
   uint32_t nbValue;
   DataEntry *entry;
   bool ok = false;
  if ( GetSpacing(xspacing,yspacing,zspacing) )
  {
    return yspacing;
  }
 // else fallback

   std::string SOPClassUID = GetEntryString(0x0008,0x0016);

   /// \todo check the various SOP Class
   ///       to get the Pixel Spacing at the proper location
   
   ///\todo find images to check if it *actually* works       

   if (Util::DicomStringEqual( SOPClassUID,"1.2.840.10008.5.1.4.1.1.6")
   // Ultrasound Image Storage (Retired)
    || Util::DicomStringEqual( SOPClassUID,"1.2.840.10008.5.1.4.1.1.6.1")
   // Ultrasound Image Storage
    || Util::DicomStringEqual( SOPClassUID,"1.2.840.10008.5.1.4.1.1.3")
   // Ultrasound Multi-Frame Storage (Retired)
    || Util::DicomStringEqual( SOPClassUID,"1.2.840.10008.5.1.4.1.1.3.1") )
   // Ultrasound Multi-FrameImage Storage      
   {
      // - check if  SOPClassUID contains 2 parts (e.g. "4\3")
      // - no way to deduce the spacing/
      
      entry = GetDataEntry(0x0028,0x0034);
      if ( entry )
      {       
         nbValue = entry->GetValueCount();
         if( nbValue ==2 ) {
            yspacing = (float)entry->GetValue(0)/(float)entry->GetValue(1);
            //std::cout << "ys " << yspacing << std::endl;
            ok = true;
      }
      else
      {
         gdcmWarningMacro("PixelAspectRatio (0x0028,0x0034) "
               << "has a wrong number of values :" << nbValue);
         if (nbValue == 0 ) {
            ok = false;
         }
         else if (nbValue == 1 ) {
            yspacing = 1.0; // We get Pixel Aspect Ratio, not Spacing ...
            ok = true;
         } 
      }                  
   }
  
      if (ok)
         return yspacing;      
   }   

   // go on with old method ...
   // ---------------------
   // To follow David Clunie's advice, we first check ImagerPixelSpacing
   yspacing = 1.0;
   // To follow David Clunie's advice, we first check ImagerPixelSpacing

   entry = GetDataEntry(0x0018,0x1164);
   if( entry )
   {
      yspacing = (float)entry->GetValue(0);

      if ( yspacing == 0.0 )
         yspacing = 1.0;
      return yspacing;
   }
   else
   {
      gdcmWarningMacro( "Unfound Imager Pixel Spacing (0018,1164)" );
   }

   entry = GetDataEntry(0x0028,0x0030);
   if( entry )
   {
      yspacing = (float)entry->GetValue(0);

      if ( yspacing == 0.0 )
         yspacing = 1.0;
      return yspacing;
   }
   else
   {
      gdcmWarningMacro( "Unfound Pixel Spacing (0028,0030)" );
   }

   return yspacing;
} 

/**
 * \brief gets the info from 0018,0088 : Space Between Slices
 *                 else from 0018,0050 : Slice Thickness
 *                 else 1.0
 *
 * When an element is missing, we suppose slices join together
 * (no overlapping, no interslice gap) but we have no way to check it !
 * For *Dicom* images, ZSpacing *should be* calculated using 
 * XOrigin, YOrigin, ZOrigin (of the top left image corner)
 * of 2 consecutive images, and the Orientation
 * Computing ZSpacing on a single image is not really meaningfull ! 
 * @return Z dimension of a voxel-to be
 */
float File::GetZSpacing()
{

   float xspacing = 1.0;
   float yspacing = 1.0;
   float zspacing = 1.0;
   if ( GetSpacing(xspacing,yspacing,zspacing) )
   {
      return zspacing;
   }

   // Spacing Between Slices : distance between the middle of 2 slices
   // Slices may be :
   //   jointives     (Spacing between Slices = Slice Thickness)
   //   overlapping   (Spacing between Slices < Slice Thickness)
   //   disjointes    (Spacing between Slices > Slice Thickness)
   // Slice Thickness : epaisseur de tissus sur laquelle est acquis le signal
   //   It only concerns the MRI guys, not people wanting to visualize volumes
   //   If Spacing Between Slices is missing, 
   //   we suppose slices joint together
   DataEntry *entry = GetDataEntry(0x0018,0x0088);
   if( entry )
   {      zspacing = (float)entry->GetValue(0);

      if ( zspacing == 0.0 )
         zspacing = 1.0;
      return zspacing;
   }
   else
      gdcmWarningMacro("Unfound Spacing Between Slices (0018,0088)");

   // if no 'Spacing Between Slices' is found, 
   // we assume slices join together
   // (no overlapping, no interslice gap)
   entry = GetDataEntry(0x0018,0x0050);
   if( entry )
   {
      zspacing = (float)entry->GetValue(0);

      if ( zspacing == 0.0 )
         zspacing = 1.0;
      return zspacing;
   }
   else
      gdcmWarningMacro("Unfound Slice Thickness (0018,0050)");

   // if no 'Spacing Between Slices' is found, 
   // we assume slices join together
   // (no overlapping, no interslice gap)
   entry = GetDataEntry(0x3004,0x000c);
   if( entry )
   {
      float z1 = (float)entry->GetValue(0);
      float z2 = (float)entry->GetValue(1);
      zspacing = z2 - z1; // can be negative...

      if ( zspacing == 0.0 )
         zspacing = 1.0;
      return zspacing;
   }

   return zspacing;
}

/**
 * \brief gets the info from 0020,0032 : Image Position Patient
 *                 else from 0020,0030 : Image Position (RET)
 *                 else 0.
 * @return up-left image corner X position
 */
float File::GetXOrigin()
{
   DataEntry *entry = GetDataEntry(0x0020,0x0032);
   if( !entry )
   {
      gdcmWarningMacro( "Unfound Image Position Patient (0020,0032)");
      entry = GetDataEntry(0x0020,0x0030);
      if( !entry )
      {
         gdcmWarningMacro( "Unfound Image Position (RET) (0020,0030)");
         return 0.0f;
      }
   }

   if( entry->GetValueCount() == 3 )
   {
      if (!entry->IsValueCountValid() )
      {
         gdcmErrorMacro( "Invalid Value Count" );
      }
      return (float)entry->GetValue(0);
   }
   return 0.0f;
}

/**
 * \brief gets the info from 0020,0032 : Image Position Patient
 *                 else from 0020,0030 : Image Position (RET)
 *                 else 0.
 * @return up-left image corner Y position
 */
float File::GetYOrigin()
{
   DataEntry *entry = GetDataEntry(0x0020,0x0032);
   if( !entry )
   {
      gdcmWarningMacro( "Unfound Image Position Patient (0020,0032)");
      entry = GetDataEntry(0x0020,0x0030);
      if( !entry )
      {
         gdcmWarningMacro( "Unfound Image Position (RET) (0020,0030)");
         return 0.0f;
      }
   }

   if( entry->GetValueCount() == 3 )
   {
      if (!entry->IsValueCountValid() )
      {
         gdcmErrorMacro( "Invalid Value Count" );
      }
      return (float)entry->GetValue(1);
   }
   return 0.0f;
}

/**
 * \brief gets the info from 0020,0032 : Image Position Patient
 *                 else from 0020,0030 : Image Position (RET)
 *                 else from 0020,1041 : Slice Location
 *                 else from 0020,0050 : Location
 *                 else 0.
 * @return up-left image corner Z position
 */
float File::GetZOrigin()
{
   DataEntry *entry = GetDataEntry(0x0020,0x0032);
   if( entry )
   {
      if( entry->GetValueCount() == 3 )
      {
         if (!entry->IsValueCountValid() )
         {
            gdcmErrorMacro( "Invalid Value Count" );
         }
         return (float)entry->GetValue(2);
      }
      gdcmWarningMacro( "Wrong Image Position Patient (0020,0032)");
      return 0.0f;
   }

   entry = GetDataEntry(0x0020,0x0030);
   if( entry )
   {
      if( entry->GetValueCount() == 3 )
      {
         if (!entry->IsValueCountValid() )
         {
            gdcmErrorMacro( "Invalid Value Count" );
         }
         return (float)entry->GetValue(2);
      }
      gdcmWarningMacro( "Wrong Image Position (RET) (0020,0030)");
      return 0.0f;
   }

   // for *very* old ACR-NEMA images
   entry = GetDataEntry(0x0020,0x1041);
   if( entry )
   {
      if( entry->GetValueCount() == 1 )
      {
         if (!entry->IsValueCountValid() )
         {
            gdcmErrorMacro( "Invalid Value Count" );
         }
         return (float)entry->GetValue(0); // VM=1 !
      }
      gdcmWarningMacro( "Wrong Slice Location (0020,1041)");
      return 0.0f;
   }

   entry = GetDataEntry(0x0020,0x0050);
   if( entry )
   {
      if( entry->GetValueCount() == 1 )
      {
         if (!entry->IsValueCountValid() )
         {
            gdcmErrorMacro( "Invalid Value Count" );
         }
         return (float)entry->GetValue(0);
      }
      gdcmWarningMacro( "Wrong Location (0020,0050)");
      return 0.0f;
   }
   return 0.; // Hopeless
}

/**
  * \brief gets the info from 0020,0037 : Image Orientation Patient
  *                   or from 0020 0035 : Image Orientation (RET)
  *
  * (needed to organize DICOM files based on their x,y,z position)
  *
  * @param iop adress of the (6)float array to receive values.
  *        (defaulted as 1.,0.,0.,0.,1.,0. if nothing -or inconsistent stuff-
  *        is found.
  * @return true when one of the tag -with consistent values- is found
  *         false when nothing or inconsistent stuff - is found
  */
bool File::GetImageOrientationPatient( float iop[6] )
{
   std::string strImOriPat;
   //iop is supposed to be float[6]
   iop[0] = iop[4] = 1.;
   iop[1] = iop[2] = iop[3] = iop[5] = 0.;

   // 0020 0037 DS REL Image Orientation (Patient)
   if ( (strImOriPat = GetEntryString(0x0020,0x0037)) != GDCM_UNFOUND )
   {
      if ( sscanf( strImOriPat.c_str(), "%f \\ %f \\%f \\%f \\%f \\%f ", 
          &iop[0], &iop[1], &iop[2], &iop[3], &iop[4], &iop[5]) != 6 )
      {
         gdcmWarningMacro( "Wrong Image Orientation Patient (0020,0037)."
                        << " Less than 6 values were found." );
         return false;
      }
      return true;
   }
   //For ACR-NEMA
   // 0020 0035 DS REL Image Orientation (RET)
   else if ( (strImOriPat = GetEntryString(0x0020,0x0035)) != GDCM_UNFOUND )
   {
      if ( sscanf( strImOriPat.c_str(), "%f \\ %f \\%f \\%f \\%f \\%f ", 
          &iop[0], &iop[1], &iop[2], &iop[3], &iop[4], &iop[5]) != 6 )
      {
         gdcmWarningMacro( "wrong Image Orientation Patient (0020,0035). "
                        << "Less than 6 values were found." );
         return false;
      }
      return true;
   }
   return false;
}

/**
  * \brief gets the cosine of image X axis, against patient X axis
  *        (Sorry, but Python needs it :-( )
  * @return cosine of image X axis, against patient X axis 
  */
float File::GetXCosineOnX()
{  
   float iop[6];
   GetImageOrientationPatient( iop );
   return(iop[0]);
}
   
/**
  * \brief gets the cosine of image X axis, against patient Y axis
  *        (Sorry, but Python needs it :-( )
  * @return cosine of image X axis, against patient Y axis 
  */
float File::GetXCosineOnY()
{  
   float iop[6];
   GetImageOrientationPatient( iop );
   return(iop[1]);
}   

/**
  * \brief gets the cosine of image X axis, against patient Z axis
  *        (Sorry, but Python needs it :-( )
  * @return cosine of image X axis, against patient Z axis 
  */
float File::GetXCosineOnZ()
{  
   float iop[6];
   GetImageOrientationPatient( iop );
   return(iop[2]);
}   

/**
  * \brief gets the cosine of image Y axis, against patient X axis
  *        (Sorry, but Python needs it :-( )
  * @return cosine of image Y axis, against patient X axis 
  */
float File::GetYCosineOnX()
{  
   float iop[6];
   GetImageOrientationPatient( iop );
   return(iop[3]);
}
   
/**
  * \brief gets the cosine of image Y axis, against patient Y axis
  *        (Sorry, but Python needs it :-( )
  * @return cosine of image Y axis, against patient Y axis 
  */
float File::GetYCosineOnY()
{  
   float iop[6];
   GetImageOrientationPatient( iop );
   return(iop[4]);
}   

/**
  * \brief gets the cosine of image Y axis, against patient Z axis
  *        (Sorry, but Python needs it :-( )
  * @return cosine of image Y axis, against patient Z axis 
  */
float File::GetYCosineOnZ()
{  
   float iop[6];
   GetImageOrientationPatient( iop );
   return(iop[5]);
}    
/**
  * \brief gets the info from 0020,0032 : Image Position Patient
  *                   or from 0020 0030 : Image Position (RET)
  *
  * @param ipp adress of the (3)float array to receive values.
  *        (defaulted as 0.,0.,0. if nothing -or inconsistent stuff-
  *        is found.
  * @return true when one of the tag -with consistent values- is found
  *         false when nothing or inconsistent stuff - is found
  */
bool File::GetImagePositionPatient( float ipp[3] )
{
   std::string strImPosiPat;
   //ipp is supposed to be float[3]
   ipp[0] = ipp[1] = ipp[2] = 0.;

   // 0020 0032 DS REL Image Position (Patient)
   strImPosiPat = GetEntryString(0x0020,0x0032);
   if ( strImPosiPat != GDCM_UNFOUND )
   {
      if ( sscanf( strImPosiPat.c_str(), "%f \\ %f \\%f ", 
          &ipp[0], &ipp[1], &ipp[2]) != 3 )
      {
         gdcmWarningMacro( "Wrong Image Position Patient (0020,0032)."
                        << " Less than 3 values were found." );
         return false;
      }
      return true;
   }
   //For ACR-NEMA
   // 0020 0030 DS REL Image Position (RET)
   else if ( (strImPosiPat = GetEntryString(0x0020,0x0030)) != GDCM_UNFOUND )
   {
      if ( sscanf( strImPosiPat.c_str(), "%f \\ %f \\%f ", 
          &ipp[0], &ipp[1], &ipp[2]) != 3 )
      {
         gdcmWarningMacro( "wrong Image Position Patient (0020,0030). "
                        << "Less than 3 values were found." );
         return false;
      }
      return true;
   }
   return false;
}

/**
 * \brief   Retrieve the number of Bits Stored (actually used)
 *          (as opposed to number of Bits Allocated)
 * @return  The encountered number of Bits Stored, 0 by default.
 *          0 means the file is NOT USABLE. The caller has to check it !
 */
int File::GetBitsStored()
{
   DataEntry *entry = GetDataEntry(0x0028,0x0101);
   if( !entry )
   {
      gdcmWarningMacro("BitsStored (0028,0101) is supposed to be mandatory");
      return 0;
   }
   return (int)entry->GetValue(0);
}

/**
 * \brief   Retrieve the number of Bits Allocated
 *          (8, 12 -compacted ACR-NEMA files-, 16, 24 -old RGB ACR-NEMA files-,)
 * @return  The encountered Number of Bits Allocated, 0 by default.
 *          0 means the file is NOT USABLE. The caller has to check it !
 */
int File::GetBitsAllocated()
{
   DataEntry *entry = GetDataEntry(0x0028,0x0100);
   if( !entry )
   {
      gdcmWarningMacro("BitsAllocated (0028,0100) is supposed to be mandatory");
      return 0;
   }
   return (int)entry->GetValue(0);
}

/**
 * \brief   Retrieve the high bit position.
 * \warning The method defaults to 0 when information is missing.
 *          The responsability of checking this value is left to the caller.
 * @return  The high bit position when present. 0 when missing.
 */
int File::GetHighBitPosition()
{
   DataEntry *entry = GetDataEntry(0x0028,0x0102);
   if( !entry )
   {
      gdcmWarningMacro("HighBitPosition (0028,0102) is supposed to be mandatory");
      return 0;
   }
   return (int)entry->GetValue(0);
}

/**
 * \brief   Retrieve the number of Samples Per Pixel
 *          (1 : gray level, 3 : RGB/YBR -1 or 3 Planes-)
 * @return  The encountered number of Samples Per Pixel, 1 by default.
 *          (we assume Gray level Pixels)
 */
int File::GetSamplesPerPixel()
{
   DataEntry *entry = GetDataEntry(0x0028,0x0002);
   if( !entry )
   {
      gdcmWarningMacro("SamplesPerPixel (0028,0002) is supposed to be mandatory");
      return 1; // Well, it's supposed to be mandatory ...
                // but sometimes it's missing : *we* assume Gray pixels
   }
   return (int)entry->GetValue(0);
}

/**
 * \brief   Retrieve the Planar Configuration for RGB images
 *          (0 : RGB Pixels , 1 : R Plane + G Plane + B Plane)
 * @return  The encountered Planar Configuration, 0 by default.
 */
int File::GetPlanarConfiguration()
{
   DataEntry *entry = GetDataEntry(0x0028,0x0006);
   if( !entry )
   {
      return 0;
   }
   return (int)entry->GetValue(0);
}

/**
 * \brief   Return the size (in bytes) of a single pixel of data.
 * @return  The size in bytes of a single pixel of data; 0 by default
 *          0 means the file is NOT USABLE; the caller will have to check
 */
int File::GetPixelSize()
{
   // 0028 0100 US IMG Bits Allocated
   // (in order no to be messed up by old ACR-NEMA RGB images)
   assert( !(GetEntryString(0x0028,0x0100) == "24") );

   std::string pixelType = GetPixelType();
   if ( pixelType ==  "8U" || pixelType == "8S" )
   {
      return 1;
   }
   if ( pixelType == "16U" || pixelType == "16S")
   {
      return 2;
   }
   if ( pixelType == "32U" || pixelType == "32S")
   {
      return 4;
   }
   if ( pixelType == "FD" )
   {
      return 8;
   }
   gdcmWarningMacro( "Unknown pixel type: " << pixelType);
   return 0;
}

/**
 * \brief   Build the Pixel Type of the image.
 *          Possible values are:
 *          - 8U  unsigned  8 bit,
 *          - 8S    signed  8 bit,
 *          - 16U unsigned 16 bit,
 *          - 16S   signed 16 bit,
 *          - 32U unsigned 32 bit,
 *          - 32S   signed 32 bit,
 *          - FD floating double 64 bits (Not kosher DICOM, but so usefull!)
 * \warning 12 bit images appear as 16 bit.
 *          24 bit images appear as 8 bit + photochromatic interp ="RGB "
 *                                        + Planar Configuration = 0
 * @return  0S if nothing found. NOT USABLE file. The caller has to check
 */
std::string File::GetPixelType()
{
   std::string bitsAlloc = GetEntryString(0x0028, 0x0100); // Bits Allocated
   if ( bitsAlloc == GDCM_UNFOUND )
   {
      gdcmWarningMacro( "Bits Allocated (0028,0100) supposed to be mandatory");
      bitsAlloc = "16"; // default and arbitrary value, not to polute the output
   }

   else if ( bitsAlloc == "64" )
   {
      return "FD";
   }
     // useless since we have to bypass a bug ( >8 && < 16)
   else if ( bitsAlloc == "12" )
   {
      // It will be unpacked
      bitsAlloc = "16";
   }
   
   else if ( bitsAlloc == "24" )
   {
      // (in order no to be messed up by old RGB images)
      bitsAlloc = "8";
   }

   int i= atoi(bitsAlloc.c_str());  // fix a bug in some headers
   if ( i > 8 &&  i < 16 )
   {
      bitsAlloc = "16";
   }

   std::string sign;
   if( IsSignedPixelData() )
   {
      sign = "S";
   }
   else
   {
      sign = "U";
   }
   return bitsAlloc + sign;
}

/**
 * \brief   Check whether the pixels are signed (1) or UNsigned (0) data.
 * \warning The method defaults to false (UNsigned) when tag 0028|0103
 *          is missing.
 *          The responsability of checking this value is left to the caller
 *          (NO transformation is performed on the pixels to make then >0)
 * @return  True when signed, false when UNsigned
 */
bool File::IsSignedPixelData()
{
   DataEntry *entry = GetDataEntry(0x0028, 0x0103);//"Pixel Representation"
   if( !entry )
   {
      gdcmWarningMacro( "Pixel Representation (0028,0103) supposed to be "
                      << "mandatory");
      return false;
   }
   return entry->GetValue(0) != 0;
}

/**
 * \brief   Check whether this a monochrome picture (gray levels) or not,
 *          using "Photometric Interpretation" tag (0x0028,0x0004).
 * @return  true when "MONOCHROME1" or "MONOCHROME2". False otherwise.
 */
bool File::IsMonochrome()
{
   const std::string &PhotometricInterp = GetEntryString( 0x0028, 0x0004 );
   if (  Util::DicomStringEqual(PhotometricInterp, "MONOCHROME1")
      || Util::DicomStringEqual(PhotometricInterp, "MONOCHROME2") )
   {
      return true;
   }
   if ( PhotometricInterp == GDCM_UNFOUND )
   {
      gdcmWarningMacro( "Photometric Interpretation (0028,0004) supposed to be "
                         << "mandatory");
      // to deal with old ACR-NEMA images
      if (GetNumberOfScalarComponents() == 1)
         return true;
   }
   return false;
}

/**
 * \brief   Check whether this a MONOCHROME1 picture (high values = dark)
 *            or not using "Photometric Interpretation" tag (0x0028,0x0004).
 * @return  true when "MONOCHROME1" . False otherwise.
 */
bool File::IsMonochrome1()
{
   const std::string &PhotometricInterp = GetEntryString( 0x0028, 0x0004 );
   if (  Util::DicomStringEqual(PhotometricInterp, "MONOCHROME1") )
   {
      return true;
   }
   if ( PhotometricInterp == GDCM_UNFOUND )
   {
      gdcmWarningMacro( "Photometric Interpretation (0028,0004) : supposed to"
      << " be mandatory! ");
   }
   return false;
}

/**
 * \brief   Check whether this a "PALETTE COLOR" picture or not by accessing
 *          the "Photometric Interpretation" tag ( 0x0028, 0x0004 ).
 * @return  true when "PALETTE COLOR". False otherwise.
 */
bool File::IsPaletteColor()
{
   std::string PhotometricInterp = GetEntryString( 0x0028, 0x0004 );
   if (   PhotometricInterp == "PALETTE COLOR " )
   {
      return true;
   }
   
   // MONOCHROME + [Enhanced CT Image Storage] actually have Palettes
   std::string sopClassUid = GetEntryString( 0x0008, 0x0016 );
   if (Util::DicomStringEqual( sopClassUid, "1.2.840.10008.5.1.4.1.1.2.1"))
   {
      return true;
   }   
   if ( PhotometricInterp == GDCM_UNFOUND )
   {
      gdcmDebugMacro( "Not found : Palette color (0028,0004)");
   }
   return false;
}

/**
 * \brief   Check whether this a "YBR_FULL" color picture or not by accessing
 *          the "Photometric Interpretation" tag ( 0x0028, 0x0004 ).
 * @return  true when "YBR_FULL". False otherwise.
 */
bool File::IsYBRFull()
{
   std::string PhotometricInterp = GetEntryString( 0x0028, 0x0004 );   
   if (   PhotometricInterp == "YBR_FULL" )
   {
      return true;
   }
   if ( PhotometricInterp == GDCM_UNFOUND )
   {
      gdcmDebugMacro( "Not found : YBR Full (0028,0004)");
   }
   return false;
}

/**
  * \brief tells us if LUT are used
  * \warning Right now, 'Segmented xxx Palette Color Lookup Table Data'
  *          are NOT considered as LUT, since nobody knows
  *          how to deal with them
  *          Please warn me if you know sbdy that *does* know ... jprx
  * @return true if LUT Descriptors and LUT Tables were found 
  */
bool File::HasLUT()
{
   // Check the presence of the LUT Descriptors, and LUT Tables    
   // LutDescriptorRed    
   if ( !GetDocEntry(0x0028,0x1101) )
   {
      return false;
   }
   // LutDescriptorGreen 
   if ( !GetDocEntry(0x0028,0x1102) )
   {
      return false;
   }
   // LutDescriptorBlue 
   if ( !GetDocEntry(0x0028,0x1103) )
   {
      return false;
   }
   // Red Palette Color Lookup Table Data
   bool segmented;
   segmented = GetDocEntry(0x0028,0x1221) && 
     GetDocEntry(0x0028,0x1222) && 
     GetDocEntry(0x0028,0x1223);
   if( segmented ) return true;

   if( !GetDocEntry(0x0028,0x1201) )
   {
      return false;
   }
   // Green Palette Color Lookup Table Data       
   if ( !GetDocEntry(0x0028,0x1202) )
   {
      return false;
   }
   // Blue Palette Color Lookup Table Data      
   if ( !GetDocEntry(0x0028,0x1203) )
   {
      return false;
   }

   // FIXME : (0x0028,0x3006) : LUT Data (CTX dependent)
   //         NOT taken into account, but we don't know how to use it ...   
   return true;
}

/**
  * \brief gets the info from 0028,1101 : Lookup Table Desc-Red
  *             else 0
  * @return Lookup Table number of Bits , 0 by default
  *          when (0028,0004),Photometric Interpretation = [PALETTE COLOR ]
  * @ return bit number of each LUT item 
  */
int File::GetLUTNbits()
{
   std::vector<std::string> tokens;
   int lutNbits;

   //Just hope Lookup Table Desc-Red = Lookup Table Desc-Red
   //                                = Lookup Table Desc-Blue
   // Consistency already checked in GetLUTLength
   std::string lutDescription = GetEntryString(0x0028,0x1101);
   if ( lutDescription == GDCM_UNFOUND )
   {
      return 0;
   }

   tokens.clear(); // clean any previous value
   Util::Tokenize ( lutDescription, tokens, "\\" );
   //LutLength=atoi(tokens[0].c_str());
   //LutDepth=atoi(tokens[1].c_str());

   lutNbits = atoi( tokens[2].c_str() );
   tokens.clear();

   return lutNbits;
}

// Special case:
//  ts["1.2.840.10008.5.1.4.1.1.4.1"] = "Enhanced MR Image Storage";
   bool File::GetRescaleSlopeIntercept(double &slope, double &intercept)
   {
      slope = 1.0;
      intercept = 0.0;
      TS *ts = Global::GetTS();
      std::string sopclassuid_used;
      // D 0002|0002 [UI] [Media Storage SOP Class UID]
      const std::string &mediastoragesopclassuid_str = GetEntryString(0x0002,0x0002);
      const std::string &mediastoragesopclassuid = ts->GetValue(mediastoragesopclassuid_str);
      //D 0008|0016 [UI] [SOP Class UID]
      const std::string &sopclassuid_str = GetEntryString(0x0008,0x0016);
      const std::string &sopclassuid = ts->GetValue(sopclassuid_str);
      if ( mediastoragesopclassuid == GDCM_UNFOUND && sopclassuid == GDCM_UNFOUND )
        {
        return false;
        }
      else
        {
        if( mediastoragesopclassuid == sopclassuid )
          {
          sopclassuid_used = mediastoragesopclassuid;
          }
        else
          {
          gdcmWarningMacro( "Inconsistant SOP Class UID: "
            << mediastoragesopclassuid << " and " << sopclassuid );
          return false;
          }
        }
      // ok we have now the correc SOP Class UID
      if( sopclassuid_used == "Enhanced MR Image Storage" )
        {
        SeqEntry *PerframeFunctionalGroupsSequence = GetSeqEntry(0x5200,0x9230);
        unsigned int n = PerframeFunctionalGroupsSequence->GetNumberOfSQItems();
        if( !n ) return false;
        SQItem *item1 = PerframeFunctionalGroupsSequence->GetFirstSQItem();
        DocEntry *p = item1->GetDocEntry(0x0028,0x9145);
        if( !p ) return false;
        SeqEntry *seq = dynamic_cast<SeqEntry*>(p);
        unsigned int n1 = seq->GetNumberOfSQItems();
        if( !n1 ) return false;
        SQItem *item2 = seq->GetFirstSQItem();
        // D 0028|1052 [DS] [Rescale Intercept] [0 ]
        DocEntry *p2 = item2->GetDocEntry(0x0028,0x1052);
        if( !p2 ) return false;
        DataEntry *entry = dynamic_cast<DataEntry *>(p2);
        std::string intercept_str = entry->GetString();
        if ( sscanf( intercept_str.c_str(), "%lf", &intercept) != 1 )
          {
          intercept = 0.;
          return false;
          }
        // D 0028|1053 [DS] [Rescale Slope] [5.65470085470085]
        DocEntry *p3 = item2->GetDocEntry(0x0028,0x1053);
        if( !p3 ) return false;
        DataEntry *entry2 = dynamic_cast<DataEntry *>(p3);
        std::string slope_str = entry2->GetString();
        if ( sscanf( slope_str.c_str(), "%lf", &slope) != 1 )
          {
          slope = 1.;
          return false;
          }
        return true;
        }
     return false;
   }

/**
 *\brief gets the info from 0028,1052 : Rescale Intercept
 * @return Rescale Intercept. defaulted to 0.0 if not found or empty
 */
double File::GetRescaleIntercept()
{
   // 0028 1052 DS IMG Rescale Intercept
   DataEntry *entry = GetDataEntry(0x0028, 0x1052);
   if( !entry )
   {
      gdcmWarningMacro( "Missing Rescale Intercept (0028,1052)");
      return 0.0f;
   }
   return (float)entry->GetValue(0);

}

/**
 *\brief   gets the info from 0028,1053 : Rescale Slope
 * @return Rescale Slope. defaulted to 1.0 if not found or empty
 */
double File::GetRescaleSlope()
{
   double resInter = 0.;
   double resSlope = 1.;
   if ( GetRescaleSlopeIntercept(resSlope, resInter) )
     {
     return resSlope;
     }
   //0028 1053 DS IMG Rescale Slope
   std::string strRescSlope = GetEntryString(0x0028,0x1053);
   if ( strRescSlope != GDCM_UNFOUND )
   {
      if ( sscanf( strRescSlope.c_str(), "%lf ", &resSlope) != 1 )
      {
         // bug in the element 0x0028,0x1053
         gdcmWarningMacro( "Rescale Slope (0028,1053) is empty.");
      }
   }

   return resSlope;
}

/**
 * \brief This function is intended to user who doesn't want 
 *   to have to manage a LUT and expects to get an RBG Pixel image
 *   (or a monochrome one, if no LUT found ...) 
 * \warning to be used with GetImagePixels()
 * @return 1 if Gray level, 3 if Color (RGB, YBR, *or PALETTE COLOR*)
 */
int File::GetNumberOfScalarComponents()
{
   if ( GetSamplesPerPixel() == 3 )
   {
      return 3;
   }

   // 0028 0100 US IMG Bits Allocated
   // (in order no to be messed up by old RGB images)
   if ( GetEntryString(0x0028,0x0100) == "24" )
   {
      return 3;
   }

   std::string strPhotometricInterpretation = GetEntryString(0x0028,0x0004);

   if ( ( strPhotometricInterpretation == "PALETTE COLOR ") )
   {
      if ( HasLUT() )// PALETTE COLOR is NOT enough
      {
         return 3;
      }
      else
      {
         return 1;
      }
   }

   // beware of trailing space at end of string      
   // DICOM tags are never of odd length
   if ( strPhotometricInterpretation == GDCM_UNFOUND   || 
        Util::DicomStringEqual(strPhotometricInterpretation, "MONOCHROME1") ||
        Util::DicomStringEqual(strPhotometricInterpretation, "MONOCHROME2") )
   {
      return 1;
   }
   else
   {
      // we assume that *all* kinds of YBR are dealt with
      return 3;
   }
}

/**
 * \brief This function is intended to user that DOESN'T want 
 *  to get RGB pixels image when it's stored as a PALETTE COLOR image
 *   - the (vtk) user is supposed to know how deal with LUTs - 
 * \warning to be used with GetImagePixelsRaw()
 * @return 1 if Gray level, 3 if Color (RGB or YBR - NOT 'PALETTE COLOR' -)
 */
int File::GetNumberOfScalarComponentsRaw()
{
   // 0028 0100 US IMG Bits Allocated
   // (in order no to be messed up by old RGB images)
   if ( File::GetEntryString(0x0028,0x0100) == "24" )
   {
      return 3;
   }

   // we assume that *all* kinds of YBR are dealt with
   return GetSamplesPerPixel();
}

/**
 * \brief   Recover the offset (from the beginning of the file) 
 *          of *image* pixels (not *icone image* pixels, if any !)
 * @return Pixel Offset
 */
size_t File::GetPixelOffset()
{
   DocEntry *pxlElement = GetDocEntry(GrPixel, NumPixel);
   if ( pxlElement )
   {
      return pxlElement->GetOffset();
   }
   else
   {
      gdcmWarningMacro( "Big trouble : Pixel Element ("
                      << std::hex << GrPixel<<","<< NumPixel<< ") NOT found" );
      return 0;
   }
}

/**
 * \brief   Recover the pixel area length (in Bytes)
 * @return Pixel Element Length, as stored in the header
 *         (NOT the memory space necessary to hold the Pixels 
 *          -in case of embeded compressed image-)
 *         0 : NOT USABLE file. The caller has to check.
 */
size_t File::GetPixelAreaLength()
{
   DocEntry *pxlElement = GetDocEntry(GrPixel, NumPixel);
   if ( pxlElement )
   {
      return pxlElement->GetLength();
   }
   else
   {
      gdcmWarningMacro( "Big trouble : Pixel Element ("
                      << std::hex << GrPixel<<","<< NumPixel<< ") NOT found" );
      return 0;
   }
}

/**
 * \brief Adds the characteristics of a new element we want to anonymize
 * @param   group  Group number of the target tag.
 * @param   elem Element number of the target tag.
 * @param   value new value (string) to substitute with 
 */
void File::AddAnonymizeElement (uint16_t group, uint16_t elem, 
                                std::string const &value) 
{ 
   DicomElement el;
   el.Group = group;
   el.Elem  = elem;
   el.Value = value;
   UserAnonymizeList.push_back(el); 
}

/**
 * \brief Overwrites in the file the values of the DicomElements
 *       held in the list 
 */
void File::AnonymizeNoLoad()
{
   std::fstream *fp = new std::fstream(Filename.c_str(), 
                              std::ios::in | std::ios::out | std::ios::binary); 
   GDCM_NAME_SPACE::DocEntry *d;
   uint32_t offset;
   uint32_t lgth;
   uint32_t valLgth = 0;
   std::string *spaces;
   for (ListElements::iterator it = UserAnonymizeList.begin();  
                               it != UserAnonymizeList.end();
                             ++it)
   { 
   
      //std::cout << "File::AnonymizeNoLoad -------" << std::hex <<(*it).Group <<"|"<< 
      //         (*it).Elem 
      //         << "[" << (*it).Value << "] "<< std::dec << std::endl; 
      d = GetDocEntry( (*it).Group, (*it).Elem);

      if ( d == NULL)
         continue;

      if ( dynamic_cast<SeqEntry *>(d) )
      {
         gdcmWarningMacro( "You cannot 'Anonymize' a SeqEntry ");
         continue;
      }

      valLgth = (*it).Value.size();
      if (valLgth == 0)
         continue;

      offset = d->GetOffset();
      lgth =   d->GetLength();
      
      //std::cout << "lgth " << lgth << " valLgth " << valLgth << std::endl;
      if (valLgth < lgth)
      {
         spaces = new std::string( lgth-valLgth, ' ');
         (*it).Value = (*it).Value + *spaces;
         //std::cout << "[" << (*it).Value << "] " << lgth << std::endl;
         delete spaces;
      }
      fp->seekp( offset, std::ios::beg );
      fp->write( (*it).Value.c_str(), lgth );
     
   }
   fp->close();
   delete fp;
}

/**
 * \brief anonymize a File (remove Patient's personal info passed with
 *        AddAnonymizeElement()
 * \note You cannot Anonymize a DataEntry (to be fixed)
 */
bool File::AnonymizeFile()
{
   // If Anonymisation list is empty, let's perform some basic anonymization
   if ( UserAnonymizeList.begin() == UserAnonymizeList.end() )
   {
      // If exist, replace by spaces
      SetEntryString("  ",0x0010, 0x2154); // Telephone   
      SetEntryString("  ",0x0010, 0x1040); // Adress
      SetEntryString("  ",0x0010, 0x0020); // Patient ID

      DocEntry *patientNameHE = GetDocEntry (0x0010, 0x0010);
  
      if ( patientNameHE ) // we replace it by Study Instance UID (why not ?)
      {
         std::string studyInstanceUID =  GetEntryString (0x0020, 0x000d);
         if ( studyInstanceUID != GDCM_UNFOUND )
         {
            SetEntryString(studyInstanceUID, 0x0010, 0x0010);
         }
         else
         {
            SetEntryString("anonymized", 0x0010, 0x0010);
         }
      }
   }
   else
   {
      GDCM_NAME_SPACE::DocEntry *d;
      for (ListElements::iterator it = UserAnonymizeList.begin();  
                                  it != UserAnonymizeList.end();
                                ++it)
      {  
         d = GetDocEntry( (*it).Group, (*it).Elem);

         if ( d == NULL)
            continue;

         if ( dynamic_cast<SeqEntry *>(d) )
         {
            gdcmWarningMacro( "You cannot 'Anonymize' a SeqEntry ");
            continue;
         }
/*
         if ( dynamic_cast<DataEntry *>(d) )
         {
            gdcmWarningMacro( "To 'Anonymize' a DataEntry, better use AnonymizeNoLoad (FIXME) ");
            continue;
         }
*/
         else
            SetEntryString ((*it).Value, (*it).Group, (*it).Elem);
      }
}

  // In order to make definitively impossible any further identification
  // remove or replace all the stuff that contains a Date

//0008 0012 DA ID Instance Creation Date
//0008 0020 DA ID Study Date
//0008 0021 DA ID Series Date
//0008 0022 DA ID Acquisition Date
//0008 0023 DA ID Content Date
//0008 0024 DA ID Overlay Date
//0008 0025 DA ID Curve Date
//0008 002a DT ID Acquisition Datetime
//0018 9074 DT ACQ Frame Acquisition Datetime
//0018 9151 DT ACQ Frame Reference Datetime
//0018 a002 DT ACQ Contribution Date Time
//0020 3403 SH REL Modified Image Date (RET)
//0032 0032 DA SDY Study Verified Date
//0032 0034 DA SDY Study Read Date
//0032 1000 DA SDY Scheduled Study Start Date
//0032 1010 DA SDY Scheduled Study Stop Date
//0032 1040 DA SDY Study Arrival Date
//0032 1050 DA SDY Study Completion Date
//0038 001a DA VIS Scheduled Admission Date
//0038 001c DA VIS Scheduled Discharge Date
//0038 0020 DA VIS Admitting Date
//0038 0030 DA VIS Discharge Date
//0040 0002 DA PRC Scheduled Procedure Step Start Date
//0040 0004 DA PRC Scheduled Procedure Step End Date
//0040 0244 DA PRC Performed Procedure Step Start Date
//0040 0250 DA PRC Performed Procedure Step End Date
//0040 2004 DA PRC Issue Date of Imaging Service Request
//0040 4005 DT PRC Scheduled Procedure Step Start Date and Time
//0040 4011 DT PRC Expected Completion Date and Time
//0040 a030 DT PRC Verification Date Time
//0040 a032 DT PRC Observation Date Time
//0040 a120 DT PRC DateTime
//0040 a121 DA PRC Date
//0040 a13a DT PRC Referenced Datetime
//0070 0082 DA ??? Presentation Creation Date
//0100 0420 DT ??? SOP Autorization Date and Time
//0400 0105 DT ??? Digital Signature DateTime
//2100 0040 DA PJ Creation Date
//3006 0008 DA SSET Structure Set Date
//3008 0024 DA ??? Treatment Control Point Date
//3008 0054 DA ??? First Treatment Date
//3008 0056 DA ??? Most Recent Treatment Date
//3008 0162 DA ??? Safe Position Exit Date
//3008 0166 DA ??? Safe Position Return Date
//3008 0250 DA ??? Treatment Date
//300a 0006 DA RT RT Plan Date
//300a 022c DA RT Air Kerma Rate Reference Date
//300e 0004 DA RT Review Date

   return true;
}

/**
 * \brief Performs some consistency checking on various 'File related' 
 *       (as opposed to 'DicomDir related') entries 
 *       then writes in a file all the (Dicom Elements) included the Pixels 
 * @param fileName file name to write to
 * @param writetype type of the file to be written 
 *          (ACR, ExplicitVR, ImplicitVR)
 */
bool File::Write(std::string fileName, FileType writetype)
{
      gdcmDebugMacro(" File::Write ");
   std::ofstream *fp = new std::ofstream(fileName.c_str(), 
                                         std::ios::out | std::ios::binary);
   if (*fp == NULL)
   {
      gdcmWarningMacro("Failed to open (write) File: " << fileName.c_str());
      return false;
   }

   // Entry : 0002|0000 = group length -> recalculated
   DataEntry *e0000 = GetDataEntry(0x0002,0x0000);
   if ( e0000 )
   {
      std::ostringstream sLen;
      sLen << ComputeGroup0002Length( );
      e0000->SetString(sLen.str());
   }

   /// \todo FIXME : Derma?.dcm does not have it...let's remove it ?!? JPRx
   if( writetype != JPEG && writetype != JPEG2000)
   {
      int i_lgPix = GetEntryLength(GrPixel, NumPixel);
      if (i_lgPix != -2)  /// \todo wtf "-2" ?!?
      {
         // no (GrPixel, NumPixel) element
         std::string s_lgPix = Util::Format("%d", i_lgPix+12);
         s_lgPix = Util::DicomString( s_lgPix.c_str() );
         InsertEntryString(s_lgPix,GrPixel, 0x0000, "UL");
      }
   }
   Document::WriteContent(fp, writetype,false,false);

   fp->close();
   delete fp;

   return true;
}

//-----------------------------------------------------------------------------
// Protected


//-----------------------------------------------------------------------------
// Private
/**
 * \brief Parse pixel data from disk of [multi-]fragment RLE encoding.
 *        Compute the RLE extra information and store it in RLEInfo
 *        for later pixel retrieval usage.
 */
void File::ComputeRLEInfo()
{
   std::string ts = GetTransferSyntax();
   if ( !Global::GetTS()->IsRLELossless(ts) ) 
   {
      return;
   }

   // Encoded pixel data: for the time being we are only concerned with
   // Jpeg or RLE Pixel data encodings.
   // As stated in PS 3.5-2003, section 8.2 p44:
   // "If sent in Encapsulated Format (i.e. other than the Native Format) the
   //  value representation OB is used".
   // Hence we expect an OB value representation. Concerning OB VR,
   // the section PS 3.5-2003, section A.4.c p 58-59, states:
   // "For the Value Representations OB and OW, the encoding shall meet the
   //   following specifications depending on the Data element tag:"
   //   [...snip...]
   //    - the first item in the sequence of items before the encoded pixel
   //      data stream shall be basic offset table item. The basic offset table
   //      item value, however, is not required to be present"
   ReadEncapsulatedBasicOffsetTable();

   // Encapsulated RLE Compressed Images (see PS 3.5-2003, Annex G)
   // Loop on the individual frame[s] and store the information
   // on the RLE fragments in a RLEFramesInfo.
   // Note: - when only a single frame is present, this is a
   //         classical image.
   //       - when more than one frame are present, then we are in 
   //         the case of a multi-frame image.
   long frameLength;
   int i=0;
   uint32_t sum = 0;
   while ( (frameLength = ReadTagLength(0xfffe, 0xe000)) != 0 )
   { 
      // Since we have read the basic offset table, let's check the value were correct
      // or else produce a warning:
      if ( BasicOffsetTableItemValue )
        {
        // If a BasicOffsetTableItemValue was read
        uint32_t individualLength = BasicOffsetTableItemValue[i];
        assert( individualLength == sum ); // REMOVE that if this is a problem
        if( individualLength != sum )
          {
          gdcmWarningMacro( "BasicOffsetTableItemValue differs from the fragment lenght" );
          }
        sum += frameLength + 8;
        i++;
        }
      // Parse the RLE Header and store the corresponding RLE Segment
      // Offset Table information on fragments of this current Frame.
      // Note that the fragment pixels themselves are not loaded
      // (but just skipped).
      long frameOffset = Fp->tellg(); // once per fragment

      uint32_t nbRleSegments = ReadInt32();
      if ( nbRleSegments > 16 )
      {
         // There should be at most 15 segments (refer to RLEFrame class)
         gdcmWarningMacro( "Too many segments.");
      }
 
      uint32_t rleSegmentOffsetTable[16];
      for( int k = 1; k <= 15; k++ )
      {
         rleSegmentOffsetTable[k] = ReadInt32();
      }

      // Deduce from both RLE Header and frameLength 
      // the fragment length, and again store this info
      // in a RLEFramesInfo.
      long rleSegmentLength[15];
      // skipping (not reading) RLE Segments
      if ( nbRleSegments > 1)
      {
         for(unsigned int k = 1; k <= nbRleSegments-1; k++)
         {
             rleSegmentLength[k] =  rleSegmentOffsetTable[k+1]
                                  - rleSegmentOffsetTable[k];
             SkipBytes(rleSegmentLength[k]);
          }
       }

       rleSegmentLength[nbRleSegments] = frameLength 
                                      - rleSegmentOffsetTable[nbRleSegments];
       SkipBytes(rleSegmentLength[nbRleSegments]);

       // Store the collected info
       RLEFrame *newFrame = new RLEFrame;
       newFrame->SetNumberOfFragments(nbRleSegments);
       for( unsigned int uk = 1; uk <= nbRleSegments; uk++ )
       {
          newFrame->SetOffset(uk,frameOffset + rleSegmentOffsetTable[uk]);
          newFrame->SetLength(uk,rleSegmentLength[uk]);
       }
       RLEInfo->AddFrame(newFrame);
   }

   // Make sure that  we encounter a 'Sequence Delimiter Item'
   // at the end of the item :
   if ( !ReadTag(0xfffe, 0xe0dd) ) // once per RLE File
   {
      gdcmWarningMacro( "No sequence delimiter item at end of RLE item sequence");
   }
}

/**
 * \brief Parse pixel data from disk of [multi-]fragment Jpeg encoding.
 *        Compute the jpeg extra information (fragment[s] offset[s] and
 *        length) and store it[them] in JPEGInfo for later pixel
 *        retrieval usage.
 */
void File::ComputeJPEGFragmentInfo()
{
   // If you need to, look for comments of ComputeRLEInfo().
   std::string ts = GetTransferSyntax();
   if ( ! Global::GetTS()->IsJPEG(ts) )
   {
      return;
   }

   ReadEncapsulatedBasicOffsetTable();

   // Loop on the fragments[s] and store the parsed information in a
   // JPEGInfo.
   long fragmentLength;
   int i=0;
   uint32_t sum = 0;
   while ( (fragmentLength = ReadTagLength(0xfffe, 0xe000)) != 0 )
   { 
      // Since we have read the basic offset table, let's check the value were correct
      // or else produce a warning:
      // A.4 Transfer syntaxes for encapsulation of encoded pixel data:
      // When the Item Value is present, the Basic Offset Table Item Value shall contain
      // concatenated 32-bit unsigned integer values that are byte offsets to the first
      // byte of the Item Tag of the first fragment for each frame in the Sequence of
      // Items. These offsets are measured from the first byte of the first Item Tag
      // following the Basic Offset Table item (See Table A.4-2).

      if ( BasicOffsetTableItemValue )
        {
        // If a BasicOffsetTableItemValue was read
        uint32_t individualLength = BasicOffsetTableItemValue[i];
        //assert( individualLength == sum ); // Seems like 00191113.dcm is off by one ??
        if( individualLength != sum )
          {
          gdcmWarningMacro( "BasicOffsetTableItemValue differs from the fragment lenght:" <<
              individualLength << " != " << sum );
          }
        sum += fragmentLength + 8;
        i++;
        }

      long fragmentOffset = Fp->tellg(); // Once per fragment
      // Store the collected info
      JPEGFragment *newFragment = new JPEGFragment;
      newFragment->SetOffset(fragmentOffset);
      newFragment->SetLength(fragmentLength);
      JPEGInfo->AddFragment(newFragment);

      SkipBytes(fragmentLength);
   }

   // Make sure that  we encounter a 'Sequence Delimiter Item'
   // at the end of the item :
   if ( !ReadTag(0xfffe, 0xe0dd) )
   {
      gdcmWarningMacro( "No sequence delimiter item at end of JPEG item sequence");
   }
}

/**
 * \brief   Assuming the internal file pointer Document::Fp 
 *          is placed at the beginning of a tag, check whether this
 *          tag is (TestGroup, TestElem).
 * \warning On success the internal file pointer Document::Fp
 *          is modified to point after the tag.
 *          On failure (i.e. when the tag wasn't the expected tag
 *          (TestGroup, TestElem) the internal file pointer
 *          Document::Fp is restored to its original position.
 * @param   testGroup The expected group   of the tag.
 * @param   testElem  The expected Element of the tag.
 * @return  True on success, false otherwise.
 */
bool File::ReadTag(uint16_t testGroup, uint16_t testElem)
{
   long positionOnEntry = Fp->tellg(); // Only when reading fragments
   //long currentPosition = positionOnEntry;      // On debugging purposes

   // Read the Item Tag group and element, and make
   // sure they are what we expected:
   uint16_t itemTagGroup;
   uint16_t itemTagElem;
   try
   {
      itemTagGroup = ReadInt16();
      itemTagElem  = ReadInt16();
   }
   catch ( FormatError )
   {
      gdcmErrorMacro( "Can not read tag at 0x(" 
       << std::hex << positionOnEntry
       << ").  We should have found tag ("
       << DictEntry::TranslateToKey(testGroup,testElem) << ")"
       ) ;

      return false;
   }
   if ( itemTagGroup != testGroup || itemTagElem != testElem )
   {
       // in order not to pollute output we don't warn on 'delimitors'
      if (itemTagGroup != 0xfffe ||  testGroup != 0xfffe )
         gdcmWarningMacro( "Wrong Item Tag found:"
          << "   We should have found tag ("
          << DictEntry::TranslateToKey(testGroup,testElem) << ")" << std::endl
          << "   but instead we encountered tag ("
          << DictEntry::TranslateToKey(itemTagGroup,itemTagElem) << ")"
          << "  at address: " << "  0x(" << std::hex 
          << (unsigned int)positionOnEntry  << std::dec << ")"
          ) ;
      Fp->seekg(positionOnEntry, std::ios::beg);

      return false;
   }
   return true;
}

/**
 * \brief   Assuming the internal file pointer Document::Fp 
 *          is placed at the beginning of a tag (TestGroup, TestElement),
 *          read the length associated to the Tag.
 * \warning On success the internal file pointer Document::Fp
 *          is modified to point after the tag and its length.
 *          On failure (i.e. when the tag wasn't the expected tag
 *          (TestGroup, TestElement) the internal file pointer
 *          Document::Fp is restored to its original position.
 * @param   testGroup The expected Group   of the tag.
 * @param   testElem  The expected Element of the tag.
 * @return  On success returns the length associated to the tag. On failure
 *          returns 0.
 */
uint32_t File::ReadTagLength(uint16_t testGroup, uint16_t testElem)
{

   if ( !ReadTag(testGroup, testElem) )
   {
      // Avoid polutting output
      if ( testGroup != 0xfffe ) 
         gdcmErrorMacro( "ReadTag did not succeed for ("
                    << DictEntry::TranslateToKey(testGroup,testElem) 
                    << ")..." );
      return 0;
   }
                                                                                
   //// Then read the associated Item Length

   // long currentPosition = Fp->tellg(); // save time // JPRx
   uint32_t itemLength  = ReadInt32();
   gdcmDebugMacro( "Basic Item Length is: " << itemLength 
//        << "  at address: " << std::hex << (unsigned int)currentPosition
   );
   return itemLength;
}

/**
 * \brief When parsing the Pixel Data of an encapsulated file, read
 *        the basic offset table (when present, and BTW dump it).
 */
void File::ReadEncapsulatedBasicOffsetTable()
{
   //// Read the Basic Offset Table Item Tag length...
   uint32_t itemLength = ReadTagLength(0xfffe, 0xe000);

   // When present, read the basic offset table itself.
   // Notes: - since the presence of this basic offset table is optional
   //          we can't rely on it for the implementation, and we will simply
   //          trash its content (when present).
   //        - still, when present, we could add some further checks on the
   //          lengths, but we won't bother with such fuses for the time being.
   if ( itemLength != 0 )
   {
      char *charBasicOffsetTableItemValue = new char[itemLength];
      Fp->read(charBasicOffsetTableItemValue, itemLength);
      unsigned int nbEntries = itemLength/4;
      assert( nbEntries*4 == itemLength); // Make sure this is a multiple
      BasicOffsetTableItemValue = new uint32_t[nbEntries];

      for (unsigned int i=0; i < nbEntries; i++ )
      {
         BasicOffsetTableItemValue[i] = *((uint32_t*)(&charBasicOffsetTableItemValue[4*i]));
#if defined(GDCM_WORDS_BIGENDIAN) || defined(GDCM_FORCE_BIGENDIAN_EMULATION)
         uint32_t val = BasicOffsetTableItemValue[i];
         BasicOffsetTableItemValue[i] 
           = (  (val<<24)               | ((val<<8)  & 0x00ff0000) | 
              ( (val>>8)  & 0x0000ff00) |  (val>>24)               );
#endif
         gdcmDebugMacro( "Read one length for: " << 
                          std::hex << BasicOffsetTableItemValue[i] );
      }

      delete[] charBasicOffsetTableItemValue;
   }
}

// These are the deprecated method that one day should be removed (after the next release)

//#ifndef GDCM_LEGACY_REMOVE
/*
 * \ brief   Loader. (DEPRECATED :  temporaryly kept not to break the API)
 * @ param   fileName file to be open for parsing
 * @ return false if file cannot be open or no swap info was found,
 *         or no tag was found.
 * @deprecated Use the Load() [ + SetLoadMode() ] + SetFileName() functions instead
 */
 /*
bool File::Load( std::string const &fileName ) 
{
   GDCM_LEGACY_REPLACED_BODY(File::Load(std::string), "1.2",
                             File::Load());
   SetFileName( fileName );
   if ( ! this->Document::Load( ) )
      return false;

   return DoTheLoadingJob( );
}
#endif
*/
//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
} // end namespace gdcm
