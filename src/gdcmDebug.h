/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDebug.h,v $
  Language:  C++
  Date:      $Date: 2005/01/07 19:20:38 $
  Version:   $Revision: 1.13 $
                                                                                
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
#ifndef __FUNCTION__
#  define __FUNCTION__ ""
#endif

/**
 * \brief   Verbose 
 * @param level level
 * @param msg1 first message part
 * @param msg2 second message part 
 */
#define gdcmDebugMacro(x)                                  \
{                                                          \
   if( gdcm::Debug::GetDebugFlag() )                       \
   {                                                       \
   std::ostringstream osmacro;                             \
   osmacro << "Debug: In " __FILE__ ", line " << __LINE__  \
      << ", function " << __FUNCTION__ << '\n'             \
      << x << "\n\n";                                      \
   std::cerr << osmacro.str() << std::endl;                \
   }                                                       \
}

/**
 * \brief   Verbose 
 * @param level level
 * @param msg1 first message part
 * @param msg2 second message part 
 */
#define gdcmVerboseMacro(x)                                 \
{                                                           \
   if( gdcm::Debug::GetDebugFlag() )                        \
   {                                                        \
   std::ostringstream osmacro;                              \
   osmacro << "Verbose: In " __FILE__ ", line " << __LINE__ \
      << ", function " __FUNCTION__ "\n"                    \
      << x << "\n\n";                                       \
   std::cerr << osmacro.str() << std::endl;                 \
   }                                                        \
}

/**
 * \brief   Error 
 * @param test test
 * @param msg1 first message part
 * @param msg2 second message part 
 */
#define gdcmErrorMacro(x)                                 \
{                                                         \
   if( gdcm::Debug::GetDebugFlag() )                      \
   {                                                      \
   std::ostringstream osmacro;                            \
   osmacro << "Error: In " __FILE__ ", line " << __LINE__ \
      << ", function " << __FUNCTION__ << '\n'            \
      << x << "\n\n";                                     \
   std::cerr << osmacro.str() << std::endl;               \
   exit(1);                                               \
   }                                                      \
}

/**
 * \brief   Assert 
 * @param level level 
 * @param test test
 * @param msg1 first message part
 * @param msg2 second message part
 */
#define gdcmAssertMacro(x)                                 \
{                                                          \
   if( x )                                                 \
   {                                                       \
   std::ostringstream osmacro;                             \
   osmacro << "Assert: In " __FILE__ ", line " << __LINE__ \
                      << ", function " << __FUNCTION__     \
                      << "\n\n";                           \
   std::cerr << osmacro.str() << std::endl;                \
   assert ( x );                                           \
   }                                                       \
}

#endif
