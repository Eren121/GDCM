/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmHeaderHelper.h,v $
  Language:  C++
  Date:      $Date: 2004/06/25 20:48:25 $
  Version:   $Revision: 1.19 $
                                                                                
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
/*
 * \defgroup gdcmSerieHeader
 * \brief  
 *
 * - This class should be used for a stack of 2D dicom images.
 * - For a multiframe dicom image better use directly gdcmHeaderHelper
*/
class GDCM_EXPORT gdcmSerieHeader 
{
public:
    gdcmSerieHeader();
    ~gdcmSerieHeader();

   void AddFileName(std::string const & filename); //should return bool or throw error ?
   void AddGdcmFile(gdcmHeader *file);
   void SetDirectory(std::string const & dir);
   void OrderGdcmFileList();
   
   inline gdcmHeader *GetGdcmHeader()
   {
      // Assume all element in the list have the same global infos
      // Assume the list is not empty
      return CoherentGdcmFileList.front();
   }

   typedef std::list<gdcmHeader* > GdcmHeaderList;
   /**
    * \brief Gets the *coherent* File List
    * @return the *coherent* File List
    */
   const GdcmHeaderList& GetGdcmFileList()
   {
     return CoherentGdcmFileList;
   }

private:
   bool ImagePositionPatientOrdering();
   bool ImageNumberOrdering();
   bool FileNameOrdering();
   
   GdcmHeaderList CoherentGdcmFileList;
};

//-----------------------------------------------------------------------------
#endif
