/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmVR.h,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:48 $
  Version:   $Revision: 1.9 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
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

//-----------------------------------------------------------------------------
typedef std::string VRKey;
typedef std::string VRAtr;
typedef std::map<VRKey, VRAtr> VRHT;    // Value Representation Hash Table

//-----------------------------------------------------------------------------
/*
 * Container for dicom Value Representation Hash Table
 * \note   This is a singleton
 */
class GDCM_EXPORT gdcmVR 
{
public:
   gdcmVR(void);
   ~gdcmVR();

   void Print(std::ostream &os = std::cout);
   int Count(VRKey key);
   bool IsVROfGdcmBinaryRepresentable(VRKey);
   bool IsVROfGdcmStringRepresentable(VRKey);

private:
   VRHT vr;
};

//-----------------------------------------------------------------------------
#endif
