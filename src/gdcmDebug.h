/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDebug.h,v $
  Language:  C++
  Date:      $Date: 2005/01/07 16:39:59 $
  Version:   $Revision: 1.12 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDEBUG_H
#define GDCMDEBUG_H

#include "gdcmCommon.h"

namespace gdcm 
{
//-----------------------------------------------------------------------------

/**
 * \ingroup Debug
 * \brief Debug is an object for debugging in program.
 * It has 2 debugging modes :
 *  - error : for bad library use
 *  - debug : for debugging messages
 * 
 * A debugging message has a level of priority and is 
 * Shown only when the debug level is higher than the 
 * message level.
 */
class GDCM_EXPORT Debug
{
public:
   /// This is a global flag that controls whether any debug, warning
   /// messages are displayed.
   static void SetDebugLevel (int level);
   static int  GetDebugLevel ();

   static void Verbose(int level, const char *msg1, const char *msg2 = "") ;
   static void Error  (bool test, const char *msg1, const char *msg2 = "");
   static void Error  (const char *msg1, const char *msg2 = "", const char *msg3 = "");

   static void Assert(int level, bool test, const char *msg1, const char *msg2);
   static void Exit  (int a);
};

} // end namespace gdcm

#endif
