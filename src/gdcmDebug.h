/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDebug.h,v $
  Language:  C++
  Date:      $Date: 2004/09/24 03:34:27 $
  Version:   $Revision: 1.6 $
                                                                                
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
class GDCM_EXPORT gdcmDebug
{
public:
   gdcmDebug(int level = -1);

   void SetDebug (int level);
   void Verbose(int level, const char* msg1, const char* msg2 = "") ;
   void Error  (bool test, const char* msg1, const char* msg2 = "");
   void Error  (const char* msg1, const char* msg2 = "", const char* msg3 = "");

   void Assert(int level, bool test, const char * msg1, const char * msg2);
   void Exit(int a);

   static gdcmDebug &GetReference();

private:
/// warning message level to be displayed
   int DebugLevel;

};

/// Instance of debugging utility.
static gdcmDebug dbg;

#endif
