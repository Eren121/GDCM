// $Header: /cvs/public/gdcm/src/Attic/gdcmHeaderHelper.h,v 1.1 2003/09/08 16:37:47 malaterre Exp $

#ifndef GDCMHEADERHELPER_H
#define GDCMHEADERHELPER_H

#include "gdcmHeader.h"
#include <list>
#include <string>
#include <vector>

   enum ModalityType {
      Unknow,
      CR,
      CT,
      MR,
      NM,
      OT,
      US,
      XA };
      
/**
  This class is meant to *interprate* data given from gdcmHeader
  That is to say :
   * it will help other dev to link against there lib
   * return value instead of string
   * will be able to search for data at some other place
   * return *default value* which is not a gdcmHeader goal
   * ...
*/
class GDCM_EXPORT gdcmHeaderHelper : public gdcmHeader {

public:

   gdcmHeaderHelper::gdcmHeaderHelper();
   gdcmHeaderHelper::gdcmHeaderHelper(const char *filename, bool exception_on_error = false);

    //Ces fonction releve de l'interpretation des données de gdcmHeader
   int GetPixelSize();
   std::string GetPixelType();
   
   float GetXSpacing();
   float GetYSpacing();
   float GetZSpacing();
   
   std::string GetStudyUID();
   std::string GetSeriesUID();
   std::string GetClassUID();
   std::string GetInstanceUID();
   
    /**
    change GetXImagePosition -> GetXOrigin in order not to confused reader
      -# GetXOrigin can return default value (=0) if it was not ImagePosition
      -# Image Position is different in dicomV3 <> ACR NEMA -> better use generic
      name
    */
   float GetXOrigin();
   float GetYOrigin();
   float GetZOrigin();
   
   int GetImageNumber();
   ModalityType GetModality();
   
   void GetImageOrientationPatient( float* iop );

};

/**
This class should be used for a stack of 2D dicom images.
For a multiframe dicom image better use directly gdcmHeaderHelper
*/
class GDCM_EXPORT gdcmSerieHeaderHelper {

public:
    gdcmSerieHeaderHelper::gdcmSerieHeaderHelper() {};
    gdcmSerieHeaderHelper::~gdcmSerieHeaderHelper();

   void AddFileName(std::string filename); //should return bool or throw error ?
   void AddGdcmFile(gdcmHeaderHelper *file);
   void SetDirectory(std::string dir);
   void OrderGdcmFileList();
   
   gdcmHeaderHelper *GetGdcmHeader()
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

#endif
