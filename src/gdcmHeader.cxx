/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmHeader.cxx,v $
  Language:  C++
  Date:      $Date: 2004/07/17 22:45:40 $
  Version:   $Revision: 1.177 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmHeader.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"
#include "gdcmTS.h"
#include "gdcmValEntry.h"

#include <vector>

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief  Constructor 
 * @param  filename name of the file whose header we want to analyze
 * @param  exception_on_error whether we want to throw an exception or not
 * @param  enable_sequences = true to allow the header 
 *         to be parsed *inside* the SeQuences, when they have an actual length 
 * @param  ignore_shadow = true if user wants to skip shadow groups 
 *         during parsing, to save memory space
 */
gdcmHeader::gdcmHeader( std::string const & filename, 
                        bool exception_on_error,
                        bool enable_sequences, 
                        bool ignore_shadow ):
            gdcmDocument( filename,
                          exception_on_error,
                          enable_sequences,
                          ignore_shadow )
{    
   // for some ACR-NEMA images GrPixel, NumPixel is *not* 7fe0,0010
   // We may encounter the 'RETired' (0x0028, 0x0200) tag
   // (Image Location") . This Element contains the number of
   // the group that contains the pixel data (hence the "Pixel Data"
   // is found by indirection through the "Image Location").
   // Inside the group pointed by "Image Location" the searched element
   // is conventionally the element 0x0010 (when the norm is respected).
   // When the "Image Location" is absent we default to group 0x7fe0.
   
   // This IS the right place for the code
 
   // Image Location
   std::string imgLocation = GetEntryByNumber(0x0028, 0x0200);
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
   // the supposed processor endianity. 
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
}

/**
 * \brief Constructor  
 * @param exception_on_error whether we want to throw an exception or not
 */
gdcmHeader::gdcmHeader(bool exception_on_error) :
            gdcmDocument( exception_on_error )
{
}

/**
 * \ingroup gdcmHeader
 * \brief   Canonical destructor.
 */
gdcmHeader::~gdcmHeader ()
{
}


/**
 * \brief Performs some consistency checking on various 'File related' 
 *       (as opposed to 'DicomDir related') entries 
 *       then writes in a file all the (Dicom Elements) included the Pixels 
 * @param fp file pointer on an already open file
 * @param filetype Type of the File to be written 
 *          (ACR-NEMA, ExplicitVR, ImplicitVR)
 */
void gdcmHeader::Write(FILE* fp,FileType filetype) {
   
   // Bits Allocated
   if ( GetEntryByNumber(0x0028,0x0100) ==  "12") {
      SetEntryByNumber("16", 0x0028,0x0100);
   }

  // correct Pixel group Length if necessary

   // TODO : create a gdcmHeader::Write method and move this part.
   //        (only gdcmHeader knows GrPixel, NumPixel)

   int i_lgPix = GetEntryLengthByNumber(GrPixel, NumPixel);
   if (i_lgPix != -2) { // no (GrPixel, NumPixel) element
      char * dumm = new char[20];
      sprintf(dumm ,"%d", i_lgPix+12);
      std::string s_lgPix = dumm;
      delete dumm;
      ReplaceOrCreateByNumber(s_lgPix,GrPixel, 0x0000);
   }
  
   // Drop Palette Color, if necessary
   
   if ( GetEntryByNumber(0x0028,0x0002).c_str()[0] == '3' ) {
    // if SamplesPerPixel = 3, sure we don't need any LUT !   
    // Drop 0028|1101, 0028|1102, 0028|1103
    // Drop 0028|1201, 0028|1202, 0028|1203

     gdcmDocEntry *e;
     e=GetDocEntryByNumber(0x0028,0x01101);
     if (e) 
        RemoveEntry(e);
     e=GetDocEntryByNumber(0x0028,0x1102);
     if (e) 
        RemoveEntry(e);
     e=GetDocEntryByNumber(0x0028,0x1103);

     if (e) 
        RemoveEntry(e);
     e=GetDocEntryByNumber(0x0028,0x01201);
     if (e) 
        RemoveEntry(e);
     e=GetDocEntryByNumber(0x0028,0x1202);
     if (e) 
        RemoveEntry(e);
     e=GetDocEntryByNumber(0x0028,0x1203);
     if (e) 
       RemoveEntry(e);
   }

   gdcmDocument::Write(fp,filetype);
}

//-----------------------------------------------------------------------------
// Print


//-----------------------------------------------------------------------------
// Public

/**
 * \brief  This predicate, based on hopefully reasonable heuristics,
 *         decides whether or not the current gdcmHeader was properly parsed
 *         and contains the mandatory information for being considered as
 *         a well formed and usable Dicom/Acr File.
 * @return true when gdcmHeader is the one of a reasonable Dicom/Acr file,
 *         false otherwise. 
 */
bool gdcmHeader::IsReadable()
{
   if( !gdcmDocument::IsReadable() )
   {
      return false;
   }

   std::string res = GetEntryByNumber(0x0028, 0x0005);
   if ( res != GDCM_UNFOUND && atoi(res.c_str()) > 4 )
   {
      return false; // Image Dimensions
   }
   if ( !GetDocEntryByNumber(0x0028, 0x0100) )
   {
      return false; // "Bits Allocated"
   }
   if ( !GetDocEntryByNumber(0x0028, 0x0101) )
   {
      return false; // "Bits Stored"
   }
   if ( !GetDocEntryByNumber(0x0028, 0x0102) )
   {
      return false; // "High Bit"
   }
   if ( !GetDocEntryByNumber(0x0028, 0x0103) )
   {
      return false; // "Pixel Representation" i.e. 'Sign'
   }

   return true;
}

/**
 * \brief   Retrieve the number of columns of image.
 * @return  The encountered size when found, 0 by default.
 *          0 means the file is NOT USABLE. The caller will have to check
 */
int gdcmHeader::GetXSize()
{
   std::string strSize;
   strSize = GetEntryByNumber(0x0028,0x0011);
   if ( strSize == GDCM_UNFOUND )
   {
      return 0;
   }

   return atoi( strSize.c_str() );
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of lines of image.
 * \warning The defaulted value is 1 as opposed to gdcmHeader::GetXSize()
 * @return  The encountered size when found, 1 by default 
 *          (The ACR-NEMA file contains a Signal, not an Image).
 */
int gdcmHeader::GetYSize()
{
   std::string strSize = GetEntryByNumber(0x0028,0x0010);
   if ( strSize != GDCM_UNFOUND )
   {
      return atoi( strSize.c_str() );
   }
   if ( IsDicomV3() )
   {
      return 0;
   }

   // The Rows (0028,0010) entry was optional for ACR/NEMA. It might
   // hence be a signal (1d image). So we default to 1:
   return 1;
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of planes of volume or the number
 *          of frames of a multiframe.
 * \warning When present we consider the "Number of Frames" as the third
 *          dimension. When absent we consider the third dimension as
 *          being the ACR-NEMA "Planes" tag content.
 * @return  The encountered size when found, 1 by default (single image).
 */
int gdcmHeader::GetZSize()
{
   // Both  DicomV3 and ACR/Nema consider the "Number of Frames"
   // as the third dimension.
   std::string strSize = GetEntryByNumber(0x0028,0x0008);
   if ( strSize != GDCM_UNFOUND )
   {
      return atoi( strSize.c_str() );
   }

   // We then consider the "Planes" entry as the third dimension 
   strSize = GetEntryByNumber(0x0028,0x0012);
   if ( strSize != GDCM_UNFOUND )
   {
      return atoi( strSize.c_str() );
   }

   return 1;
}

/**
  * \ingroup gdcmHeader
  * \brief gets the info from 0028,0030 : Pixel Spacing
  *             else 1.0
  * @return X dimension of a pixel
  */
float gdcmHeader::GetXSpacing()
{
   float xspacing, yspacing;
   std::string strSpacing = GetEntryByNumber(0x0028,0x0030);

   if ( strSpacing == GDCM_UNFOUND )
   {
      dbg.Verbose(0, "gdcmHeader::GetXSpacing: unfound Pixel Spacing (0028,0030)");
      return 1.;
   }

   int nbValues;
   if( ( nbValues = sscanf( strSpacing.c_str(), 
         "%f\\%f", &yspacing, &xspacing)) != 2 )
   {
      // if single value is found, xspacing is defaulted to yspacing
      if ( nbValues == 1 )
      {
         return yspacing;
      }
   }
   if ( xspacing == 0.)
   {
      dbg.Verbose(0, "gdcmHeader::GetYSpacing: gdcmData/CT-MONO2-8-abdo.dcm problem");
      // seems to be a bug in the header ...
      sscanf( strSpacing.c_str(), "%f\\0\\%f", &yspacing, &xspacing);
   }

   return xspacing;
}

/**
  * \ingroup gdcmHeader
  * \brief gets the info from 0028,0030 : Pixel Spacing
  *             else 1.0
  * @return Y dimension of a pixel
  */
float gdcmHeader::GetYSpacing()
{
   float yspacing = 0;
   std::string strSpacing = GetEntryByNumber(0x0028,0x0030);
  
   if ( strSpacing == GDCM_UNFOUND )
   {
      dbg.Verbose(0, "gdcmHeader::GetYSpacing: unfound Pixel Spacing (0028,0030)");
      return 1.;
    }

   // if sscanf cannot read any float value, it won't affect yspacing
   sscanf( strSpacing.c_str(), "%f", &yspacing);

   return yspacing;
} 

/**
  *\ingroup gdcmHeader
  *\brief gets the info from 0018,0088 : Space Between Slices
  *                else from 0018,0050 : Slice Thickness
   *                else 1.0
  * @return Z dimension of a voxel-to be
  */
float gdcmHeader::GetZSpacing()
{
   // Spacing Between Slices : distance entre le milieu de chaque coupe
   // Les coupes peuvent etre :
   //   jointives     (Spacing between Slices = Slice Thickness)
   //   chevauchantes (Spacing between Slices < Slice Thickness)
   //   disjointes    (Spacing between Slices > Slice Thickness)
   // Slice Thickness : epaisseur de tissus sur laquelle est acquis le signal
   //   ca interesse le physicien de l'IRM, pas le visualisateur de volumes ...
   //   Si le Spacing Between Slices est absent, 
   //   on suppose que les coupes sont jointives
   
   std::string strSpacingBSlices = GetEntryByNumber(0x0018,0x0088);

   if ( strSpacingBSlices == GDCM_UNFOUND )
   {
      dbg.Verbose(0, "gdcmHeader::GetZSpacing: unfound StrSpacingBSlices");
      std::string strSliceThickness = GetEntryByNumber(0x0018,0x0050);       
      if ( strSliceThickness == GDCM_UNFOUND )
      {
         return 1.;
      }
      else
      {
         // if no 'Spacing Between Slices' is found, 
         // we assume slices join together
         // (no overlapping, no interslice gap)
         // if they don't, we're fucked up
         return atof( strSliceThickness.c_str() );
      }
   }
   else
   {
      return atof( strSpacingBSlices.c_str() );
   }
}

/**
  *\ingroup gdcmHeader
  *\brief gets the info from 0028,1052 : Rescale Intercept
  * @return Rescale Intercept
 */
float gdcmHeader::GetRescaleIntercept()
{
   float resInter = 0.;
   std::string strRescInter = GetEntryByNumber(0x0028,0x1052); //0028 1052 DS IMG Rescale Intercept
   if ( strRescInter != GDCM_UNFOUND )
   {
      if( sscanf( strRescInter.c_str(), "%f", &resInter) != 1 )
      {
         // bug in the element 0x0028,0x1052
         dbg.Verbose(0, "gdcmHeader::GetRescaleIntercept: Rescale Slope is empty");
      }
   }

  return resInter;
}

/**
  *\ingroup gdcmHeader
  *\brief gets the info from 0028,1053 : Rescale Slope
  * @return Rescale Slope
 */
float gdcmHeader::GetRescaleSlope()
{
   float resSlope = 1.;
   std::string strRescSlope = GetEntryByNumber(0x0028,0x1053); //0028 1053 DS IMG Rescale Slope
   if ( strRescSlope != GDCM_UNFOUND )
   {
      if( sscanf( strRescSlope.c_str(), "%f", &resSlope) != 1)
      {
         // bug in the element 0x0028,0x1053
         dbg.Verbose(0, "gdcmHeader::GetRescaleSlope: Rescale Slope is empty");
      }
   }

   return resSlope;
}

/**
  * \ingroup gdcmHeader
  * \brief This function is intended to user who doesn't want 
  *   to have to manage a LUT and expects to get an RBG Pixel image
  *   (or a monochrome one ...) 
  * \warning to be used with GetImagePixels()
  * @return 1 if Gray level, 3 if Color (RGB, YBR or PALETTE COLOR)
  */
int gdcmHeader::GetNumberOfScalarComponents()
{
   if ( GetSamplesPerPixel() == 3 )
   {
      return 3;
   }
      
   // 0028 0100 US IMG Bits Allocated
   // (in order no to be messed up by old RGB images)
   if ( GetEntryByNumber(0x0028,0x0100) == "24" )
   {
      return 3;
   }
       
   std::string strPhotometricInterpretation = GetEntryByNumber(0x0028,0x0004);

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
        strPhotometricInterpretation == "MONOCHROME1 " || 
        strPhotometricInterpretation == "MONOCHROME2 " )
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
  * \ingroup gdcmHeader
  * \brief This function is intended to user that DOESN'T want 
  *  to get RGB pixels image when it's stored as a PALETTE COLOR image
  *   - the (vtk) user is supposed to know how deal with LUTs - 
  * \warning to be used with GetImagePixelsRaw()
  * @return 1 if Gray level, 3 if Color (RGB or YBR - NOT 'PALETTE COLOR' -)
  */
int gdcmHeader::GetNumberOfScalarComponentsRaw()
{
   // 0028 0100 US IMG Bits Allocated
   // (in order no to be messed up by old RGB images)
   if ( gdcmHeader::GetEntryByNumber(0x0028,0x0100) == "24" )
   {
      return 3;
   }

   // we assume that *all* kinds of YBR are dealt with
   return GetSamplesPerPixel();
}

//
// --------------  Remember ! ----------------------------------
//
// Image Position Patient                              (0020,0032):
// If not found (ACR_NEMA) we try Image Position       (0020,0030)
// If not found (ACR-NEMA), we consider Slice Location (0020,1041)
//                                   or Location       (0020,0050) 
// as the Z coordinate, 
// 0. for all the coordinates if nothing is found

// \todo find a way to inform the caller nothing was found
// \todo How to tell the caller a wrong number of values was found?
//
// ---------------------------------------------------------------
//

/**
  * \brief gets the info from 0020,0032 : Image Position Patient
  *                 else from 0020,0030 : Image Position (RET)
  *                 else 0.
  * @return up-left image corner X position
  */
    
float gdcmHeader::GetXOrigin()
{
   float xImPos, yImPos, zImPos;  
   std::string strImPos = GetEntryByNumber(0x0020,0x0032);

   if ( strImPos == GDCM_UNFOUND )
   {
      dbg.Verbose(0, "gdcmHeader::GetXImagePosition: unfound Image Position Patient (0020,0032)");
      strImPos = GetEntryByNumber(0x0020,0x0030); // For ACR-NEMA images
      if ( strImPos == GDCM_UNFOUND )
      {
         dbg.Verbose(0, "gdcmHeader::GetXImagePosition: unfound Image Position (RET) (0020,0030)");
         /// \todo How to tell the caller nothing was found ?
         return 0.;
      }
   }

   if( sscanf( strImPos.c_str(), "%f\\%f\\%f", &xImPos, &yImPos, &zImPos) != 3 )
   {
     return 0.;
   }

   return xImPos;
}

/**
  * \brief gets the info from 0020,0032 : Image Position Patient
  *                 else from 0020,0030 : Image Position (RET)
  *                 else 0.
  * @return up-left image corner Y position
  */
float gdcmHeader::GetYOrigin()
{
   float xImPos, yImPos, zImPos;
   std::string strImPos = GetEntryByNumber(0x0020,0x0032);

   if ( strImPos == GDCM_UNFOUND)
   {
      dbg.Verbose(0, "gdcmHeader::GetYImagePosition: unfound Image Position Patient (0020,0032)");
      strImPos = GetEntryByNumber(0x0020,0x0030); // For ACR-NEMA images
      if ( strImPos == GDCM_UNFOUND )
      {
         dbg.Verbose(0, "gdcmHeader::GetYImagePosition: unfound Image Position (RET) (0020,0030)");
         /// \todo How to tell the caller nothing was found ?
         return 0.;
      }  
   }

   if( sscanf( strImPos.c_str(), "%f\\%f\\%f", &xImPos, &yImPos, &zImPos) != 3 )
   {
      return 0.;
   }

   return yImPos;
}

/**
  * \brief gets the info from 0020,0032 : Image Position Patient
  * \               else from 0020,0030 : Image Position (RET)
  * \               else from 0020,1041 : Slice Location
  * \               else from 0020,0050 : Location
  * \               else 0.
  * @return up-left image corner Z position
  */
float gdcmHeader::GetZOrigin()
{
   float xImPos, yImPos, zImPos; 
   std::string strImPos = GetEntryByNumber(0x0020,0x0032);

   if ( strImPos != GDCM_UNFOUND )
   {
      if( sscanf( strImPos.c_str(), "%f\\%f\\%f", &xImPos, &yImPos, &zImPos) != 3)
      {
         dbg.Verbose(0, "gdcmHeader::GetZImagePosition: wrong Image Position Patient (0020,0032)");
         return 0.;  // bug in the element 0x0020,0x0032
      }
      else
      {
         return zImPos;
      }
   }

   strImPos = GetEntryByNumber(0x0020,0x0030); // For ACR-NEMA images
   if ( strImPos != GDCM_UNFOUND )
   {
      if( sscanf( strImPos.c_str(), 
          "%f\\%f\\%f", &xImPos, &yImPos, &zImPos ) != 3 )
      {
         dbg.Verbose(0, "gdcmHeader::GetZImagePosition: wrong Image Position (RET) (0020,0030)");
         return 0.;  // bug in the element 0x0020,0x0032
      }
      else
      {
         return zImPos;
      }
   }

   std::string strSliceLocation = GetEntryByNumber(0x0020,0x1041); // for *very* old ACR-NEMA images
   if ( strSliceLocation != GDCM_UNFOUND )
   {
      if( sscanf( strSliceLocation.c_str(), "%f", &zImPos) != 1)
      {
         dbg.Verbose(0, "gdcmHeader::GetZImagePosition: wrong Slice Location (0020,1041)");
         return 0.;  // bug in the element 0x0020,0x1041
      }
      else
      {
         return zImPos;
      }
   }
   dbg.Verbose(0, "gdcmHeader::GetZImagePosition: unfound Slice Location (0020,1041)");

   std::string strLocation = GetEntryByNumber(0x0020,0x0050);
   if ( strLocation != GDCM_UNFOUND )
   {
      if( sscanf( strLocation.c_str(), "%f", &zImPos) != 1)
      {
         dbg.Verbose(0, "gdcmHeader::GetZImagePosition: wrong Location (0020,0050)");
         return 0.;  // bug in the element 0x0020,0x0050
      }
      else
      {
         return zImPos;
      }
   }
   dbg.Verbose(0, "gdcmHeader::GetYImagePosition: unfound Location (0020,0050)");  

   return 0.; // Hopeless
}

/**
  * \brief gets the info from 0020,0013 : Image Number
  * \               else 0.
  * @return image number
  */
int gdcmHeader::GetImageNumber()
{
   // The function i atoi() takes the address of an area of memory as
   // parameter and converts the string stored at that location to an integer
   // using the external decimal to internal binary conversion rules. This may
   // be preferable to sscanf() since atoi() is a much smaller, simpler and
   // faster function. sscanf() can do all possible conversions whereas
   // atoi() can only do single decimal integer conversions.
   std::string strImNumber = GetEntryByNumber(0x0020,0x0013); //0020 0013 IS REL Image Number
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
ModalityType gdcmHeader::GetModality()
{
   // 0008 0060 CS ID Modality
   std::string strModality = GetEntryByNumber(0x0008,0x0060);
   if ( strModality != GDCM_UNFOUND )
   {
           if ( strModality.find("AU") < strModality.length()) return AU;
      else if ( strModality.find("AS") < strModality.length()) return AS;
      else if ( strModality.find("BI") < strModality.length()) return BI;
      else if ( strModality.find("CF") < strModality.length()) return CF;
      else if ( strModality.find("CP") < strModality.length()) return CP;
      else if ( strModality.find("CR") < strModality.length()) return CR;
      else if ( strModality.find("CT") < strModality.length()) return CT;
      else if ( strModality.find("CS") < strModality.length()) return CS;
      else if ( strModality.find("DD") < strModality.length()) return DD;
      else if ( strModality.find("DF") < strModality.length()) return DF;
      else if ( strModality.find("DG") < strModality.length()) return DG;
      else if ( strModality.find("DM") < strModality.length()) return DM;
      else if ( strModality.find("DS") < strModality.length()) return DS;
      else if ( strModality.find("DX") < strModality.length()) return DX;
      else if ( strModality.find("ECG") < strModality.length()) return ECG;
      else if ( strModality.find("EPS") < strModality.length()) return EPS;
      else if ( strModality.find("FA") < strModality.length()) return FA;
      else if ( strModality.find("FS") < strModality.length()) return FS;
      else if ( strModality.find("HC") < strModality.length()) return HC;
      else if ( strModality.find("HD") < strModality.length()) return HD;
      else if ( strModality.find("LP") < strModality.length()) return LP;
      else if ( strModality.find("LS") < strModality.length()) return LS;
      else if ( strModality.find("MA") < strModality.length()) return MA;
      else if ( strModality.find("MR") < strModality.length()) return MR;
      else if ( strModality.find("NM") < strModality.length()) return NM;
      else if ( strModality.find("OT") < strModality.length()) return OT;
      else if ( strModality.find("PT") < strModality.length()) return PT;
      else if ( strModality.find("RF") < strModality.length()) return RF;
      else if ( strModality.find("RG") < strModality.length()) return RG;
      else if ( strModality.find("RTDOSE")   < strModality.length()) return RTDOSE;
      else if ( strModality.find("RTIMAGE")  < strModality.length()) return RTIMAGE;
      else if ( strModality.find("RTPLAN")   < strModality.length()) return RTPLAN;
      else if ( strModality.find("RTSTRUCT") < strModality.length()) return RTSTRUCT;
      else if ( strModality.find("SM") < strModality.length()) return SM;
      else if ( strModality.find("ST") < strModality.length()) return ST;
      else if ( strModality.find("TG") < strModality.length()) return TG;
      else if ( strModality.find("US") < strModality.length()) return US;
      else if ( strModality.find("VF") < strModality.length()) return VF;
      else if ( strModality.find("XA") < strModality.length()) return XA;
      else if ( strModality.find("XC") < strModality.length()) return XC;

      else
      {
         /// \todo throw error return value ???
         /// specified <> unknow in our database
         return Unknow;
      }
   }

   return Unknow;
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of Bits Stored (actually used)
 *          (as opposite to number of Bits Allocated)
 * @return  The encountered number of Bits Stored, 0 by default.
 *          0 means the file is NOT USABLE. The caller has to check it !
 */
int gdcmHeader::GetBitsStored()
{
   std::string strSize = GetEntryByNumber(0x0028,0x0101);
   if ( strSize == GDCM_UNFOUND )
   {
      dbg.Verbose(0, "gdcmHeader::GetBitsStored: this is supposed to be mandatory");
      return 0;  // It's supposed to be mandatory
                 // the caller will have to check
   }

   return atoi( strSize.c_str() );
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of Bits Allocated
 *          (8, 12 -compacted ACR-NEMA files, 16, ...)
 * @return  The encountered number of Bits Allocated, 0 by default.
 *          0 means the file is NOT USABLE. The caller has to check it !
 */
int gdcmHeader::GetBitsAllocated()
{
   std::string strSize = GetEntryByNumber(0x0028,0x0100);
   if ( strSize == GDCM_UNFOUND )
   {
      dbg.Verbose(0, "gdcmHeader::GetBitsStored: this is supposed to be mandatory");
      return 0; // It's supposed to be mandatory
                // the caller will have to check
   }

   return atoi( strSize.c_str() );
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the number of Samples Per Pixel
 *          (1 : gray level, 3 : RGB -1 or 3 Planes-)
 * @return  The encountered number of Samples Per Pixel, 1 by default.
 *          (Gray level Pixels)
 */
int gdcmHeader::GetSamplesPerPixel()
{
   std::string strSize = GetEntryByNumber(0x0028,0x0002);
   if ( strSize == GDCM_UNFOUND )
   {
      dbg.Verbose(0, "gdcmHeader::GetBitsStored: this is supposed to be mandatory");
      return 1; // Well, it's supposed to be mandatory ...
                // but sometimes it's missing : *we* assume Gray pixels
   }

   return atoi( strSize.c_str() );
}

/**
 * \ingroup gdcmHeader
 * \brief   Retrieve the Planar Configuration for RGB images
 *          (0 : RGB Pixels , 1 : R Plane + G Plane + B Plane)
 * @return  The encountered Planar Configuration, 0 by default.
 */
int gdcmHeader::GetPlanarConfiguration()
{
   std::string strSize = GetEntryByNumber(0x0028,0x0006);
   if ( strSize == GDCM_UNFOUND )
   {
      return 0;
   }

   return atoi( strSize.c_str() );
}

/**
 * \ingroup gdcmHeader
 * \brief   Return the size (in bytes) of a single pixel of data.
 * @return  The size in bytes of a single pixel of data; 0 by default
 *          0 means the file is NOT USABLE; the caller will have to check        
 */
int gdcmHeader::GetPixelSize()
{
   // 0028 0100 US IMG Bits Allocated
   // (in order no to be messed up by old RGB images)
   //   if (gdcmHeader::GetEntryByNumber(0x0028,0x0100) == "24")
   //      return 3;

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

   dbg.Verbose(0, "gdcmHeader::GetPixelSize: Unknown pixel type");
   return 0;
}

/**
 * \ingroup gdcmHeader
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
 *          24 bit images appear as 8 bit
 * @return  0S if nothing found. NOT USABLE file. The caller has to check
 */
std::string gdcmHeader::GetPixelType()
{
   std::string bitsAlloc = GetEntryByNumber(0x0028, 0x0100); // Bits Allocated
   if ( bitsAlloc == GDCM_UNFOUND )
   {
      dbg.Verbose(0, "gdcmHeader::GetPixelType: unfound Bits Allocated");
      bitsAlloc = "16";
   }

   if ( bitsAlloc == "64" )
   {
      return "FD";
   }
   if ( bitsAlloc == "12" )
   {
      // It will be unpacked
      bitsAlloc = "16";
   }
   else if ( bitsAlloc == "24" )
   {
      // (in order no to be messed up
      bitsAlloc = "8";  // by old RGB images)
   }

   std::string sign = GetEntryByNumber(0x0028, 0x0103); // "Pixel Representation"

   if (sign == GDCM_UNFOUND )
   {
      dbg.Verbose(0, "gdcmHeader::GetPixelType: unfound Pixel Representation");
      bitsAlloc = "0";
   }
   if ( sign == "0" )
   {
      sign = "U";
   }
   else
   {
      sign = "S";
   }

   return bitsAlloc + sign;
}


/**
 * \ingroup gdcmHeader
 * \brief   Recover the offset (from the beginning of the file) 
 *          of *image* pixels (not *icone image* pixels, if any !)
 * @return Pixel Offset
 */
size_t gdcmHeader::GetPixelOffset()
{
   gdcmDocEntry* pxlElement = GetDocEntryByNumber(GrPixel,NumPixel);
   if ( pxlElement )
   {
      return pxlElement->GetOffset();
   }
   else
   {
#ifdef GDCM_DEBUG
      std::cout << "Big trouble : Pixel Element ("
                << std::hex << GrPixel<<","<< NumPixel<< ") NOT found"
                << std::endl;  
#endif //GDCM_DEBUG
      return 0;
   }
}

// TODO : unify those two (previous one and next one)
/**
 * \ingroup gdcmHeader
 * \brief   Recover the pixel area length (in Bytes)
 * @return Pixel Element Length, as stored in the header
 *         (NOT the memory space necessary to hold the Pixels 
 *          -in case of embeded compressed image-)
 *         0 : NOT USABLE file. The caller has to check.
 */
size_t gdcmHeader::GetPixelAreaLength()
{
   gdcmDocEntry* pxlElement = GetDocEntryByNumber(GrPixel,NumPixel);
   if ( pxlElement )
   {
      return pxlElement->GetLength();
   }
   else
   {
#ifdef GDCM_DEBUG
      std::cout << "Big trouble : Pixel Element ("
                << std::hex << GrPixel<<","<< NumPixel<< ") NOT found"
                << std::endl;
#endif //GDCM_DEBUG
      return 0;
   }
}

/**
  * \ingroup gdcmHeader
  * \brief tells us if LUT are used
  * \warning Right now, 'Segmented xxx Palette Color Lookup Table Data'
  *          are NOT considered as LUT, since nobody knows
  *          how to deal with them
  *          Please warn me if you know sbdy that *does* know ... jprx
  * @return true if LUT Descriptors and LUT Tables were found 
  */
bool gdcmHeader::HasLUT()
{
   // Check the presence of the LUT Descriptors, and LUT Tables    
   // LutDescriptorRed    
   if ( !GetDocEntryByNumber(0x0028,0x1101) )
   {
      return false;
   }
   // LutDescriptorGreen 
   if ( !GetDocEntryByNumber(0x0028,0x1102) )
   {
      return false;
   }
   // LutDescriptorBlue 
   if ( !GetDocEntryByNumber(0x0028,0x1103) )
   {
      return false;
   }
   // Red Palette Color Lookup Table Data
   if ( !GetDocEntryByNumber(0x0028,0x1201) )
   {
      return false;
   }
   // Green Palette Color Lookup Table Data       
   if ( !GetDocEntryByNumber(0x0028,0x1202) )
   {
      return false;
   }
   // Blue Palette Color Lookup Table Data      
   if ( !GetDocEntryByNumber(0x0028,0x1203) )
   {
      return false;
   }

   // FIXME : (0x0028,0x3006) : LUT Data (CTX dependent)
   //         NOT taken into account, but we don't know how to use it ...   
   return true;
}

/**
  * \ingroup gdcmHeader
  * \brief gets the info from 0028,1101 : Lookup Table Desc-Red
  *             else 0
  * @return Lookup Table number of Bits , 0 by default
  *          when (0028,0004),Photometric Interpretation = [PALETTE COLOR ]
  * @ return bit number of each LUT item 
  */
int gdcmHeader::GetLUTNbits()
{
   std::vector<std::string> tokens;
   int lutNbits;

   //Just hope Lookup Table Desc-Red = Lookup Table Desc-Red = Lookup Table Desc-Blue
   // Consistency already checked in GetLUTLength
   std::string lutDescription = GetEntryByNumber(0x0028,0x1101);
   if ( lutDescription == GDCM_UNFOUND )
   {
      return 0;
   }

   tokens.clear(); // clean any previous value
   Tokenize ( lutDescription, tokens, "\\" );
   //LutLength=atoi(tokens[0].c_str());
   //LutDepth=atoi(tokens[1].c_str());

   lutNbits = atoi( tokens[2].c_str() );
   tokens.clear();

   return lutNbits;
}

/**
  * \ingroup gdcmHeader
  * \brief builts Red/Green/Blue/Alpha LUT from Header
  *         when (0028,0004),Photometric Interpretation = [PALETTE COLOR ]
  *          and (0028,1101),(0028,1102),(0028,1102)  
  *            - xxx Palette Color Lookup Table Descriptor - are found
  *          and (0028,1201),(0028,1202),(0028,1202) 
  *            - xxx Palette Color Lookup Table Data - are found 
  * \warning does NOT deal with :
  *   0028 1100 Gray Lookup Table Descriptor (Retired)
  *   0028 1221 Segmented Red Palette Color Lookup Table Data
  *   0028 1222 Segmented Green Palette Color Lookup Table Data
  *   0028 1223 Segmented Blue Palette Color Lookup Table Data 
  *   no known Dicom reader deals with them :-(
  * @return a RGBA Lookup Table 
  */ 
uint8_t* gdcmHeader::GetLUTRGBA()
{
   // Not so easy : see 
   // http://www.barre.nom.fr/medical/dicom2/limitations.html#Color%20Lookup%20Tables

//  if Photometric Interpretation # PALETTE COLOR, no LUT to be done
   if ( GetEntryByNumber(0x0028,0x0004) != "PALETTE COLOR " )
   {
      return NULL;
   }

   int lengthR, debR, nbitsR;
   int lengthG, debG, nbitsG;
   int lengthB, debB, nbitsB;
   
   // Get info from Lut Descriptors
   // (the 3 LUT descriptors may be different)    
   std::string lutDescriptionR = GetEntryByNumber(0x0028,0x1101);
   if ( lutDescriptionR == GDCM_UNFOUND )
   {
      return NULL;
   }

   std::string lutDescriptionG = GetEntryByNumber(0x0028,0x1102);
   if ( lutDescriptionG == GDCM_UNFOUND )
   {
      return NULL;
   }

   std::string lutDescriptionB = GetEntryByNumber(0x0028,0x1103);
   if ( lutDescriptionB == GDCM_UNFOUND )
   {
      return NULL;
   }

   // lengthR: Red LUT length in Bytes
   // debR:    subscript of the first Lut Value
   // nbitsR:  Lut item size (in Bits)
   int nbRead = sscanf( lutDescriptionR.c_str(), "%d\\%d\\%d", 
                        &lengthR, &debR, &nbitsR );
   if( nbRead != 3 )
   {
      dbg.Verbose(0, "gdcmHeader::GetLUTRGBA: trouble reading red LUT");
   }
   
   // lengthG: Green LUT length in Bytes
   // debG:    subscript of the first Lut Value
   // nbitsG:  Lut item size (in Bits)
   nbRead = sscanf( lutDescriptionG.c_str(), "%d\\%d\\%d", 
                    &lengthG, &debG, &nbitsG );
   if( nbRead != 3 )
   {
      dbg.Verbose(0, "gdcmHeader::GetLUTRGBA: trouble reading green LUT");
   }

   // lengthB: Blue LUT length in Bytes
   // debB:    subscript of the first Lut Value
   // nbitsB:  Lut item size (in Bits)
   nbRead = sscanf( lutDescriptionB.c_str(), "%d\\%d\\%d", 
                    &lengthB, &debB, &nbitsB );
   if( nbRead != 3 )
   {
      dbg.Verbose(0, "gdcmHeader::GetLUTRGBA: trouble reading blue LUT");
   }
 
   // Load LUTs into memory, (as they were stored on disk)
   uint8_t* lutR = (uint8_t*) GetEntryVoidAreaByNumber(0x0028,0x1201);
   uint8_t* lutG = (uint8_t*) GetEntryVoidAreaByNumber(0x0028,0x1202);
   uint8_t* lutB = (uint8_t*) GetEntryVoidAreaByNumber(0x0028,0x1203); 

   if ( !lutR || !lutG || !lutB )
   {
      dbg.Verbose(0, "gdcmHeader::GetLUTRGBA: trouble with one of the LUT");
      return NULL;
   } 
   // forge the 4 * 8 Bits Red/Green/Blue/Alpha LUT 

   uint8_t* LUTRGBA = new uint8_t[1024]; // 256 * 4 (R, G, B, Alpha) 
   if ( !LUTRGBA )
   {
      return NULL;
   }
   memset(LUTRGBA, 0, 1024);

   // Bits Allocated
   int nb;
   std::string str_nb = GetEntryByNumber(0x0028,0x0100);
   if ( str_nb == GDCM_UNFOUND )
   {
      nb = 16;
   }
   else
   {
      nb = atoi( str_nb.c_str() );
   }
   int mult;

   if ( nbitsR == 16 && nb == 8)
   {
      // when LUT item size is different than pixel size
      mult = 2; // high byte must be = low byte 
   }
   else
   {
      // See PS 3.3-2003 C.11.1.1.2 p 619
      mult = 1;
   }

   // if we get a black image, let's just remove the '+1'
   // from 'i*mult+1' and check again 
   // if it works, we shall have to check the 3 Palettes
   // to see which byte is ==0 (first one, or second one)
   // and fix the code
   // We give up the checking to avoid some (useless ?)overhead 
   // (optimistic asumption)
   uint8_t* a;      
   int i;

   a = LUTRGBA + 0;
   for( i=0; i < lengthR; ++i)
   {
      *a = lutR[i*mult+1];
      a += 4;
   }        

   a = LUTRGBA + 1;
   for( i=0; i < lengthG; ++i)
   {
      *a = lutG[i*mult+1];
      a += 4;
   }  

   a = LUTRGBA + 2;
   for(i=0; i < lengthB; ++i)
   {
      *a = lutB[i*mult+1];
      a += 4;
   }

   a = LUTRGBA + 3;
   for(i=0; i < 256; ++i)
   {
      *a = 1; // Alpha component
      a += 4;
   }

   return LUTRGBA;
} 

/**
 * \brief Accesses the info from 0002,0010 : Transfert Syntax and gdcmTS
 *        else 1.
 * @return The full Transfert Syntax Name (as opposed to Transfert Syntax UID)
 */
std::string gdcmHeader::GetTransfertSyntaxName()
{
   // use the gdcmTS (TS : Transfert Syntax)
   std::string transfertSyntax = GetEntryByNumber(0x0002,0x0010);
   if ( transfertSyntax == GDCM_UNFOUND )
   {
      dbg.Verbose(0, "gdcmHeader::GetTransfertSyntaxName:"
                     " unfound Transfert Syntax (0002,0010)");
      return "Uncompressed ACR-NEMA";
   }

   // we do it only when we need it
   gdcmTS* ts         = gdcmGlobal::GetTS();
   std::string tsName = ts->GetValue( transfertSyntax );

   //delete ts; /// \todo Seg Fault when deleted ?!
   return tsName;
}

/**
 * \brief Sets the Pixel Area size in the Header
 *        --> not-for-rats function
 * @param ImageDataSize new Pixel Area Size
 *        warning : nothing else is checked
 */
void gdcmHeader::SetImageDataSize(size_t ImageDataSize)
{
   std::string content1;
   char car[20];

   sprintf(car,"%d",ImageDataSize);
 
   gdcmDocEntry *a = GetDocEntryByNumber(GrPixel, NumPixel);
   a->SetLength(ImageDataSize);

   ImageDataSize+=8;
   sprintf(car,"%d",ImageDataSize);
   content1=car;
   SetEntryByNumber(content1, GrPixel, NumPixel);
}

//-----------------------------------------------------------------------------
// Protected

/**
 * \brief anonymize a Header (removes Patient's personal info)
 *        (read the code to see which ones ...)
 */
bool gdcmHeader::AnonymizeHeader()
{
   gdcmDocEntry* patientNameHE = GetDocEntryByNumber (0x0010, 0x0010);

   ReplaceIfExistByNumber ("  ",0x0010, 0x2154); // Telephone   
   ReplaceIfExistByNumber ("  ",0x0010, 0x1040); // Adress
   ReplaceIfExistByNumber ("  ",0x0010, 0x0020); // Patient ID
  
   if ( patientNameHE )
   {
      std::string studyInstanceUID =  GetEntryByNumber (0x0020, 0x000d);
      if ( studyInstanceUID != GDCM_UNFOUND )
      {
         ReplaceOrCreateByNumber(studyInstanceUID, 0x0010, 0x0010);
      }
      else
      {
         ReplaceOrCreateByNumber(std::string("anonymised"), 0x0010, 0x0010);
      }
   }

  // Just for fun :-(
  // (if any) remove or replace all the stuff that contains a Date

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
  * \brief gets the info from 0020,0037 : Image Orientation Patient
  * @param iop adress of the (6)float aray to receive values
  * @return cosines of image orientation patient
  */
void gdcmHeader::GetImageOrientationPatient( float* iop )
{
   //iop is supposed to be float[6]
   iop[0] = iop[1] = iop[2] = iop[3] = iop[4] = iop[5] = 0;

   // 0020 0037 DS REL Image Orientation (Patient)
   std::string strImOriPat = GetEntryByNumber(0x0020,0x0037);
   if ( strImOriPat != GDCM_UNFOUND )
   {
      if( sscanf( strImOriPat.c_str(), "%f\\%f\\%f\\%f\\%f\\%f", 
          &iop[0], &iop[1], &iop[2], &iop[3], &iop[4], &iop[5]) != 6 )
      {
         dbg.Verbose(0, "gdcmHeader::GetImageOrientationPatient: wrong Image Orientation Patient (0020,0037)");
         return ;  // bug in the element 0x0020,0x0037
      }
      else
      {
         return ;
      }
   }

   //For ACR-NEMA
   // 0020 0035 DS REL Image Orientation (RET)
   strImOriPat = GetEntryByNumber(0x0020,0x0035);
   if ( strImOriPat != GDCM_UNFOUND )
   {
      if( sscanf( strImOriPat.c_str(), "%f\\%f\\%f\\%f\\%f\\%f", 
          &iop[0], &iop[1], &iop[2], &iop[3], &iop[4], &iop[5]) != 6 )
      {
         dbg.Verbose(0, "gdcmHeader::GetImageOrientationPatient: wrong Image Orientation Patient (0020,0035)");
         return ;  // bug in the element 0x0020,0x0035
      }
   }
   else
   {
      return;
   }
}

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
