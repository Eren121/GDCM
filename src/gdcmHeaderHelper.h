/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmHeaderHelper.h,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:07 $
  Version:   $Revision: 1.21 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
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

   /// \todo should return bool or throw error ?
   void AddFileName(std::string const & filename);
   void AddGdcmFile(gdcmHeader *file);
   void SetDirectory(std::string const & dir);
   void OrderGdcmFileList();
   
   /// \warning Assumes all elements in the list have the same global infos.
   ///          Assumes the list is not empty.
   gdcmHeader* GetGdcmHeader() { return CoherentGdcmFileList.front(); }

   typedef std::list<gdcmHeader* > GdcmHeaderList;

   /// \brief Gets the *coherent* File List
   /// @return the *coherent* File List
   const GdcmHeaderList& GetGdcmFileList() { return CoherentGdcmFileList; }

private:
   bool ImagePositionPatientOrdering();
   bool ImageNumberOrdering();
   bool FileNameOrdering();
   
   GdcmHeaderList CoherentGdcmFileList;
};

//-----------------------------------------------------------------------------
#endif
