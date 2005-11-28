/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmVR.h,v $
  Language:  C++
  Date:      $Date: 2005/11/28 16:31:24 $
  Version:   $Revision: 1.28 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMVR_H
#define GDCMVR_H

#include "gdcmRefCounter.h"
#include "gdcmVRKey.h"

#include <map>
#include <string>
#include <iostream>

namespace gdcm 
{

//-----------------------------------------------------------------------------
typedef std::string VRAtr;
/// Value Representation Hash Table
typedef std::map<VRKey, VRAtr> VRHT;

//-----------------------------------------------------------------------------
/**
 * \brief Container for dicom Value Representation Hash Table
 * \note   This is a singleton
 */
class GDCM_EXPORT VR : public RefCounter
{
   gdcmTypeMacro(VR);

public:
   static VR *New() {return new VR();}

   virtual void Print(std::ostream &os = std::cout, 
                      std::string const &indent = "" );

   /// \brief   Get the count for an element
   int Count(VRKey const &key) { return vr.count(key); };

   bool IsVROfBinaryRepresentable(VRKey const &tested);
   bool IsVROfStringRepresentable(VRKey const &tested);

   /// \brief   Simple predicate that checks whether the given argument
   ///          corresponds to the Value Representation of a \ref SeqEntry
   bool IsVROfSequence(VRKey const &tested) { return tested == "SQ"; }

// Remove inline optimization for VS6
#if defined(_MSC_VER) && (_MSC_VER == 1200)
   bool IsValidVR(VRKey const &key);
#else
/// \brief checks is a supposed-to-be VR is a 'legal' one.
   bool IsValidVR(VRKey const &key) { return vr.find(key) != vr.end(); }
#endif

   unsigned short GetAtomicElementLength(VRKey const &tested);

protected:
   VR();
   ~VR();

private:
   VRHT vr;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
