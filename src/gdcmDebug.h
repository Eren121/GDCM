// gdcmDebug.h
//-----------------------------------------------------------------------------
#ifndef GDCMDEBUG_H
#define GDCMDEBUG_H

#define GDCM_DEBUG -1

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
class gdcmDebug {
public:
   gdcmDebug(int level = GDCM_DEBUG);

   void SetDebug (int level);
   void Verbose(int, const char*, const char* ="");
   void Error(bool, const char*,  const char* ="");
   void Error(const char*, const char* ="", const char* ="");

   void Assert(int, bool, const char*, const char*);
   void Exit(int);

private:
/// warning message level to be displayed
   int DebugLevel;
};

extern gdcmDebug dbg;

#endif
