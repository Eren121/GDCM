/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmVR.h,v $
  Language:  C++
  Date:      $Date: 2005/01/06 17:16:16 $
  Version:   $Revision: 1.16 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMVR_H
#define GDCMVR_H

#include "gdcmCommon.h"
#include <map>
#include <string>
#include <iostream>

namespace gdcm 
{

//-----------------------------------------------------------------------------
typedef std::string VRKey;
typedef std::string VRAtr;
/// Value Representation Hash Table
typedef std::map<VRKey, VRAtr> VRHT;

//-----------------------------------------------------------------------------
/*
 * Container for dicom Value Representation Hash Table
 * \note   This is a singleton
 */
class GDCM_EXPORT VR 
{
public:
   VR(void);
   ~VR();

   void Print(std::ostream &os = std::cout);
   int Count(VRKey const & key);
   bool IsVROfBinaryRepresentable(VRKey const & tested);
   bool IsVROfStringRepresentable(VRKey const & tested);
   bool IsVROfSequence(VRKey const & tested);

   bool IsValidVR(VRKey const & key);

private:
   VRHT vr;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
