/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDebug.h,v $
  Language:  C++
  Date:      $Date: 2004/07/19 11:51:26 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDEBUG_H
#define GDCMDEBUG_H

#include "gdcmCommon.h"

//-----------------------------------------------------------------------------
#define dbg gdcmDebug::GetReference()

//-----------------------------------------------------------------------------

/**
 * \ingroup gdcmDebug
 * \brief gdcmDebug is an object for debugging in program.
 * It has 2 debugging modes :
 *  - error : for bad library use
 *  - debug : for debugging messages
 * 
 * A debugging message has a level of priority and is 
 * Shown only when the debug level is higher than the 
 * message level.
 */
class GDCM_EXPORT gdcmDebug {
public:
   gdcmDebug(int level = -1);

   void SetDebug (int level);
   void Verbose(int, const char*, const char* ="");
   void Error(bool, const char*,  const char* ="");
   void Error(const char*, const char* ="", const char* ="");

   void Assert(int, bool, const char*, const char*);
   void Exit(int);

   static gdcmDebug &GetReference();

private:
/// warning message level to be displayed
   int DebugLevel;

/// Instance of debugging utility.
   static gdcmDebug debug;
};

#endif
