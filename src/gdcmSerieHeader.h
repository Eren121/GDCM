/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSerieHeader.h,v $
  Language:  C++
  Date:      $Date: 2004/12/03 20:16:58 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMSERIEHEADER_H
#define GDCMSERIEHEADER_H

#include "gdcmCommon.h"
#include <list>

namespace gdcm 
{
class Header;
//-----------------------------------------------------------------------------
/*
 * \defgroup SerieHeader
 * \brief  
 *
 * - This class should be used for a stack of 2D dicom images.
 * - For a multiframe dicom image better use directly SerieHeader
*/
class GDCM_EXPORT SerieHeader 
{
public:
    SerieHeader();
    ~SerieHeader();

   /// \todo should return bool or throw error ?
   void AddFileName(std::string const & filename);
   void AddGdcmFile(Header *file);
   void SetDirectory(std::string const & dir);
   void OrderGdcmFileList();
   
   /// \warning Assumes all elements in the list have the same global infos.
   ///          Assumes the list is not empty.
   Header* GetGdcmHeader() { return CoherentGdcmFileList.front(); }

   typedef std::list<Header* > GdcmHeaderList;

   /// \brief Gets the *coherent* File List
   /// @return the *coherent* File List
   const GdcmHeaderList& GetGdcmFileList() { return CoherentGdcmFileList; }

private:
   bool ImagePositionPatientOrdering();
   bool ImageNumberOrdering();
   bool FileNameOrdering();
   
   GdcmHeaderList CoherentGdcmFileList;
};

} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
