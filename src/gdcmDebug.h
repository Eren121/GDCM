/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDebug.h,v $
  Language:  C++
  Date:      $Date: 2005/02/11 20:14:55 $
  Version:   $Revision: 1.33 $
                                                                                
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

#include <sstream>
#include <fstream>
#include <assert.h>
#include <errno.h>

namespace gdcm 
{
//-----------------------------------------------------------------------------

/**
 * \brief Debug is an object for debugging in program.
 * It has 2 debugging modes :
 *  - error : for bad library use, seriously wrong DICOM
 *  - debug : for information/debug messages
 *  - warning : for warning about DICOM quality (kosher)
 * 
 * A debugging message is only show if the flag is on (DebugFlag)
 * This is static var and can be set at beginning of code:
 *         gdcm::Debug::SetDebugOn();
 */
class GDCM_EXPORT Debug
{
public:
   Debug();
   ~Debug();

   /// \brief This is a global flag that controls whether any debug, warning
   ///        messages are displayed.
   static void SetDebugFlag (bool flag);
   static bool GetDebugFlag ();
   /// \brief Sets the Debug Flag to true
   static void DebugOn  () { SetDebugFlag(true);  };
   /// \brief Sets the Debug Flag to false
   static void DebugOff () { SetDebugFlag(false); };

   /// \brief This is a global flag that controls if debug are redirected
   ///        to a file or not
   static void SetDebugToFile (bool flag);
   static bool GetDebugToFile ();
   /// \brief Next debug messages will be sent in the debug file
   static void DebugToFileOn  () { SetDebugToFile(true);  };
   /// \brief Next debug messages will be sent in the standard output
   static void DebugToFileOff () { SetDebugToFile(false); };

   static void SetDebugFilename (std::string const &filename);

   static std::ofstream &GetDebugFile ();
};

} // end namespace gdcm

// Here we define function this is the only way to be able to pass
// stuff with indirection like:
// gdcmDebug( "my message:" << i << '\t' ); 
// You cannot use function unless you use vnsprintf ...

// __FUNCTION is not always defined by preprocessor
// In c++ we should use __PRETTY_FUNCTION__ instead...
#ifdef GDCM_COMPILER_HAS_FUNCTION
// Handle particular case for GNU C++ which also defines __PRETTY_FUNCTION__
// which is a lot nice in C++
#ifdef __BORLANDC__
#  define __FUNCTION__ __FUNC__
#endif
#ifdef __GNUC__
#  define GDCM_FUNCTION __PRETTY_FUNCTION__
#else
#  define GDCM_FUNCTION __FUNCTION__ 
#endif //__GNUC__
#else
#  define GDCM_FUNCTION "<unknow>"
#endif //GDCM_COMPILER_HAS_FUNCTION

/**
 * \brief   Debug
 * @param msg message part
 */
#ifdef NDEBUG
#define gdcmDebugMacro(msg)
#else
#define gdcmDebugMacro(msg)                                 \
{                                                           \
   if( Debug::GetDebugFlag() )                              \
   {                                                        \
   std::ostringstream osmacro;                              \
   osmacro << "Debug: In " __FILE__ ", line " << __LINE__   \
           << ", function " << GDCM_FUNCTION << '\n'        \
           << "Last system error was: " << strerror(errno)  \
           << '\n' << msg << "\n\n";                        \
   if( Debug::GetDebugToFile() )                            \
      Debug::GetDebugFile() << osmacro.str() << std::endl;  \
   else                                                     \
      std::cerr << osmacro.str() << std::endl;              \
   }                                                        \
}
#endif //NDEBUG

/**
 * \brief   Warning
 * @param msg message part
 */
#ifdef NDEBUG
#define gdcmWarningMacro(msg)
#else
#define gdcmWarningMacro(msg)                               \
{                                                           \
   if( Debug::GetDebugFlag() )                              \
   {                                                        \
   std::ostringstream osmacro;                              \
   osmacro << "Warning: In " __FILE__ ", line " << __LINE__ \
           << ", function " << GDCM_FUNCTION << "\n"        \
           << msg << "\n\n";                                \
   if( Debug::GetDebugToFile() )                            \
      Debug::GetDebugFile() << osmacro.str() << std::endl;  \
   else                                                     \
      std::cerr << osmacro.str() << std::endl;              \
   }                                                        \
}
#endif //NDEBUG

/**
 * \brief   Error 
 * @param msg second message part 
 */
#ifdef NDEBUG
#define gdcmErrorMacro(msg)
#else
#define gdcmErrorMacro(msg)                                 \
{                                                           \
   std::ostringstream osmacro;                              \
   osmacro << "Error: In " __FILE__ ", line " << __LINE__   \
           << ", function " << GDCM_FUNCTION << '\n'        \
           << msg << "\n\n";                                \
   if( Debug::GetDebugToFile() )                            \
      Debug::GetDebugFile() << osmacro.str() << std::endl;  \
   else                                                     \
      std::cerr << osmacro.str() << std::endl;              \
}
#endif //NDEBUG

/**
 * \brief   Assert 
 * @param arg argument to test
 *        An easy solution to pass also a message is to do:
 *        gdcmAssertMacro( "my message" && 2 < 3 )
 */
#ifdef NDEBUG
#define gdcmAssertMacro(arg)
#else
#define gdcmAssertMacro(arg)                                \
{                                                           \
   if( !(arg) )                                             \
   {                                                        \
   std::ostringstream osmacro;                              \
   osmacro << "Assert: In " __FILE__ ", line " << __LINE__  \
           << ", function " << GDCM_FUNCTION                \
           << "\n\n";                                       \
   if( Debug::GetDebugToFile() )                            \
      Debug::GetDebugFile() << osmacro.str() << std::endl;  \
   else                                                     \
      std::cerr << osmacro.str() << std::endl;              \
   assert ( arg );                                          \
   }                                                        \
}
#endif //NDEBUG

#endif
