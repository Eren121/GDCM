/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmHeaderHelper.h,v $
  Language:  C++
  Date:      $Date: 2004/06/21 04:18:26 $
  Version:   $Revision: 1.17 $
                                                                                
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

class GDCM_EXPORT gdcmHeaderHelper : public gdcmHeader
{
};

/*
 * \defgroup gdcmSerieHeader
 * \brief  
 *
 * - This class should be used for a stack of 2D dicom images.
 * - For a multiframe dicom image better use directly gdcmHeaderHelper
*/
class GDCM_EXPORT gdcmSerieHeader {
public:
    gdcmSerieHeader() {};
    ~gdcmSerieHeader();

   void AddFileName(std::string filename); //should return bool or throw error ?
   void AddGdcmFile(gdcmHeader *file);
   void SetDirectory(std::string dir);
   void OrderGdcmFileList();
   
   inline gdcmHeader *GetGdcmHeader()
   {
      //Assume all element in the list have the same global infos
      return CoherentGdcmFileList.front();
   }
   
   std::list<gdcmHeader*>& GetGdcmFileList();

private:
   bool ImagePositionPatientOrdering();
   bool ImageNumberOrdering();
   bool FileNameOrdering();
   
   std::list<gdcmHeader*> CoherentGdcmFileList;
};

//-----------------------------------------------------------------------------
#endif
