/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDebug.h,v $
  Language:  C++
  Date:      $Date: 2005/01/07 20:31:36 $
  Version:   $Revision: 1.14 $
                                                                                
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
 *  - error : for bad library use, seriously wrong DICOM
 *  - debug : for information/debug messages
 *  - warning : for warning about DICOM quality (kosher)
 * 
 * A debugging message is only show if the flag is on (DebugFlag)
 * This is static var and can be set at begining of code:
 *         gdcm::Debug::SetDebugOn();
 */
class GDCM_EXPORT Debug
{
public:
   /// This is a global flag that controls whether any debug, warning
   /// messages are displayed.
   static int  GetDebugFlag ();
   static void SetDebugFlag (int flag);
   static void SetDebugOn  () { SetDebugFlag(1); };
   static void SetDebugOff () { SetDebugFlag(0); };
};

} // end namespace gdcm

// Here we define function this is the only way to be able to pass
// stuff with indirection like:
// gdcmDebug( "my message:" << i << '\t' ); 
// You cannot use function unless you use vnsprintf ...

// __FUNCTION is not always defined by preprocessor
// In c++ we should use __PRETTY_FUNCTION__ instead...
#ifndef __GNUC__
#  define __FUNCTION__ "<unknow>"
#endif

/**
 * \brief   Debug
 * @param msg message part
 */
#define gdcmDebugMacro(msg)                                \
{                                                          \
   if( gdcm::Debug::GetDebugFlag() )                       \
   {                                                       \
   std::ostringstream osmacro;                             \
   osmacro << "Debug: In " __FILE__ ", line " << __LINE__  \
           << ", function " << __FUNCTION__ << '\n'        \
           << msg << "\n\n";                               \
   std::cerr << osmacro.str() << std::endl;                \
   }                                                       \
}

/**
 * \brief   Verbose 
 * @param msg message part
 */
#define gdcmVerboseMacro(msg)                               \
{                                                           \
   if( gdcm::Debug::GetDebugFlag() )                        \
   {                                                        \
   std::ostringstream osmacro;                              \
   osmacro << "Verbose: In " __FILE__ ", line " << __LINE__ \
           << ", function " << __FUNCTION__ << "\n"         \
           << msg << "\n\n";                                \
   std::cerr << osmacro.str() << std::endl;                 \
   }                                                        \
}

/**
 * \brief   Error 
 * @param msg second message part 
 */
#define gdcmErrorMacro(msg)                               \
{                                                         \
   if( gdcm::Debug::GetDebugFlag() )                      \
   {                                                      \
   std::ostringstream osmacro;                            \
   osmacro << "Error: In " __FILE__ ", line " << __LINE__ \
           << ", function " << __FUNCTION__ << '\n'       \
           << msg << "\n\n";                              \
   std::cerr << osmacro.str() << std::endl;               \
   exit(1);                                               \
   }                                                      \
}

/**
 * \brief   Assert 
 * @param arg argument to test
 *        An easy solution to pass also a message is to do:
 *        gdcmAssertMacro( "my message" && 2 < 3 )
 */
#define gdcmAssertMacro(arg)                               \
{                                                          \
   if( arg )                                               \
   {                                                       \
   std::ostringstream osmacro;                             \
   osmacro << "Assert: In " __FILE__ ", line " << __LINE__ \
           << ", function " << __FUNCTION__                \
           << "\n\n";                                      \
   std::cerr << osmacro.str() << std::endl;                \
   assert ( arg );                                         \
   }                                                       \
}

#endif
