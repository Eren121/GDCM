#include <iostream>
#include "gdcmDebug.h"

/**
 * \ingroup Globals
 * \brief   Instance of debugging utility.
 */
gdcmDebug dbg;

/**
 * \ingroup gdcmDebug
 * \brief   constructor
 * @param level debug level
 */ 
gdcmDebug::gdcmDebug(int level) {
   DebugLevel = level;
}

/**
 * \ingroup gdcmDebug
 * \brief   Accessor
 * @param   level Set the debug level
 */ 
void gdcmDebug::SetDebug(int level) {
   DebugLevel = level;
}

/**
 * \ingroup gdcmDebug
 * \brief   Verbose 
 * @param Level level
 * @param Msg1 first message part
 * @param Msg2 second message part 
 */
void gdcmDebug::Verbose(int Level, const char * Msg1, const char * Msg2) {
   if (Level > DebugLevel)
      return ;
   std::cerr << Msg1 << ' ' << Msg2 << std::endl;
}

/**
 * \ingroup gdcmDebug
 * \brief   Error 
 * @param Test test
 * @param Msg1 first message part
 * @param Msg2 second message part 
 */
void gdcmDebug::Error( bool Test, const char * Msg1, const char * Msg2) {
   if (!Test)
      return;
   std::cerr << Msg1 << ' ' << Msg2 << std::endl;
   Exit(1);
}

/**
 * \ingroup gdcmDebug
 * \brief   Error 
 * @param Msg1 first message part
 * @param Msg2 second message part
 * @param Msg3 Third message part  
 */
void gdcmDebug::Error(const char* Msg1, const char* Msg2,
                      const char* Msg3) {
   std::cerr << Msg1 << ' ' << Msg2 << ' ' << Msg3 << std::endl;
   Exit(1);
}

/**
 * \ingroup gdcmDebug
 * \brief   Assert 
 * @param Level level 
 * @param Test test
 * @param Msg1 first message part
 * @param Msg2 second message part
 */
 void gdcmDebug::Assert(int Level, bool Test,
                 const char * Msg1, const char * Msg2) {
   if (Level > DebugLevel)
      return ;
   if (!Test)
      std::cerr << Msg1 << ' ' << Msg2 << std::endl;
}

/**
 * \ingroup gdcmDebug
 * \brief   Exit 
 * @param a return code 
 */
void gdcmDebug::Exit(int a) {
#ifdef __GNUC__
   std::exit(a);
#endif
#ifdef _MSC_VER
   exit(a);    // Found in #include <stdlib.h>
#endif
}
