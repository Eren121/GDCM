/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDebug.cxx,v $
  Language:  C++
  Date:      $Date: 2004/08/01 02:39:09 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include <iostream>
#include "gdcmDebug.h"

//-----------------------------------------------------------------------------
gdcmDebug gdcmDebug::debug;

//-----------------------------------------------------------------------------
/**
 * \brief   constructor
 * @param level debug level
 */ 
gdcmDebug::gdcmDebug(int level) 
{
   DebugLevel = level;
}

/**
 * \brief   Accessor
 * @param   level Set the debug level
 */ 
void gdcmDebug::SetDebug(int level) 
{
   DebugLevel = level;
}

/**
 * \brief   Verbose 
 * @param Level level
 * @param Msg1 first message part
 * @param Msg2 second message part 
 */
void gdcmDebug::Verbose(int level, const char * msg1, const char * msg2) 
{
   if (level > DebugLevel)
   {
      return ;
   }
   std::cerr << msg1 << ' ' << msg2 << std::endl;
}

/**
 * \brief   Error 
 * @param Test test
 * @param Msg1 first message part
 * @param Msg2 second message part 
 */
void gdcmDebug::Error(bool test, const char * msg1, const char * msg2) 
{
   if (!test)
   {
      return;
   }
   std::cerr << msg1 << ' ' << msg2 << std::endl;
   Exit(1);
}

/**
 * \brief   Error 
 * @param Msg1 first message part
 * @param Msg2 second message part
 * @param Msg3 Third message part  
 */
void gdcmDebug::Error(const char* msg1, const char* msg2,
                      const char* msg3) 
{
   std::cerr << msg1 << ' ' << msg2 << ' ' << msg3 << std::endl;
   Exit(1);
}

/**
 * \brief   Assert 
 * @param Level level 
 * @param Test test
 * @param Msg1 first message part
 * @param Msg2 second message part
 */
void gdcmDebug::Assert(int level, bool test, const char * msg1, 
                       const char * msg2) 
{
   if (level > DebugLevel)
   {
      return ;
   }
   if (!test)
   {
      std::cerr << msg1 << ' ' << msg2 << std::endl;
   }
}

/**
 * \brief   Exit 
 * @param a return code 
 */
void gdcmDebug::Exit(int a) 
{
#ifdef __GNUC__
   std::exit(a);
#endif
#ifdef _MSC_VER
   exit(a);    // Found in #include <stdlib.h>
#endif
}

/**
 * \brief  Get the debug instance 
 * \return Reference to the debug instance
 */
gdcmDebug &gdcmDebug::GetReference()
{
   return gdcmDebug::debug;
}

