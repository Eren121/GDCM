// $Header: /cvs/public/gdcm/src/Attic/gdcmHeaderHelper.cxx,v 1.14 2003/10/27 14:01:12 jpr Exp $

#include "gdcmHeaderHelper.h"

#include "gdcmUtil.h" //for debug
#include <math.h>
#include <algorithm>
#include <string.h> //for bzero

//directory manipulation (os indep).
//cygwin ???? -> _WIN32 ??
#ifdef _MSC_VER 
#include <windows.h> 
int GetDir(std::string dPath, std::list<std::string> &filenames)
{
  //For now dPath should have an ending "\"
  WIN32_FIND_DATA FileData; 
  HANDLE hFile; 
  hFile = FindFirstFile((dPath+"*").c_str(), &FileData); 
  if ( hFile == INVALID_HANDLE_VALUE ) 
  { 
    //No files !
    return false; 
  } 
  
  if( strncmp(FileData.cFileName, ".", 1) != 0 )
    filenames.push_back( dPath+FileData.cFileName );
  while( FindNextFile(hFile, &FileData ) != 0)
  { 
    if( strncmp(FileData.cFileName, ".", 1) != 0 )
      filenames.push_back( dPath+FileData.cFileName );
  }
  return true;
}

#else
#include <dirent.h>

int GetDir(std::string dPath, std::list<std::string> &filenames)
{
 DIR *dir = opendir( dPath.c_str() );
 struct dirent *entry;
 while((entry = readdir(dir)) != NULL)
 {
//   if( strncmp(entry->d_name, ".", 1) != 0 && strncmp(entry->d_name, "..", 2) != 0)
   if( strncmp(entry->d_name, ".", 1) != 0 )
   {
      filenames.push_back( dPath + "/" + entry->d_name );
   }
 }
 closedir(dir);
 return true;
}

#endif

//----------------------------------------------------------------------------
/**
 * \ingroup gdcmHeaderHelper
 * \brief   cstor
 */
gdcmHeaderHelper::gdcmHeaderHelper() : gdcmHeader( )
{
}
//----------------------------------------------------------------------------
/**
 * \ingroup gdcmHeaderHelper
 * \brief   cstor
 */
gdcmHeaderHelper::gdcmHeaderHelper(const char *InFilename, 
    bool exception_on_error) : gdcmHeader( InFilename , exception_on_error)
{
}
//----------------------------------------------------------------------------
/**
 * \ingroup gdcmHeaderHelper
 * \brief   Return the size (in bytes) of a single pixel of data.
 * @return  The size in bytes of a single pixel of data.
 *
 */
int gdcmHeaderHelper::GetPixelSize() {
   std::string PixelType = GetPixelType();
   if (PixelType == "8U"  || PixelType == "8S")
      return 1;
   if (PixelType == "16U" || PixelType == "16S")
      return 2;
   if (PixelType == "32U" || PixelType == "32S")
      return 4;
   dbg.Verbose(0, "gdcmHeader::GetPixelSize: Unknown pixel type");
   return 0;
}

//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief gets the info from 0028,0004 : Photometric Interp
  * \           else 1.
  * @return 1 if Gray level, 3 if Color
  */
int gdcmHeaderHelper::GetNumberOfScalarComponents() {
      std::string PhotometricInterpretation = 
                  gdcmHeader::GetPubElValByNumber(0x0028,0x0004);


// The compiler will optimze, if it feels like !

      //beware of trailing space at end of string
      if (PhotometricInterpretation.find(GDCM_UNFOUND) < PhotometricInterpretation.length() || 
          PhotometricInterpretation.find("MONOCHROME1") < PhotometricInterpretation.length() || 
          PhotometricInterpretation.find("MONOCHROME2") < PhotometricInterpretation.length() ) return 1;

            // WARNING : quick and dirty trick to produce a single plane Grey image
	    // See also  gdcmFile::GetImageDataIntoVector()
           // if(GetPubElValVoidAreaByNumber(0x0028,0x1201)==NULL) return 1; // Lut Red
            // end of dirty trick

      return 3;
}
//----------------------------------------------------------------------------
/**
 * \ingroup gdcmHeaderHelper
 * \brief   Build the Pixel Type of the image.
 *          Possible values are:
 *          - 8U  unsigned  8 bit,
 *          - 8S    signed  8 bit,
 *          - 16U unsigned 16 bit,
 *          - 16S   signed 16 bit,
 *          - 32U unsigned 32 bit,
 *          - 32S   signed 32 bit,
 * \warning 12 bit images appear as 16 bit.
 * @return  
 */
std::string gdcmHeaderHelper::GetPixelType() {
   std::string BitsAlloc;
   BitsAlloc = GetElValByName("Bits Allocated");
   if (BitsAlloc == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetPixelType: unfound Bits Allocated");
      BitsAlloc = std::string("16");
   }
   if (BitsAlloc == "12")
      BitsAlloc = std::string("16");

   std::string Signed;
   Signed = GetElValByName("Pixel Representation");
   if (Signed == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetPixelType: unfound Pixel Representation");
      BitsAlloc = std::string("0");
   }
   if (Signed == "0")
      Signed = std::string("U");
   else
      Signed = std::string("S");

   return( BitsAlloc + Signed);
}
//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief gets the info from 0028,0030 : Pixel Spacing
  * \           else 1.
  * @return X dimension of a pixel
  */
float gdcmHeaderHelper::GetXSpacing() {
    float xspacing, yspacing;
    std::string StrSpacing = GetPubElValByNumber(0x0028,0x0030);
    
   if (StrSpacing == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetXSpacing: unfound Pixel Spacing (0028,0030)");
      return 1.;
    }
  if( sscanf( StrSpacing.c_str(), "%f\\%f", &yspacing, &xspacing) != 2)
    return 0.;
  if (xspacing == 0.) {
    dbg.Verbose(0, "gdcmHeader::GetYSpacing: gdcmData/CT-MONO2-8-abdo.dcm problem");
    // seems to be a bug in the header ...
    sscanf( StrSpacing.c_str(), "%f\\0\\%f", &yspacing, &xspacing);
  }
  return xspacing;
}
//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief gets the info from 0028,0030 : Pixel Spacing
  * \           else 1.
  * @return Y dimension of a pixel
  */
float gdcmHeaderHelper::GetYSpacing() {
   float xspacing, yspacing;
   std::string StrSpacing = GetPubElValByNumber(0x0028,0x0030);
  
   if (StrSpacing == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetYSpacing: unfound Pixel Spacing (0028,0030)");
      return 1.;
    }
  if( sscanf( StrSpacing.c_str(), "%f\\%f", &yspacing, &xspacing) != 2)
    return 0.;
  if (xspacing == 0.) {
    dbg.Verbose(0, "gdcmHeader::GetYSpacing: gdcmData/CT-MONO2-8-abdo.dcm problem");
    // seems to be a bug in the header ...
    sscanf( StrSpacing.c_str(), "%f\\0\\%f", &yspacing, &xspacing);
  }
  return yspacing;
} 

//----------------------------------------------------------------------------
/**
  *\ingroup gdcmHeaderHelper
  *\brief gets the info from 0018,0088 : Space Between Slices
  *\               else from 0018,0050 : Slice Thickness
  *\               else 1.
  * @return Z dimension of a voxel-to be
  */
float gdcmHeaderHelper::GetZSpacing() {
   // TODO : translate into English
   // Spacing Between Slices : distance entre le milieu de chaque coupe
   // Les coupes peuvent etre :
   //   jointives     (Spacing between Slices = Slice Thickness)
   //   chevauchantes (Spacing between Slices < Slice Thickness)
   //   disjointes    (Spacing between Slices > Slice Thickness)
   // Slice Thickness : epaisseur de tissus sur laquelle est acquis le signal
   //   ca interesse le physicien de l'IRM, pas le visualisateur de volumes ...
   //   Si le Spacing Between Slices est absent, 
   //   on suppose que les coupes sont jointives
   
   std::string StrSpacingBSlices = GetPubElValByNumber(0x0018,0x0088);

   if (StrSpacingBSlices == GDCM_UNFOUND) {
      dbg.Verbose(0, "gdcmHeader::GetZSpacing: unfound StrSpacingBSlices");
      std::string StrSliceThickness = GetPubElValByNumber(0x0018,0x0050);       
      if (StrSliceThickness == GDCM_UNFOUND)
         return 1.;
      else
         // if no 'Spacing Between Slices' is found, 
         // we assume slices join together
         // (no overlapping, no interslice gap)
         // if they don't, we're fucked up
         return atof(StrSliceThickness.c_str());  
   } else {
      return atof(StrSpacingBSlices.c_str());
   }
}

//----------------------------------------------------------------------------
//
// Image Position Patient                              (0020,0032):
// If not found (ACR_NEMA) we try Image Position       (0020,0030)
// If not found (ACR-NEMA), we consider Slice Location (0020,1041)
//                                   or Location       (0020,0050) 
// as the Z coordinate, 
// 0. for all the coordinates if nothing is found

// TODO : find a way to inform the caller nothing was found
// TODO : How to tell the caller a wrong number of values was found?

/**
  * \ingroup gdcmHeaderHelper
  * \brief gets the info from 0020,0032 : Image Position Patient
  *\                else from 0020,0030 : Image Position (RET)
  *\                else 0.
  * @return up-left image corner position
  */
float gdcmHeaderHelper::GetXOrigin() {
    float xImPos, yImPos, zImPos;  
    std::string StrImPos = GetPubElValByNumber(0x0020,0x0032);

    if (StrImPos == GDCM_UNFOUND) {
       dbg.Verbose(0, "gdcmHeader::GetXImagePosition: unfound Image Position Patient (0020,0032)");
       StrImPos = GetPubElValByNumber(0x0020,0x0030); // For ACR-NEMA images
       if (StrImPos == GDCM_UNFOUND) {
          dbg.Verbose(0, "gdcmHeader::GetXImagePosition: unfound Image Position (RET) (0020,0030)");
          // How to tell the caller nothing was found ?
         return 0.;
       }  
     }
   if( sscanf( StrImPos.c_str(), "%f\\%f\\%f", &xImPos, &yImPos, &zImPos) != 3)
     return 0.;
   return xImPos;
}
//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief gets the info from 0020,0032 : Image Position Patient
  * \               else from 0020,0030 : Image Position (RET)
  * \               else 0.
  * @return up-left image corner position
  */
float gdcmHeaderHelper::GetYOrigin() {
    float xImPos, yImPos, zImPos;
    std::string StrImPos = GetPubElValByNumber(0x0020,0x0032);

    if (StrImPos == GDCM_UNFOUND) {
       dbg.Verbose(0, "gdcmHeader::GetYImagePosition: unfound Image Position Patient (0020,0032)");
       StrImPos = GetPubElValByNumber(0x0020,0x0030); // For ACR-NEMA images
       if (StrImPos == GDCM_UNFOUND) {
          dbg.Verbose(0, "gdcmHeader::GetYImagePosition: unfound Image Position (RET) (0020,0030)");
          // How to tell the caller nothing was found ?
           return 0.;
       }  
     }
   if( sscanf( StrImPos.c_str(), "%f\\%f\\%f", &xImPos, &yImPos, &zImPos) != 3)
     return 0.;
   return yImPos;
}
//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief gets the info from 0020,0032 : Image Position Patient
  * \               else from 0020,0030 : Image Position (RET)
  * \               else from 0020,1041 : Slice Location
  * \               else from 0020,0050 : Location
  * \               else 0.
  * @return up-left image corner position
  */
float gdcmHeaderHelper::GetZOrigin() {
   float xImPos, yImPos, zImPos; 
   std::string StrImPos = GetPubElValByNumber(0x0020,0x0032);
   if (StrImPos != GDCM_UNFOUND) {
      if( sscanf( StrImPos.c_str(), "%f\\%f\\%f", &xImPos, &yImPos, &zImPos) != 3) {
         dbg.Verbose(0, "gdcmHeader::GetZImagePosition: wrong Image Position Patient (0020,0032)");
         return 0.;  // bug in the element 0x0020,0x0032
      } else {
         return zImPos;
      }    
   }  
   StrImPos = GetPubElValByNumber(0x0020,0x0030); // For ACR-NEMA images
   if (StrImPos != GDCM_UNFOUND) {
      if( sscanf( StrImPos.c_str(), "%f\\%f\\%f", &xImPos, &yImPos, &zImPos) != 3) {
         dbg.Verbose(0, "gdcmHeader::GetZImagePosition: wrong Image Position (RET) (0020,0030)");
         return 0.;  // bug in the element 0x0020,0x0032
      } else {
         return zImPos;
      }    
   }                
   std::string StrSliceLocation = GetPubElValByNumber(0x0020,0x1041);// for *very* old ACR-NEMA images
   if (StrSliceLocation != GDCM_UNFOUND) {
      if( sscanf( StrSliceLocation.c_str(), "%f", &zImPos) !=1) {
         dbg.Verbose(0, "gdcmHeader::GetZImagePosition: wrong Slice Location (0020,1041)");
         return 0.;  // bug in the element 0x0020,0x1041
      } else {
         return zImPos;
      }
   }   
   dbg.Verbose(0, "gdcmHeader::GetZImagePosition: unfound Slice Location (0020,1041)");
   std::string StrLocation = GetPubElValByNumber(0x0020,0x0050);
   if (StrLocation != GDCM_UNFOUND) {
      if( sscanf( StrLocation.c_str(), "%f", &zImPos) !=1) {
         dbg.Verbose(0, "gdcmHeader::GetZImagePosition: wrong Location (0020,0050)");
         return 0.;  // bug in the element 0x0020,0x0050
      } else {
         return zImPos;
      }
   }
   dbg.Verbose(0, "gdcmHeader::GetYImagePosition: unfound Location (0020,0050)");  
   return 0.; // Hopeless
}
//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief gets the info from 0020,0013 : Image Number
  * \               else 0.
  * @return image number
  */
int gdcmHeaderHelper::GetImageNumber() {
  //The function i atoi() takes the address of an area of memory as parameter and converts 
  //the string stored at that location to an integer using the external decimal to internal
  //binary conversion rules. This may be preferable to sscanf() since atoi() is a much smaller,
  // simpler and faster function. sscanf() can do all possible conversions whereas atoi() can 
  //only do single decimal integer conversions.
  std::string StrImNumber = GetPubElValByNumber(0x0020,0x0013); //0020 0013 IS REL Image Number
  if (StrImNumber != GDCM_UNFOUND) {
    return atoi( StrImNumber.c_str() );
  }
  return 0;   //Hopeless
}
//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief gets the info from 0008,0060 : Modality
  * @return ModalityType
  */
ModalityType gdcmHeaderHelper::GetModality(void) {
  std::string StrModality = GetPubElValByNumber(0x0008,0x0060); //0008 0060 CS ID Modality
  if (StrModality != GDCM_UNFOUND) {
         if ( StrModality.find("AU") < StrModality.length()) return AU;
    else if ( StrModality.find("AS") < StrModality.length()) return AS;
    else if ( StrModality.find("BI") < StrModality.length()) return BI;
    else if ( StrModality.find("CF") < StrModality.length()) return CF;
    else if ( StrModality.find("CP") < StrModality.length()) return CP;
    else if ( StrModality.find("CR") < StrModality.length()) return CR;
    else if ( StrModality.find("CT") < StrModality.length()) return CT;
    else if ( StrModality.find("CS") < StrModality.length()) return CS;
    else if ( StrModality.find("DD") < StrModality.length()) return DD;
    else if ( StrModality.find("DF") < StrModality.length()) return DF;
    else if ( StrModality.find("DG") < StrModality.length()) return DG;
    else if ( StrModality.find("DM") < StrModality.length()) return DM;
    else if ( StrModality.find("DS") < StrModality.length()) return DS;
    else if ( StrModality.find("DX") < StrModality.length()) return DX;
    else if ( StrModality.find("ECG") < StrModality.length()) return ECG;
    else if ( StrModality.find("EPS") < StrModality.length()) return EPS;
    else if ( StrModality.find("FA") < StrModality.length()) return FA;
    else if ( StrModality.find("FS") < StrModality.length()) return FS;
    else if ( StrModality.find("HC") < StrModality.length()) return HC;
    else if ( StrModality.find("HD") < StrModality.length()) return HD;
    else if ( StrModality.find("LP") < StrModality.length()) return LP;
    else if ( StrModality.find("LS") < StrModality.length()) return LS;
    else if ( StrModality.find("MA") < StrModality.length()) return MA;
    else if ( StrModality.find("MR") < StrModality.length()) return MR;
    else if ( StrModality.find("NM") < StrModality.length()) return NM;
    else if ( StrModality.find("OT") < StrModality.length()) return OT;
    else if ( StrModality.find("PT") < StrModality.length()) return PT;
    else if ( StrModality.find("RF") < StrModality.length()) return RF;
    else if ( StrModality.find("RG") < StrModality.length()) return RG;
    else if ( StrModality.find("RTDOSE")  < StrModality.length()) return RTDOSE;
    else if ( StrModality.find("RTIMAGE") < StrModality.length()) return RTIMAGE;
    else if ( StrModality.find("RTPLAN")  < StrModality.length()) return RTPLAN;
    else if ( StrModality.find("RTSTRUCT")< StrModality.length()) return RTSTRUCT;
    else if ( StrModality.find("SM") < StrModality.length()) return SM;
    else if ( StrModality.find("ST") < StrModality.length()) return ST;
    else if ( StrModality.find("TG") < StrModality.length()) return TG;
    else if ( StrModality.find("US") < StrModality.length()) return US;
    else if ( StrModality.find("VF") < StrModality.length()) return VF;
    else if ( StrModality.find("XA") < StrModality.length()) return XA;
    else if ( StrModality.find("XC") < StrModality.length()) return XC;

    else
    {
      //throw error return value ???
      // specified <> unknow in our database
      return Unknow;
    }
  }
  return Unknow;
}

//----------------------------------------------------------------------------
std::string gdcmHeaderHelper::GetStudyUID()
{
  return GetPubElValByNumber(0x0020,0x000d); //0020 000d UI REL Study Instance UID
}
//----------------------------------------------------------------------------
std::string gdcmHeaderHelper::GetSeriesUID()
{
  return GetPubElValByNumber(0x0020,0x000e); //0020 000e UI REL Series Instance UID
}
//----------------------------------------------------------------------------
std::string gdcmHeaderHelper::GetClassUID()
{
  return GetPubElValByNumber(0x0008,0x0016); //0008 0016 UI ID SOP Class UID
}
//----------------------------------------------------------------------------
std::string gdcmHeaderHelper::GetInstanceUID()
{
  return GetPubElValByNumber(0x0008,0x0018); //0008 0018 UI ID SOP Instance UID
}









gdcmSerieHeaderHelper::~gdcmSerieHeaderHelper()
{
  //! \todo
  for (std::list<gdcmHeaderHelper*>::iterator it  = CoherentGdcmFileList.begin();
        it != CoherentGdcmFileList.end(); it++)
  {
    delete *it;
  }
  CoherentGdcmFileList.clear();
}
//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief gets the info from 0020,0037 : Image Orientation Patient
  * @return cosines of image orientation patient
  */
void gdcmHeaderHelper::GetImageOrientationPatient( float* iop ) {

  //iop is supposed to be float[6]
  iop[0] = iop[1] = iop[2] = iop[3] = iop[4] = iop[5] = 0;
  
  std::string StrImOriPat = GetPubElValByNumber(0x0020,0x0037); // 0020 0037 DS REL Image Orientation (Patient)
  if (StrImOriPat != GDCM_UNFOUND) {
    if( sscanf( StrImOriPat.c_str(), "%f\\%f\\%f\\%f\\%f\\%f", 
            &iop[0], &iop[1], &iop[2], &iop[3], &iop[4], &iop[5]) != 6) {
         dbg.Verbose(0, "gdcmHeader::GetImageOrientationPatient: wrong Image Orientation Patient (0020,0037)");
         return ;  // bug in the element 0x0020,0x0037
    } 
    else
      return ;
  }
  
  //For ACR-NEMA
  StrImOriPat = GetPubElValByNumber(0x0020,0x0035); //0020 0035 DS REL Image Orientation (RET)
  if (StrImOriPat != GDCM_UNFOUND) {
    if( sscanf( StrImOriPat.c_str(), "%f\\%f\\%f\\%f\\%f\\%f", 
            &iop[0], &iop[1], &iop[2], &iop[3], &iop[4], &iop[5]) != 6) {
         dbg.Verbose(0, "gdcmHeader::GetImageOrientationPatient: wrong Image Orientation Patient (0020,0035)");
         return ;  // bug in the element 0x0020,0x0035
    } 
    else
      return ;
  }
}

//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief add a gdcmFile to the list based on file name
  */
void gdcmSerieHeaderHelper::AddFileName(std::string filename)
{
  gdcmHeaderHelper *GdcmFile = new gdcmHeaderHelper( filename.c_str() );
  this->CoherentGdcmFileList.push_back( GdcmFile );
}
//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief add a gdcmFile to the list
  */
void gdcmSerieHeaderHelper::AddGdcmFile(gdcmHeaderHelper *file)
{
  this->CoherentGdcmFileList.push_back( file );
}
//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief \todo
  */
void gdcmSerieHeaderHelper::SetDirectory(std::string dir)
{
  std::list<std::string> filenames_list;
  GetDir(dir, filenames_list);  //OS specific
  
  for(std::list<std::string>::iterator it = filenames_list.begin(); it !=
  filenames_list.end(); it++)
  {
    gdcmHeaderHelper *file = new gdcmHeaderHelper( it->c_str() );
    this->CoherentGdcmFileList.push_back( file );
  }
}
//----------------------------------------------------------------------------
//This could be implemented in a 'Strategy Pattern' approach
//But as I don't know how to do it, I leave it this way
//BTW, this is also a Strategy, I don't know this is the best approach :)
void gdcmSerieHeaderHelper::OrderGdcmFileList()
{
  if( ImagePositionPatientOrdering() )
  {
    return ;
  }
  else if( ImageNumberOrdering() )
  {
    return ;
  }
  else
  {
    FileNameOrdering();
  }
}
//----------------------------------------------------------------------------
/**
  * \ingroup gdcmHeaderHelper
  * \brief 
    We may order, considering :
      -# Image Number
      -# Image Position Patient
      -# More to come :)
*/
//based on Jolinda's algorithm
bool gdcmSerieHeaderHelper::ImagePositionPatientOrdering()
{
  //iop is calculated based on the file file
  float *cosines = new float[6];
  float normal[3];
  float ipp[3];
  float dist;
  float min, max;
  bool first = true;
  int n=0;
  std::vector<float> distlist;

  //!\todo rewrite this for loop.
  for (std::list<gdcmHeaderHelper*>::iterator it  = CoherentGdcmFileList.begin();
        it != CoherentGdcmFileList.end(); it++)
  {
    if(first) {
      (*it)->GetImageOrientationPatient(cosines);
      
      //You only have to do this once for all slices in the volume. Next, for
      //each slice, calculate the distance along the slice normal using the IPP
      //tag ("dist" is initialized to zero before reading the first slice) :
      normal[0] = cosines[1]*cosines[5] - cosines[2]*cosines[4];
      normal[1] = cosines[2]*cosines[3] - cosines[0]*cosines[5];
      normal[2] = cosines[0]*cosines[4] - cosines[1]*cosines[3];
  
      ipp[0] = (*it)->GetXOrigin();
      ipp[1] = (*it)->GetYOrigin();
      ipp[2] = (*it)->GetZOrigin();

      dist = 0;
      for (int i = 0; i < 3; ++i)
          dist += normal[i]*ipp[i];
    
      if( dist == 0 )
      {
        delete[] cosines;
        return false;
      }

      distlist.push_back( dist );

      max = min = dist;
      first = false;
    }
    else {
      ipp[0] = (*it)->GetXOrigin();
      ipp[1] = (*it)->GetYOrigin();
      ipp[2] = (*it)->GetZOrigin();
  
      dist = 0;
      for (int i = 0; i < 3; ++i)
          dist += normal[i]*ipp[i];

      if( dist == 0 )
      {
        delete[] cosines;
        return false;
      }

      
      distlist.push_back( dist );

      min = (min < dist) ? min : dist;
      max = (max > dist) ? max : dist;
    }
    n++;
  }

    //Then I order the slices according to the value "dist". Finally, once
    //I've read in all the slices, I calculate the z-spacing as the difference
    //between the "dist" values for the first two slices.
    std::vector<gdcmHeaderHelper*> CoherentGdcmFileVector(n);
    //CoherentGdcmFileVector.reserve( n );
    CoherentGdcmFileVector.resize( n );
    //assert( CoherentGdcmFileVector.capacity() >= n );

    float step = (max - min)/(n - 1);
    int pos;
    n = 0;
    
    //VC++ don't understand what scope is !! it -> it2
    for (std::list<gdcmHeaderHelper*>::iterator it2  = CoherentGdcmFileList.begin();
        it2 != CoherentGdcmFileList.end(); it2++, n++)
    {
      //2*n sort algo !!
      //Assumption: all files are present (no one missing)
      pos = (int)( fabs( (distlist[n]-min)/step) + .5 );
            
      CoherentGdcmFileVector[pos] = *it2;
    }

  CoherentGdcmFileList.clear();  //this doesn't delete list's element, node only
  
  //VC++ don't understand what scope is !! it -> it3
  for (std::vector<gdcmHeaderHelper*>::iterator it3  = CoherentGdcmFileVector.begin();
        it3 != CoherentGdcmFileVector.end(); it3++)
  {
    CoherentGdcmFileList.push_back( *it3 );
  }

  distlist.clear();
  CoherentGdcmFileVector.clear();
  delete[] cosines;
  
  return true;
}
//----------------------------------------------------------------------------
//Based on Image Number
bool gdcmSerieHeaderHelper::ImageNumberOrdering()
{
  int min, max, pos;
  int n = 0;//CoherentGdcmFileList.size(); //O(N) operation !!
  unsigned char *partition;
  
  std::list<gdcmHeaderHelper*>::iterator it  = CoherentGdcmFileList.begin();
  min = max = (*it)->GetImageNumber();

  for (; it != CoherentGdcmFileList.end(); it++, n++)
  {
    pos = (*it)->GetImageNumber();

    //else
    min = (min < pos) ? min : pos;
  }

  //bzeros(partition, n); //Cette fonction est déconseillée, utilisez plutôt memset.
  partition = new unsigned char[n];
  memset(partition, 0, n);

  std::vector<gdcmHeaderHelper*> CoherentGdcmFileVector(n);

  //VC++ don't understand what scope is !! it -> it2
  for (std::list<gdcmHeaderHelper*>::iterator it2  = CoherentGdcmFileList.begin();
        it2 != CoherentGdcmFileList.end(); it2++)
  {
    pos = (*it2)->GetImageNumber();
    CoherentGdcmFileVector[pos - min] = *it2;
    partition[pos - min]++;
  }
  
  unsigned char mult;
  for(int i=0; i<n ; i++)
  {
    mult *= partition[i];
  }

  //VC++ don't understand what scope is !! it -> it3
  CoherentGdcmFileList.clear();  //this doesn't delete list's element, node only
  for (std::vector<gdcmHeaderHelper*>::iterator it3  = CoherentGdcmFileVector.begin();
        it3 != CoherentGdcmFileVector.end(); it3++)
  {
    CoherentGdcmFileList.push_back( *it3 );
  }
  CoherentGdcmFileVector.clear();
  
  delete[] partition;
  return mult;
}
//----------------------------------------------------------------------------
bool gdcmSerieHeaderHelper::FileNameOrdering()
{
  //using the sort
  //sort(CoherentGdcmFileList.begin(), CoherentGdcmFileList.end());
  return true;
}
//----------------------------------------------------------------------------
std::list<gdcmHeaderHelper*> &gdcmSerieHeaderHelper::GetGdcmFileList()
{
  return CoherentGdcmFileList;
}
//----------------------------------------------------------------------------
