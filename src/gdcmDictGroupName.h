/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDictGroupName.h,v $
  Language:  C++
  Date:      $Date: 2005/04/05 10:56:25 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICTGROUPNAME_H
#define GDCMDICTGROUPNAME_H

#include "gdcmCommon.h"
#include <map>
#include <string>
#include <iostream>

namespace gdcm 
{

//-----------------------------------------------------------------------------
typedef std::string GroupName;
/// Group Name Hash Table
typedef std::map<uint16_t, GroupName> DictGroupNameHT;

//-----------------------------------------------------------------------------
/**
 * \brief Container for dicom Value Representation Hash Table
 * \note   This is a singleton
 */
class GDCM_EXPORT DictGroupName 
{
public:
   DictGroupName(void);
   ~DictGroupName();

   void Print(std::ostream &os = std::cout);

   const GroupName &GetName(uint16_t group);

private:
   DictGroupNameHT groupName;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
